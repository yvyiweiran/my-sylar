#include "sylar/fiber.h"
#include "sylar/log.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void run_in_fiber() {
    SYLAR_LOG_INFO(g_logger) << "run in fiber begin  " << sylar::Fiber::GetThis()->getId();
    sylar::Fiber::YieldToHold();
    SYLAR_LOG_INFO(g_logger) << "run in fiber end  " << sylar::Fiber::GetThis()->getId();
    sylar::Fiber::YieldToHold();
}

void test_fiber () {
    sylar::Fiber::GetThis();
    SYLAR_LOG_INFO(g_logger) << "main begin  " << sylar::Fiber::GetThis()->getId();
    sylar::Fiber::ptr fiber(new sylar::Fiber(run_in_fiber));
    fiber->swapIn();
    SYLAR_LOG_INFO(g_logger) << "main swapIn 1  " << sylar::Fiber::GetThis()->getId();
    fiber->swapIn();
    SYLAR_LOG_INFO(g_logger) << "main swapIn 2  " << sylar::Fiber::GetThis()->getId();
    fiber->swapIn();
}

int main(int ragc, char** argv){
    sylar::Thread::SetName("main");
    SYLAR_LOG_INFO(g_logger) << "main begin -1  " << sylar::Fiber::GetThis()->getId();
    std::vector<sylar::Thread::ptr> thrs;
    for(int i = 0; i < 3; ++i) {
        thrs.push_back(sylar::Thread::ptr(
                new sylar::Thread(&test_fiber, "name_"+std::to_string(i))));
    }
    for(auto& i: thrs) {
        i->join();
    }
    SYLAR_LOG_INFO(g_logger) << "main end  " << sylar::Fiber::GetThis()->getId();
    return 0;
}

