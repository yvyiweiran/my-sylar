#include "sylar/log.h"
#include "sylar/scheduler.h"
#include <unistd.h>

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_1() {
    sylar::Scheduler sc;
    sc.start();
    sleep(1);
    sc.stop();
}

void test() {
    static int count = 5;
    SYLAR_LOG_INFO(g_logger) << "=====test======";
    sleep(1);
    if(count >= 0){
        sylar::Scheduler::GetThis()->schedule(&test, sylar::GetThreadId());
        --count;
    }
    
}

void test_2() {
    sylar::Scheduler sc(3, "test");
    sc.start();
    SYLAR_LOG_INFO(g_logger) << "test_2 end";
    sc.schedule(&test);
    sc.stop();
}

int main(int argc, char** argv) {
    SYLAR_LOG_INFO(g_logger) << "test main begin";
    
    // test_1();

    test_2();

    SYLAR_LOG_INFO(g_logger) << "test main end";
    return 0;
}