/**
 * @file macro.h
 * @brief 常用宏的封装
 * @author yvyiweiran
 * @email 3423028697@qq.com
 * @date 2024-5-30
 */
#ifndef __SYLAR_MACRO_H__
#define __SYLAR_MACRO_H__

#include <assert.h>
#include <string.h>
#include "util.h"

#if defined __GNUC__ || defined __llvm__
#   define SYLAR_LICKLY(x)      __builtin_expect(!!(x), 1)
#   define SYLAR_UNLICKLY(x)    __builtin_expect(!!(x), 0)
#else
#   define SYLAR_LICKLY(x)      (x)
#   define SYLAR_UNLICKLY(x)    (x)
#endif
/// 断言宏封装
#define SYLAR_ASSERT(x) \
    if(SYLAR_UNLICKLY(!(x))) {\
        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ASSERTION: " #x \
            << "\nbacktrace:\n" \
            << sylar::BacktraceToString(100, 2, "\t"); \
        assert(x); \
    }

/// 断言宏封装
#define SYLAR_ASSERT2(x, w) \
    if(SYLAR_UNLICKLY(!(x))) {\
        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ASSERTION: " #x \
            << "\n" << w \
            << "\nbacktrace: "\
            << sylar::BacktraceToString(100, 2, "\t"); \
        assert(x); \
    }

#endif