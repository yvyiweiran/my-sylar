#ifndef __SYLAR_SCHEDULER_H__
#define __SYLAR_SCHEDULER_H__

#include "fiber.h"
#include <list>
#include <memory>
#include "thread.h"
#include <vector>

namespace sylar {

class Scheduler {
public:
    using ptr = std::shared_ptr<Scheduler>;
    using MutexType = Mutex;

    Scheduler(size_t threads = 1, const std::string& name = "", bool use_caller = true);
    virtual ~Scheduler();

    const std::string& getName() const { return m_name;}

    static Scheduler* GetThis();
    static Fiber* GetMainFiber();

    void start();
    void stop();

    template<class FiberOrCb>
    void schedule(FiberOrCb fc, int thread = -1) {
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }
        if(need_tickle) {
            tickle();
        }
    }

    template<class InputIterator>
    void schedule(InputIterator begin, InputIterator end) {
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            while (begin != end) {
                need_tickle = scheduleNoLock(&*begin, -1) || need_tickle;
                ++begin;
            }
        }
        if(need_tickle) {
            tickle();
        }
    }

protected:
    virtual void tickle();
    virtual bool stopping();
    virtual void idle();

    void run();
    void setThis();

    bool hasIdleThreads() { return m_idleThreadCount > 0;}

private:
    template<class FiberOtCb>
    bool scheduleNoLock(FiberOtCb fc, int thread = -1) {
        bool need_tickle = m_fiber.empty();
        FiberAndThread ft(fc, thread);
        if(ft.fiber || ft.cb) {
            m_fiber.push_back(ft);
            need_tickle &= true;
        }
        return need_tickle;
    }
private:
    struct FiberAndThread {
        Fiber::ptr fiber;
        std::function<void()> cb;
        int thread;

        FiberAndThread(Fiber::ptr f, int thr = -1)
            :fiber(f), thread(thr){}
        
        FiberAndThread(Fiber::ptr* f, int thr = -1)
            :thread(thr){
                fiber.swap(*f);
            }
        
        FiberAndThread(std::function<void()> f, int thr = -1)
            :cb(f), thread(thr){}
        
        FiberAndThread(std::function<void()>* f, int thr = -1)
            :thread(thr){
                cb.swap(*f);
            }
        
        FiberAndThread()
            :thread(-1){}
        
        void reset() {
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }
    };
    

private:
    MutexType m_mutex;
    std::string m_name;
    std::vector<Thread::ptr> m_thread;
    std::list<FiberAndThread> m_fiber;
    Fiber::ptr m_rootFiber;

protected:
    std::vector<int> m_thread_ids;
    size_t m_threadCount = 0;
    std::atomic<size_t> m_activeThreadCount{0};
    std::atomic<size_t> m_idleThreadCount{0};
    bool m_stopping = true;
    bool m_autoStop = false;
    int m_rootThread = 0;
};


}

#endif