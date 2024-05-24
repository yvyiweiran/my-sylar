#include "sylar/bytearray.h"
#include "sylar/tcp_server.h"
#include "sylar/log.h"
#include "sylar/IOManager.h"
#include <iostream>

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

class EchoServer : public sylar::TcpServer {
public:
    EchoServer(int type);
    void handleClient(sylar::Socket::ptr client) override;
private:
    int m_type = 0;
};

EchoServer::EchoServer(int type)
    :m_type(type){
}

void EchoServer::handleClient(sylar::Socket::ptr client) {
    SYLAR_LOG_INFO(g_logger) << "handleClient: " << *client;
    sylar::ByteArray::ptr ba(new sylar::ByteArray);
    while(true) {
        ba->clear();
        std::vector<iovec> iovs;
        ba->getWriteBuffers(iovs, 1024);

        int rt = client->recv(&iovs[0], iovs.size());
        if(rt == 0) {
            SYLAR_LOG_INFO(g_logger) << "client close: " << *client;
            break;
        } else if(rt < 0) {
            SYLAR_LOG_INFO(g_logger) << "client error rt=" << rt
                << " errno=" << errno << " errstr=" << strerror(errno);
            break;
        }
        ba->setPosition(ba->getPosition() + rt);
        ba->setPosition(0);
        if(m_type == 1) {// text
            std::cout << ba->toString() << std::endl;
        } else {
            std::cout << ba->toHexString() <<std::endl;
        }
    }
}

int type = 1;

void test() {
    EchoServer::ptr es(new EchoServer(type));
    auto addr = sylar::Address::LookupAny("0.0.0.0:80");
    while(!es->bind(addr)) {
        sleep(3);
    }
    es->start();
}

int main(int argc, char** argv) {
    if(argc < 2) {
        SYLAR_LOG_INFO(g_logger) << " used as{" << argv[0]
                << "-t} or {" << argv[0] << " -b}";
        return -1;
    }

    if(!strcmp(argv[1], "-b")) {
        type = 2;
    }
    sylar::IOManager iom(2);
    iom.schedule(test);
    return 0;
}
