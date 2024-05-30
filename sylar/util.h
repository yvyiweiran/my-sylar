/**
 * @file util.h
 * @brief 常用的工具函数
 * @author yvyiweiran
 * @email 3423028697@qq.com
 * @date 2024-5-30
 */
#ifndef __SYLAY_UTIL_H__
#define __SYLAY_UTIL_H__

#include <pthread.h>
#include <stdint.h>
#include <string>
#include <vector>

namespace sylar {

/**
 * @brief 返回当前线程的ID
 */
pid_t GetThreadId();

/**
 * @brief 返回当前协程的ID
 */
uint32_t GetFiderId();

/**
 * @brief 获取当前的调用栈
 * @param[out] bt 保存调用栈
 * @param[in] size 最多返回层数
 * @param[in] skip 跳过栈顶的层数
 */
void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);

/**
 * @brief 获取当前栈信息的字符串
 * @param[in] size 栈的最大层数
 * @param[in] skip 跳过栈顶的层数
 * @param[in] prefix 栈信息前输出的内容
 */
std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "\t");

// 时间
/**
 * @brief 获取当前时间的毫秒
 */
uint64_t GetCurrentMS();
/**
 * @brief 获取当前时间的微秒
 */
uint64_t GetCurrentUS();

}

#endif
