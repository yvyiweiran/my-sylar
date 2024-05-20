#include "sylar/thread.h"
#include "sylar/config.h"

#include <unistd.h>

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

int count = 0;
sylar::Mutex s_mutex;

void fun1() {
    SYLAR_LOG_INFO(g_logger) << "name: " << sylar::Thread::GetName()
            << " thread.name: " << sylar::Thread::GetThis()->getName()
            << " id: " << sylar::GetThreadId()
            << " this.id: " << sylar::Thread::GetThis()->getId();
    for(int i = 0; i < 100000; ++i) {
        sylar::Mutex::Lock aaa(s_mutex);
        ++count;
    }
}


void fun2() {
    while(1){
        SYLAR_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
        // sleep(200);
    }
}

void fun3(){
    while(1){
        SYLAR_LOG_INFO(g_logger) << "===========================================";
        // sleep(200);
    }
}


int main(int argc, char** argv) {
    SYLAR_LOG_INFO(g_logger) << "main begin";
    YAML::Node file = YAML::LoadFile("/root/sylar/bin/conf/mutex.yaml");
    sylar::Config::LoadFromYaml(file);
    
    std::vector<sylar::Thread::ptr> thrs;
    for(int i = 0; i < 2; ++i) {
        sylar::Thread::ptr thr1(new sylar::Thread(&fun2, "name_" + std::to_string(i * 2)));
        sylar::Thread::ptr thr2(new sylar::Thread(&fun3, "name_" + std::to_string(i * 2 + 1)));
        thrs.push_back(thr1);
        thrs.push_back(thr2);
    }
    for(size_t i = 0; i < thrs.size(); ++i) {
        thrs[i]->join();
    }
    SYLAR_LOG_INFO(g_logger) << "main end";
    // std::cout << "\n========" << count << "===============\n";

    return 0;
}

