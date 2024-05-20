#include "sylar/IOManager.h"
#include "sylar/log.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
sylar::Timer::ptr timer;
void test_timer() {
    sylar::IOManager iom(2);
    timer = iom.addTimer(1000, []() {
        SYLAR_LOG_INFO(g_logger) << "hello timer";
        static int xx = 0;
        if(xx++ >=3) {
            timer->reset(500, true);
            // timer->cancel();
        }
    }, true);
    // iom.addTimer(5000, []() {
    //     SYLAR_LOG_INFO(g_logger) << "hello timer";
    // });
}

int main(int argc, char** argv) {
    test_timer();
    return 0;
}
