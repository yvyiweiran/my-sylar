#include "sylar/http/http_connection.h"
#include "sylar/log.h"
#include "sylar/IOManager.h"
#include <iostream>
#include "sylar/Socket.h"

static auto g_logger = SYLAR_LOG_ROOT();

void test_pool() {
    sylar::http::HttpConnectionPool::ptr pool(new sylar::http::HttpConnectionPool(
        "www.sylar.top", "", 80, 10, 1000 * 30, 20));
    
    sylar::IOManager::GetThis()->addTimer(1000, [pool](){
        auto r = pool->doGet("/", 300);
        SYLAR_LOG_INFO(g_logger) << r->toString();
    }, true);
}

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

    SYLAR_LOG_DEBUG(g_logger) << "================================";

    auto r = sylar::http::HttpConnection::DoGet("http://www.sylar.top/blog/", 300);
    SYLAR_LOG_INFO(g_logger) << "result" << r->result
        << " error=" << r->error << "\nrsp=" << (r->response ? r->response->toString() : "");

    SYLAR_LOG_DEBUG(g_logger) << "================================";

    test_pool();
}

int main(int argc, char** argv) {
    sylar::IOManager iom(2);
    iom.schedule(run);
    return 0;
}