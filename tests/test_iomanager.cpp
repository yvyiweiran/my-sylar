#include "sylar/IOManager.h"
#include "sylar/log.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_fiber() {
    SYLAR_LOG_INFO(g_logger) << "test_fiber";
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = inet_addr("110.242.68.66");

    if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))) {
    }else if(errno == EINPROGRESS){
        SYLAR_LOG_INFO(g_logger) << "else" << errno << " " << strerror(errno);
        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::READ, []() {
            SYLAR_LOG_INFO(g_logger) << "read callback";
        });
        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::WRITE, [sock]() {
            SYLAR_LOG_INFO(g_logger) << "write callback";
            // close(sock);
            sylar::IOManager::GetThis()->cancelEvent(sock, sylar::IOManager::READ);
            close(sock);
        });
    }else {
        SYLAR_LOG_INFO(g_logger) << "else" << errno << " " << strerror(errno);
    }
}

void test1() {
    sylar::IOManager iom(2, false);
    // sylar::IOManager iom(2);
    iom.schedule(&test_fiber);

}

int main(int argc, char** argv) {
    test1();
    return 0;
}
