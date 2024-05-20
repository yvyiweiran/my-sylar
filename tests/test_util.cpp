#include <assert.h>
#include "sylar/log.h"
#include "sylar/macro.h"
#include "sylar/util.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

void test_assert() {
    SYLAR_LOG_INFO(g_logger) << sylar::BacktraceToString(10);
    // SYLAR_ASSERT(false);
    SYLAR_ASSERT2(0, "xxxxxxxxxxxx");
}

int main(int argc, char** argv) {
    test_assert();
    return 0;
}