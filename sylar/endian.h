#ifndef __SYLAR_ENDIAN_H__
#define __SYLAR_ENDIAN_H__

#define SYLAR_LITTLE_ENDIAN 1
#define SYLAR_BIG_ENDIAN 2

#include <byteswap.h>
#include <stdio.h>
#include <stdint.h>
#include <type_traits>

namespace sylar{

template<class T>
typename std::enable_if<sizeof(T) == sizeof(__uint64_t), T>::type
byteswap(T val) {
    return (T)bswap_64((uint64_t)val);
}

template<class T>
typename std::enable_if<sizeof(T) == sizeof(__uint32_t), T>::type
byteswap(T val) {
    return (T)bswap_32((uint32_t)val);
}

template<class T>
typename std::enable_if<sizeof(T) == sizeof(__uint16_t), T>::type
byteswap(T val) {
    return (T)bswap_16((uint16_t)val);
}

#if BYTE_ORDER == SYLAR_BIG_ENDIAN
#define SYLAR_BYTE_ORDER SYLAR_BIG_ENDIAN
#else
#define SYLAR_BYTE_ORDER SYLAR_LITTLE_ENDIAN
#endif

#if SYLAR_BYTE_ORDER == SYLAR_BIG_ENDIAN
template<class T>
T byteswapOnLittelEndian(T t) {
    return t;
}

template<class T>
T byteswapOnBigEndian(T t) {
    return byteswap(t);
}
#else
template<class T>
T byteswapOnLittelEndian(T t) {
    return byteswap(t);
}

template<class T>
T byteswapOnBigEndian(T t) {
    return t;
}
#endif
}

#endif