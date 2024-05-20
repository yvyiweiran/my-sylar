#ifndef __IOMANAGER_H__
#define __IOMANAGER_H__

#include "scheduler.h"
#include "thread.h"
#include "timer.h"

namespace sylar{


class IOManager : public Scheduler, public TimerManager{
public:
    using ptr = std::shared_ptr<IOManager>;
    using RWMutexType = RWNullMutex;

    enum Event{
        NONE    = 0x0,
        READ    = 0x1,
        WRITE   = 0x4
    };
private:
    struct FdContext{
        typedef Mutex MutexType;
        struct EventContext{
            Scheduler* scheduler = nullptr; // 事件执行的schedule
            Fiber::ptr fiber;               // 事件协程
            std::function<void()> cb;       // 回调函数
        };

        EventContext& getContext(Event event);
        void resetContext(EventContext& ctx);
        void triggetEvent(Event event);

        EventContext read;      // 读事件
        EventContext write;     // 写事件
        int fd = 0;                 // 事件关联的句柄
        Event events = NONE;  // 已注册事件
        MutexType mutex;
    };
    
public:
    IOManager(int thread = 1, bool use_caller = true, const std::string& name = "");
    ~IOManager();

    // 0: 成功, -1: 错误
    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
    bool delEvent(int fd, Event event);
    bool cancelEvent(int fd, Event event);
    
    bool cancelAll(int fd);

    static IOManager* GetThis();

protected:
    void tickle() override;
    bool stopping() override;
    void idle() override;
    void onTimerInsertAtFront() override;

    void contextResize(int size);
    bool stopping(uint64_t& timeout);
private:
    int m_epfd = 0;
    int m_tickleFds[2];

    std::atomic<size_t> m_pendingEventCount = {0};
    RWMutexType m_mutex;
    std::vector<FdContext*> m_fdContexts;

};

}
#endif