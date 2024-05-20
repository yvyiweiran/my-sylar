#ifndef __SYLAR_THREAD_H__
#define __SYLAR_THREAD_H__

#include <atomic>
#include <functional>
#include <memory>
#include <pthread.h>
#include <semaphore.h>
#include <thread>
#include <stdint.h>
#include "noncopyable.h"

namespace sylar{

class Semaphore :public Noncopyable{
public:
    typedef std::shared_ptr<Semaphore> ptr;
    Semaphore(uint32_t count = 0);
    ~Semaphore();

    void wait();
    void notify();
private:
    sem_t m_semaphore;
};

template<class T>
class ScopedLockImpl {
public:
    ScopedLockImpl(T& mutex)
        :m_mutex(mutex){
        m_mutex.lock();
        m_locked = true;
    }
    ~ScopedLockImpl() {
        if(m_locked)
            unlock();
    }

    void lock() {
        if(!m_locked) {
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock() {
        if(m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};

template<class T>
class ReadScopedLockImpl {
public:
    ReadScopedLockImpl(T& mutex)
        :m_mutex(mutex){
        m_mutex.rdlock();
        m_locked = true;
    }
    ~ReadScopedLockImpl() {
        if(m_locked)
            unlock();
    }

    void lock() {
        if(!m_locked) {
            m_mutex.rdlock();
            m_locked = true;
        }
    }

    void unlock() {
        if(m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};

template<class T>
class WriteScopedLockImpl {
public:
    using ptr = std::shared_ptr<WriteScopedLockImpl>;
    WriteScopedLockImpl(T& mutex)
        :m_mutex(mutex){
        m_mutex.wrlock();
        m_locked = true;
    }
    ~WriteScopedLockImpl() {
        if(m_locked)
            unlock();
    }

    void lock() {
        if(!m_locked) {
            m_mutex.wrlock();
            m_locked = true;
        }
    }

    void unlock() {
        if(m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};

class Mutex :public Noncopyable{
public:
    using Lock = ScopedLockImpl<Mutex>;
    Mutex() {
        pthread_mutex_init(&m_mutex, nullptr);
    }
    ~Mutex() {
        pthread_mutex_destroy(&m_mutex);
    }
    
    void lock() {
        pthread_mutex_lock(&m_mutex);
    }

    void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }
private:
    pthread_mutex_t m_mutex;
};

class RWMutex :public Noncopyable{
public:
    using ReadLock = ReadScopedLockImpl<RWMutex>;
    using WriteLock = WriteScopedLockImpl<RWMutex>;
    RWMutex() {
        pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWMutex() {
        pthread_rwlock_destroy(&m_lock);
    }

    void rdlock(){
        pthread_rwlock_rdlock(&m_lock);
    }

    void wrlock() {
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock() {
        pthread_rwlock_unlock(&m_lock);
    }
private:
    pthread_rwlock_t m_lock;
};

class NullMutex :public Noncopyable{
public:
    using Lock = ScopedLockImpl<NullMutex>;
    NullMutex() {}
    ~NullMutex() {}
    void lock() {}
    void unlock() {}
};

class RWNullMutex :public Noncopyable{
public:
    using ReadLock = ReadScopedLockImpl<RWNullMutex>;
    using WriteLock = WriteScopedLockImpl<RWNullMutex>;
    RWNullMutex() {}
    ~RWNullMutex() {}
    void rdlock() {}
    void wrlock() {}
    void unlock() {}
};

class Spinlock :public Noncopyable{
public:
    using Lock = ScopedLockImpl<Spinlock>;
    Spinlock() {
        pthread_spin_init(&m_mutex, 0);
    }

    ~Spinlock() {
        pthread_spin_destroy(&m_mutex);
    }

    void lock() {
        pthread_spin_lock(&m_mutex);
    }

    void unlock() {
        pthread_spin_unlock(&m_mutex);
    }
private:
    pthread_spinlock_t m_mutex;
};

class CASLock :public Noncopyable{
public:
using Lock = ScopedLockImpl<CASLock>;
    CASLock() {
        m_mutex.clear();
    }

    ~CASLock() {

    }

    void lock() {
        // 获取到锁之前自旋
        while(std::atomic_flag_test_and_set_explicit(&m_mutex, std::memory_order_acquire));
    }

    void unlock() {
        std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_release);
    }
private:
    volatile std::atomic_flag m_mutex;
};

class Thread {
public:
    typedef std::shared_ptr<Thread> ptr;
    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();

    pid_t getId() const { return m_id;}
    const std::string& getName() const {return m_name;}

    void join();

    static Thread* GetThis();
    static const std::string& GetName();
    static void SetName(const std::string& val);
private:
    Thread(const Thread&) = delete;
    Thread(const Thread&&) = delete;
    Thread& operator=(const Thread&) = delete;
    
    static void* run(void* arg);
private:
    pid_t m_id = -1;
    pthread_t m_thread = 1;
    std::function<void()> m_cb;
    std::string m_name;
    Semaphore m_semaphore;
};


}



#endif  //__SYLAR_THREAD_H__