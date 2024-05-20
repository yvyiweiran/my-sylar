#ifndef __SYLAY_UTIL_H__
#define __SYLAY_UTIL_H__

#include <pthread.h>
#include <stdint.h>
#include <string>
#include <vector>

namespace sylar {

pid_t GetThreadId();

uint32_t GetFiderId();

void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);

std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "\t");

// 时间
uint64_t GetCurrentMS();
uint64_t GetCurrentUS();

}

#endif
