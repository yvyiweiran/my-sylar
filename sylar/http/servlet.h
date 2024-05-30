/**
 * @file servlet.h
 * @brief Servlet封装
 * @author yvyiweiran
 * @email 3423028697@qq.com
 * @date 2024-5-30
 */
#ifndef __SYLAR_SERVLET_H__
#define __SYLAR_SERVLET_H__

#include <functional>
#include "http.h"
#include "http_parser.h"
#include "http_session.h"
#include <memory>
#include <string>
#include "sylar/thread.h"
#include <unordered_map>
#include <vector>

namespace sylar{
namespace http{

/**
 * @brief Servlet封装
 */
class Servlet {
public:
    /// 智能指针类型定义
    using ptr = std::shared_ptr<Servlet>;

    /**
     * @brief 构造函数
     * @param[in] name 名称
     */
    Servlet(const std::string& name);

    /**
     * @brief 析构函数
     */
    virtual ~Servlet() {}

    /**
     * @brief 处理请求
     * @param[in] request HTTP请求
     * @param[in] response HTTP响应
     * @param[in] session HTTP连接
     * @return 是否处理成功
     */
    virtual int32_t handle(sylar::http::HttpRequest::ptr request
                          , sylar::http::HttpResponse::ptr response
                          , sylar::http::HttpSession::ptr session) = 0;

    /**
     * @brief 返回Servlet名称
     */
    const std::string& getName() const { return m_name;}

protected:
    std::string m_name;
};

/**
 * @brief 函数式Servlet
 */
class FunctionServlet : public Servlet {
public:
    /// 智能指针类型定义
    using ptr = std::shared_ptr<FunctionServlet>;
    /// 函数回调类型定义
    using callback = std::function<int32_t (
        sylar::http::HttpRequest::ptr request
        , sylar::http::HttpResponse::ptr response
        , sylar::http::HttpSession::ptr session)>;

    
    /**
     * @brief 构造函数
     * @param[in] cb 回调函数
     */
    FunctionServlet(callback cb);
    int32_t handle(sylar::http::HttpRequest::ptr request
            , sylar::http::HttpResponse::ptr response
            , sylar::http::HttpSession::ptr session) override;
private:
    /// 回调函数
    callback m_cb;
};

/**
 * @brief Servlet分发器
 */
class ServletDispatch : public Servlet {
public:
    /// 智能指针类型定义
    using ptr = std::shared_ptr<ServletDispatch>;
    /// 读写锁类型定义
    using RWMutexType = RWMutex;

    /**
     * @brief 构造函数
     */
    ServletDispatch();
    int32_t handle(sylar::http::HttpRequest::ptr request
            , sylar::http::HttpResponse::ptr response
            , sylar::http::HttpSession::ptr session) override;

    /**
     * @brief 添加servlet
     * @param[in] uri uri
     * @param[in] slt serlvet
     */
    void addServlet(const std::string& uri, Servlet::ptr slt);

    /**
     * @brief 添加servlet
     * @param[in] uri uri
     * @param[in] cb FunctionServlet回调函数
     */
    void addServlet(const std::string& uri, FunctionServlet::callback cb);

    /**
     * @brief 添加模糊匹配servlet
     * @param[in] uri uri 模糊匹配 /sylar_*
     * @param[in] slt servlet
     */
    void addGlobServlet(const std::string& uri, Servlet::ptr slt);

    /**
     * @brief 添加模糊匹配servlet
     * @param[in] uri uri 模糊匹配 /sylar_*
     * @param[in] cb FunctionServlet回调函数
     */
    void addGlobServlet(const std::string& uri, FunctionServlet::callback cb);

    void delServlet(const std::string& uri);
    void delGlobServlet(const std::string& uri);

    Servlet::ptr getDefault() const { return m_default;}
    void setDefault(Servlet::ptr v) { m_default = v;}

    Servlet::ptr getServlet(const std::string& uri);
    Servlet::ptr getGlobServlet(const std::string& uri);

    Servlet::ptr getMathchedServlet(const std::string& uri);
private:
    RWMutexType m_mutex;
    // uri(/sylar/xxx) --> servlet
    std::unordered_map<std::string, Servlet::ptr> m_datas;
    // uri(/sylar/*) --> servlet
    std::vector<std::pair<std::string, Servlet::ptr>> m_globs;
    // 默认servlet, 所有路径都没匹配到时使用
    Servlet::ptr m_default;
};

/**
 * @brief NotFoundServlet(默认返回404)
 */
class NotFoundServlet : public Servlet {
public:
    NotFoundServlet();
    int32_t handle(sylar::http::HttpRequest::ptr request
        , sylar::http::HttpResponse::ptr response
        , sylar::http::HttpSession::ptr session) override;
};

}
}

#endif