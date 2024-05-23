#include "http_parser.h"
#include "sylar/log.h"
#include "sylar/config.h"
#include <string.h>

namespace sylar {
namespace http {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

static sylar::ConfigVar<uint64_t>::ptr g_http_request_buffer_size =
    sylar::Config::Lookup("http.request.buffer_size"
        , 4 * 1024 * 1024ul, "http request buffer size");

static sylar::ConfigVar<uint64_t>::ptr g_http_request_max_body_size =
    sylar::Config::Lookup("http.request.max.body.size"
        , 64 * 1024 * 1024ul, "http request max body size");

static uint64_t s_http_request_buffer_size = 0;
static uint64_t s_http_request_max_body_size = 0;

struct _RequestSizeIniter{
    _RequestSizeIniter() {
        s_http_request_max_body_size = g_http_request_max_body_size->getValue();
        s_http_request_buffer_size = g_http_request_buffer_size->getValue();
        
        g_http_request_buffer_size->addListener(
            [](const uint64_t& ov, const uint64_t& nv) {
                s_http_request_buffer_size = nv;
            }
        );

        g_http_request_max_body_size->addListener(
            [](const uint64_t& ov, const uint64_t& nv) {
                s_http_request_max_body_size = nv;
            }
        );
    }
};

static _RequestSizeIniter _init;

void on_request_method(void* data, const char *at, size_t length) {
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    HttpMethod m = CharsToHttpMethod(at);

    if(m == HttpMethod::INVALID_METHOD) {
        SYLAR_LOG_WARN(g_logger) << "invalid http request method: "
            << std::string(at, length);
        parser->setError(1000);
        return;
    }
    parser->getDate()->setMethod(m);
}

void on_request_uri(void* data, const char *at, size_t length) {
}

void on_request_fragment(void* data, const char *at, size_t length) {
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    parser->getDate()->setFragment(std::string(at, length));
}

void on_request_path(void* data, const char *at, size_t length) {
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    parser->getDate()->setPath(std::string(at, length));
}

void on_request_query(void* data, const char *at, size_t length) {
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    parser->getDate()->setQuery(std::string(at, length));
}

void on_request_version(void* data, const char *at, size_t length) {
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    uint8_t v = 0;
    if(strncmp(at, "HTTP/1.1", length) == 0) {
        v = 0x11;
    } else if(strncmp(at, "HTTP/1.0", length) == 0){
        v = 0x10;
    } else {
        SYLAR_LOG_WARN(g_logger) << "invaild http request version: "
            << std::string(at, length);
        parser->setError(1001);
        return;
    }
    parser->getDate()->setVersion(v);
}

void on_request_header_done(void* data, const char *at, size_t length) {
    // HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
}

void on_http_field(void *data, const char *field
            ,size_t flen, const char *value, size_t vlen) {
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    if(flen == 0) {
        SYLAR_LOG_WARN(g_logger) << "invalid http request version: "
            << std::string(value, vlen);
        parser->setError(1002);
        return;
    }
    parser->getDate()->setHeader(std::string(field, flen)
                               , std::string(value, vlen));
}

HttpRequestParser::HttpRequestParser() {
    m_data.reset(new HttpRequest);
    http_parser_init(&m_parser);
    m_parser.request_method = on_request_method;
    m_parser.request_uri = on_request_uri;
    m_parser.fragment = on_request_fragment;
    m_parser.request_path = on_request_path;
    m_parser.query_string = on_request_query;
    m_parser.http_version = on_request_version;
    m_parser.header_done = on_request_header_done;
    m_parser.http_field = on_http_field;
    m_parser.data = this;
}

uint64_t HttpRequestParser::getContentLength() {
    return m_data->getHeaderAs<uint64_t>("content-length", 0);
}

// 返回1: 成功
// -1: 失败
// >0: 已处理的字节数, 且data有效数据位len - v
size_t HttpRequestParser::execute(char* data, size_t len) {
    size_t rt = http_parser_execute(&m_parser, data, len, 0);
    memmove(data, data + rt, len - rt);
    return rt;
}

int HttpRequestParser::isFinished() {
    return http_parser_finish(&m_parser);
}

int HttpRequestParser::hasError() {
    return m_error || http_parser_has_error(&m_parser);
}

void on_response_reason(void* data, const char* at, size_t length) {
    HttpReasponseParser* parser = static_cast<HttpReasponseParser*>(data);
    parser->getDate()->setReason(std::string(at, length));
}

void on_response_status(void* data, const char* at, size_t length) {
    HttpReasponseParser* parser = static_cast<HttpReasponseParser*>(data);
    HttpStatus statu = (HttpStatus)(atoi(at));
    parser->getDate()->setStatus(statu);
}

void on_response_chunk(void* data, const char* at, size_t length) {
}

void on_response_version(void* data, const char* at, size_t length) {
    HttpReasponseParser* parser = static_cast<HttpReasponseParser*>(data);
    uint8_t v = 0;
    if(strncmp(at, "HTTP/1.1", length) == 0) {
        v = 0x11;
    }else if(strncmp(at, "HTTP/1.0", length) == 0) {
        v = 0x10;
    }else {
        SYLAR_LOG_WARN(g_logger) << "invalid http response version: "
            << std::string(at, length);
        parser->setError(1001);
        return;
    }
    parser->getDate()->setVersion(v);
}

void on_response_header_done(void* data, const char* at, size_t length) {
}

void on_response_last_chunk(void* data, const char* at, size_t length) {
}

void on_response_http_field(void *data, const char *field,
        size_t flen, const char *value, size_t vlen) {
    HttpReasponseParser* parser = static_cast<HttpReasponseParser*>(data);
    if(flen == 0) {
        SYLAR_LOG_WARN(g_logger) << "invalid http response version: "
            << std::string(value, vlen);
        parser->setError(1002);
        return;
    }
    parser->getDate()->setHeader(std::string(field, flen)
                               , std::string(value, vlen));
}
HttpReasponseParser::HttpReasponseParser() 
    :m_error(0){
    m_data.reset(new HttpReasponse);
    httpclient_parser_init(&m_parser);
    m_parser.reason_phrase = on_response_reason;
    m_parser.status_code = on_response_status;
    m_parser.chunk_size = on_response_chunk;
    m_parser.http_version = on_response_version;
    m_parser.header_done = on_response_header_done;
    m_parser.last_chunk = on_response_last_chunk;
    m_parser.http_field = on_response_http_field;
    m_parser.data = this;
}

uint64_t HttpReasponseParser::getContentLength() {
    return m_data->getHeaderAs<uint64_t>("content-length", 0);
}

size_t HttpReasponseParser::execute(char* data, size_t len) {
    size_t rt = httpclient_parser_execute(&m_parser, data, len, 0);
    memmove(data, data + rt, len - rt);
    return rt;
}

int HttpReasponseParser::isFinished() {
    return httpclient_parser_finish(&m_parser);
}

int HttpReasponseParser::hasError() {
    return m_error || httpclient_parser_has_error(&m_parser);
}

}
}