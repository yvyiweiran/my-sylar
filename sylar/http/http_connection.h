#ifndef __SYLAR_HTTP_SESSION_H__
#define __SYLAR_HTTP_SESSION_H__

#include "sylar/socket_stream.h"
#include "http.h"
#include "sylar/uri.h"
#include "sylar/thread.h"
#include <vector>
#include <list>

namespace sylar {
namespace http{

struct HttpResult {
    using ptr = std::shared_ptr<HttpResult>;
    enum class Error{
        /// 正常
        OK = 0,
        // 非法URL
        INVALID_URL = 1,
        // 无效HOST
        INVALID_HOST = 2,
        // 连接失败
        CONNECT_FAIL = 3,
        // 连接被对端关闭
        SEND_CLOSE_PEER = 4,
        // 发送请求时socket错误
        SEND_SOCKET_ERROR = 5,
        // 超时
        TIMEOUT = 6,
        // 场景socket失败
        SOCKTE_CREATE_ERROR = 7,
        // 从连接池取连接失败
        POOL_GET_CONNECTION = 8,
        // 无效的连接
        POOL_INVALID_CONNECTION,
    };
    HttpResult(int _result
            , HttpResponse::ptr _response
            , const std::string& _error)
        :result(_result)
        ,response(_response)
        ,error(_error){}
    int result;
    HttpResponse::ptr response;
    std::string error;

    std::string toString() const;
};

class HttpConnectionPool;

/**
 * @brief HTTP客户端类
*/
class HttpConnection : public SocketStream {
friend HttpConnectionPool;
public:
    using ptr = std::shared_ptr<HttpConnection>;

    /**
     * @brief 发送HTTP的GET请求
     * @param[in] uri 请求的uri
     * @param[in] timeout_ms 超时时间
     * @param[in] headers 请求头
     * @param[in] body 请求消息体
     * @return 返回HTTP结构结构体
    */
    static HttpResult::ptr DoGet(const std::string& uri
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

    static HttpResult::ptr DoGet(Uri::ptr uri
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

    static HttpResult::ptr DoPost(const std::string& uri
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

    static HttpResult::ptr DoPost(Uri::ptr uri
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

    static HttpResult::ptr DoRequest(HttpMethod method
                    , const std::string& uri
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

    static HttpResult::ptr DoRequest(HttpMethod method
                    , Uri::ptr uri
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

    static HttpResult::ptr DoRequest(HttpRequest::ptr req
                    , Uri::ptr uri
                    , uint64_t timeout_ms);

    /**
     * @brief 构造函数
     * @param[in] sock Socket类
     * @param[in] owner 是否掌握所有权
    */
    HttpConnection(Socket::ptr sock, bool owner = true);

    /**
     * @brief 接收Http相应
    */
    HttpResponse::ptr recvResponse();

    /**
     * @brief 发送HTTP请求
     * @param[in] req HTTP请求结构
    */
    int sendRequest(HttpRequest::ptr req);
private:
    uint64_t m_create_time = 0;
};

class HttpConnectionPool{
public:
    using ptr = std::shared_ptr<HttpConnectionPool>;
    using MutexType = Mutex;

    HttpConnectionPool(const std::string& host
                     , const std::string& vhost
                     , uint32_t port
                     , uint32_t max_size
                     , uint32_t max_alive_time
                     , uint32_t max_request);
    HttpConnection::ptr getConnection();

    HttpResult::ptr doGet(const std::string& uri
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

    HttpResult::ptr doGet(Uri::ptr uri
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

    HttpResult::ptr doPost(const std::string& uri
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

    HttpResult::ptr doPost(Uri::ptr uri
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

    HttpResult::ptr doRequest(HttpMethod method
                    , const std::string& uri
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

    HttpResult::ptr doRequest(HttpMethod method
                    , Uri::ptr uri
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

    HttpResult::ptr doRequest(HttpRequest::ptr req
                    , uint64_t timeout_ms);
    
private:
    static void ReleasePtr(HttpConnection* ptr, HttpConnectionPool* pool);
private:
    std::string m_host;
    std::string m_vhost;
    uint32_t m_port;
    // 最大连接数
    uint32_t m_maxSize;
    // 连接最大存活时间
    uint32_t m_maxAliveTime;
    // 一次链接最多处理的请求数
    uint32_t m_maxRequest;

    MutexType m_mutex;
    std::list<HttpConnection*> m_conns;
    std::atomic<int32_t> m_total = {0};
};

}
}

#endif