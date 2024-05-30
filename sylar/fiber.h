/**
 * @file fiber.h
 * @brief 协程封装
 * @author yvyiweiran
 * @email 3423028697@qq.com
 * @date 2024-5-30
 */
#ifndef __SYLAR_FIBER_H
#define __SYLAR_FIBER_H

#include <functional>
#include <memory>
#include <ucontext.h>

namespace sylar {

class Scheduler;

/**
 * @brief 协程类
 */
class Fiber : public std::enable_shared_from_this<Fiber> {
    friend sylar::Scheduler;
public:
    using ptr = std::shared_ptr<Fiber>;

    /**
     * @brief 协程状态
     */
    enum State{
        /// 初始化状态
        INIT,
        /// 暂停状态
        HOLD,
        /// 执行中状态
        EXEC,
        /// 结束状态
        TERM,
        /// 可执行状态
        READY,
        /// 异常状态
        EXCEPT
    };
private:
    Fiber();
public:
    /**
     *  @brief 构造函数
     *  @param[in] cb 协程执行函数
     *  @param[in] stacksize 协程栈大小
     * @param[in] use_caller 是否在MainFiber上调度
     */
    Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);
    ~Fiber();

    /**
     * @brief 重置协程执行函数,并设置状态
     * @pre getState() 为 INIT, TERM, EXCEPT
     * @post getState() = INIT
     */
    void reset(std::function<void()> cb);
    
    /**
     * @brief 将当前协程切换到运行状态
     * @pre getState() != EXEC
     * @post getState() = EXEC
     */
    void swapIn();
    
    /**
     * @brief 将当前协程切换到后台
     */
    void swapOut();

    /**
     * @brief 将当前线程切换到执行状态
     * @pre 执行的为当前线程的主协程
     */
    void call();

    /**
     * @brief 将当前线程切换到后台
     * @pre 执行的为该协程
     * @post 返回到线程的主协程
     */
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
    /// 协程id
    uint64_t m_id = 0;
    /// 协程运行栈大小
    uint32_t m_stacksize = 0;
    /// 协程状态
    State m_state = INIT;
    /// 协程上下文
    ucontext_t m_ctx;
    /// 协程运行栈指针
    void* m_stack = nullptr;
    /// 协程运行函数
    std::function<void()> m_cb;
};


}

#endif