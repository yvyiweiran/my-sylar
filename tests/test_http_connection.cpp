#include "sylar/http/http_connection.h"
#include "sylar/log.h"
#include "sylar/IOManager.h"
#include <iostream>

static auto g_logger = SYLAR_LOG_ROOT();

void run() {
    sylar::Address::ptr addr = sylar::Address::LookupAnyIpAddress("www.sylar.top:80");
    if(!addr) {
        SYLAR_LOG_ERROR(g_logger) << "get addr error";
        return;
    }

    sylar::Socket::ptr sock = sylar::Socket::CreateTCP(addr);
    bool rt = sock->connect(addr);
    if(!rt) {
        SYLAR_LOG_ERROR(g_logger) << "connected " << *addr << " failed";
        return;
    }
    sylar::http::HttpConnection::ptr conn(new sylar::http::HttpConnection(sock));
    sylar::http::HttpRequest::ptr req(new sylar::http::HttpRequest);
    req->setPath("/blog/");
    req->setHeader("host", "www.sylar.top");
    SYLAR_LOG_INFO(g_logger) << "req:" << std::endl << *req;

    conn->sendRequest(req);
    auto rsp = conn->recvResponse();

    if(!rsp) {
        SYLAR_LOG_ERROR(g_logger) << "recv response error";
        return;
    }
    SYLAR_LOG_INFO(g_logger) << "rsp:" << std::endl << *rsp;

//     std::ofstream ofn("rep.dat");
//     ofn << *rsp;
}

int main(int argc, char** argv) {
    sylar::IOManager iom(2);
    iom.schedule(run);
    return 0;
}