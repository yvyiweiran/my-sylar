#ifndef __SYLSR_SINGLETON_H__
#define __SYLSR_SINGLETON_H__

#include <memory>
namespace sylar{

template<class T, class X = void, int N = 0>
class Singleton {
public:
    static T* GetIstance() {
        static T v;
        return &v;
    }
private:
};

template<class T, class X = void, int N = 0>
class SingletonPtr{
public:
    static std::shared_ptr<T> GetInstance() {
        static std::shared_ptr<T> v(new T);
        return &v;
    }
};


}
#endif