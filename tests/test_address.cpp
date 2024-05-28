#include "sylar/address.h"
#include "sylar/log.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test() {
    SYLAR_LOG_INFO(g_logger) << "test begin";
    std::vector<sylar::Address::ptr> addrs;
    bool v = sylar::Address::Lookup(addrs, "www.sylar.top");
    SYLAR_LOG_INFO(g_logger) << "test end";
    if(!v) {
        SYLAR_LOG_ERROR(g_logger) << "lookup fail";
        return;
    }

    for(size_t i = 0; i < addrs.size(); ++i) {
        SYLAR_LOG_INFO(g_logger) << i << " - " << addrs[i]->toString();
    }
}

void test_iface() {
    std::multimap<std::string, std::pair<sylar::Address::ptr, uint32_t> > results;

    bool v = sylar::Address::GetInterFaceAddresses(results);
    if(!v) {
        SYLAR_LOG_INFO(g_logger) << "GetInerFaceAddress fail";
    }

    for(auto& i : results) {
        SYLAR_LOG_INFO(g_logger) << i.first << " - "
            << i.second.first->toString() << " - "
            << i.second.second;
    }
}

void test_ipv4() {
    // auto addr = sylar::IPAddress::Creat("www.sylar.top");
    auto addr = sylar::IPAddress::Creat("127.0.0.8");
    if(addr) {
        SYLAR_LOG_INFO(g_logger) << addr->toString();
    }
}

void test_1() {
    auto addr = sylar::Address::LookupAnyIpAddress("0.0.0.0");
    SYLAR_LOG_INFO(g_logger) << addr->toString();
}

int main(int argc, char** argv) {
    // test();
    // SYLAR_LOG_DEBUG(g_logger) << "===================";
    // test_iface();
    // SYLAR_LOG_DEBUG(g_logger) << "===================";
    // test_ipv4();
    test_1();
    return 0;
}
