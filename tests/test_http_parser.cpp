#include "sylar/http/http_parser.h"
#include "sylar/log.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

const char test_request_data[] = "POST / HTTP/1.1\r\n"
                                 "Host: www.sylar.top\r\n"
                                 "Connect-Length: 10\r\n\r\n"
                                 "1234567890";

void test_request() {
    sylar::http::HttpRequestParser parser;
    std::string tmp = test_request_data;
    size_t s = parser.execute(&tmp[0], tmp.size());
    SYLAR_LOG_INFO(g_logger) << "\nexecute rt=" << s
        << " has_error=" << parser.hasError()
        << " is_finished=" << parser.isFinished()
        << " total=" << tmp.size()
        << " content-length=" <<  parser.getContentLength();
    tmp.resize(tmp.size() - s);
    SYLAR_LOG_DEBUG(g_logger) << "\n" << parser.getDate()->toString();
    SYLAR_LOG_INFO(g_logger) << tmp;
}

const char test_response_data[] = "HHTTP/1.1 200 OK\r\n"
            "Date: Tue, 21 May 2024 12:37:58 GMT\r\n"
            "Server: Apache\r\n"
            "Last-Modified: Tue, 12 Jan 2010 13:48:00 GMT\r\n"
            "ETag: \"51-47cf7e6ee8400\"\r\n"
            "Accept-Ranges: bytes\r\n"
            "Content-Length: 81\r\n"
            "Cache-Control: max-age=86400\r\n"
            "Expires: Wed, 22 May 2024 12:37:58 GMT\r\n"
            "Connection: Close\r\n"
            "Content-Type: text/html\r\n\r\n"
            "<html>\r\n"
            "<meta http-equiv=\"refresh\" content=\"0;url=http://www.baidu.com/\">\r\n"
            "</html>\r\n";

void test_response() {
    sylar::http::HttpResponseParser parser;
    std::string tmp = test_response_data;
    size_t s = parser.execute(&tmp[0], tmp.size());
    SYLAR_LOG_DEBUG(g_logger) << "execute rt=" << s
        << " has_error=" << parser.hasError()
        << " is_finished=" << parser.isFinished()
        << " total=" << tmp.size()
        << " content-length=" << parser.getContentLength();
    tmp.resize(tmp.size() - s);
    SYLAR_LOG_INFO(g_logger) << parser.getDate()->toString();
    SYLAR_LOG_INFO(g_logger) << tmp;
}

int main(int argc, char** argv) {
    test_request();
    SYLAR_LOG_INFO(g_logger) << "======================";
    test_response();
    return 0;
}