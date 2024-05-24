#include "sylar/tcp_server.h"
#include "sylar/IOManager.h"
#include "sylar/log.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test() {
    auto addr = sylar::Address::LookupAny("0.0.0.0:80");

    std::vector<sylar::Address::ptr> addrs, fails;
    addrs.push_back(addr);
    sylar::TcpServer::ptr tcp_server(new sylar::TcpServer);
    if(!tcp_server->bind(addrs, fails)) {
        sleep(2);
    }
    tcp_server->start();
    
}

int main(int argc, char** argv) {
    sylar::IOManager iom(2);
    iom.schedule(test);
    return 0;
}