#ifndef __SYLAR_HTTP_PARSER_H
#define __SYLAR_HTTP_PARSER_H

#include "http.h"
#include "http11_parser.h"
#include "httpclient_parser.h"

namespace sylar {
namespace http {

class HttpRequestParser {
public:
    using ptr = std::shared_ptr<HttpRequestParser>;;
    HttpRequestParser();
    size_t execute(char* data, size_t len);
    int isFinished() ;
    int hasError() ;

    HttpRequest::ptr getDate() const { return m_data;}
    void setError(int v) { m_error = v;}

    uint64_t getContentLength();
public:
    static uint64_t GetHttpRequestBufferSize();
    static uint64_t GetHttpRequestMaxBodySize();
private:
    http_parser m_parser;
    HttpRequest::ptr m_data;
    // 1000: invalid method
    // 1001: invalid version
    // 1002: invalid field
    int m_error;
};

class HttpResponseParser {
public:
    using ptr = std::shared_ptr<HttpResponseParser>;
    HttpResponseParser();
    size_t execute(char* data, size_t len);
    int isFinished() ;
    int hasError() ;

    void setError(int v) { m_error = v;}
    HttpResponse::ptr getDate() const { return m_data;}
    uint64_t getContentLength();
private:
    httpclient_parser m_parser;
    HttpResponse::ptr m_data;
    int m_error;
};

}
}

#endif