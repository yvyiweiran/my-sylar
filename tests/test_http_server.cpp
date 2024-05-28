#include "sylar/http/http_server.h"
#include "sylar/log.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void  test() {
    sylar::http::HttpServer::ptr server(new sylar::http::HttpServer);
    sylar::Address::ptr addr = sylar::Address::LookupAnyIpAddress("0.0.0.0:8020");
    while (!server->bind(addr)) {
        sleep(2);
    }
    auto sb = server->getServletDispatch();
    sb->addServlet("/sylar/xx", [](sylar::http::HttpRequest::ptr req
            , sylar::http::HttpResponse::ptr rsp
            , sylar::http::HttpSession::ptr session) {
        rsp->setBody(req->toString());
        return 0;
    });

    sb->addGlobServlet("/sylar/*", [](sylar::http::HttpRequest::ptr req
            , sylar::http::HttpResponse::ptr rsp
            , sylar::http::HttpSession::ptr seesion) {
        rsp->setBody("Glob:\r\n"+req->toString());
        return 0;
    });
    server->start();
}

int main(int argc, char** argv) {
    sylar::IOManager iom(2);
    iom.schedule(test);
    return 0;
}