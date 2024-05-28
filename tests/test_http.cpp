#include "sylar/http/http.h"

void test_req() {
    sylar::http::HttpRequest::ptr req(new sylar::http::HttpRequest);
    req->setHeader("host", "www.sylar.top");
    req->setBody("hello sylar");

    req->dump(std::cout) << std::endl;
}

void test_response() {
    sylar::http::HttpResponse::ptr rep(new sylar::http::HttpResponse);
    rep->setHeader("X-X", "sylar");
    rep->setBody("hello sylar");
    rep->setStatus((sylar::http::HttpStatus)400);
    rep->setClose(false);

    rep->dump(std::cout) << std::endl;
}

int main(){
    test_req();
    std::cout << "=================" << std::endl;
    test_response();
    return 0;
}