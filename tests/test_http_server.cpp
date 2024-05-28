#include "sylar/http/http_server.h"
#include "sylar/log.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void  test() {
    sylar::http::HttpServer::ptr server(new sylar::http::HttpServer);
    sylar::Address::ptr addr = sylar::Address::LookupAnyIpAddress("0.0.0.0:8020");
    while (!server->bind(addr)) {
        sleep(2);
    }
    server->start();
    // server->stop();
}

int main(int argc, char** argv) {
    sylar::IOManager iom(2);
    iom.schedule(test);
    return 0;
}