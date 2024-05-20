#include "sylar/Socket.h"
#include "sylar/log.h"
#include "sylar/IOManager.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test() {
    sylar::IPAddress::ptr addr;
    addr = sylar::Address::LookupAnyIpAddress("www.baidu.com");
    if(addr) {
        SYLAR_LOG_INFO(g_logger) << "get address:" << addr->toString();
    } else {
        SYLAR_LOG_ERROR(g_logger) << "get address fail";
        return;
    }

    sylar::Socket::ptr sock = sylar::Socket::CreateTCP(addr);
    addr->setPort(80);
    if(!sock->connect(addr)) {
        SYLAR_LOG_ERROR(g_logger) << "connect " << addr->toString() << " fail";
        return;
    }else {
        SYLAR_LOG_INFO(g_logger) << "connect" << addr->toString() << " connect";
    }

    const char buff[] = "GET / HTTP/1.0\r\n\r\n";
    int rt = sock->send(buff, sizeof(buff));
    if(rt <= 0) {
        SYLAR_LOG_INFO(g_logger) << "send fail rt=" << rt;
        return;
    }

    std::string buffers;
    buffers.resize(4096);
    rt = sock->recv(&buffers[0], buffers.size());

    if(rt <= 0) {
        SYLAR_LOG_INFO(g_logger) << "recv fail rt=" << rt;
        return;
    }

    buffers.resize(rt);
    SYLAR_LOG_INFO(g_logger) << buffers;
}

int main(int argc, char** argv) {

    sylar::IOManager iom;
    iom.schedule(test);

    return 0;
}
