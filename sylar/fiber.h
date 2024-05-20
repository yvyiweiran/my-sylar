#ifndef __SYLAR_FIBER_H
#define __SYLAR_FIBER_H

#include <functional>
#include <memory>
#include <ucontext.h>

namespace sylar {

class Scheduler;

class Fiber : public std::enable_shared_from_this<Fiber> {
    friend sylar::Scheduler;
public:
    using ptr = std::shared_ptr<Fiber>;

    enum State{
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT
    };
private:
    Fiber();
public:
    /**
     *  @brief 构造函数
     *  @param[in] cb 协程执行函数
     *  @param[in] stacksize 协程栈大小
     */
    Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);
    ~Fiber();

    //重置协程函数, 并重置状态
    void reset(std::function<void()> cb);
    // 执行当前协程
    void swapIn();
    // 当前协程切换到后台
    void swapOut();

    void call();

    void back();

    uint64_t getId() const { return m_id;}
    State getState() const { return m_state;}
    void setState(const State val) { m_state = val;}
public:
    static void SetThis(Fiber* val);
    // 获取当前协程
    static Fiber::ptr GetThis();
    //协程切换到后台, 设置为Ready状态
    static void YieldToReady();
    // 协程切换到后台, 设置为Hold状态
    static void YieldToHold();
    // 总协程数
    static uint64_t TodalFiber();

    static void MainFunc();
    static void CallerMainFunc();
    static uint64_t GetFiderId();

private:
    uint64_t m_id = 0;
    uint32_t m_stacksize = 0;
    State m_state;

    ucontext_t m_ctx;
    void* m_stack = nullptr;

    std::function<void()> m_cb;
};


}

#endif