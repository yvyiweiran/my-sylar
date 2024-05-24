#ifndef __SYLAR_TCP_SERVER_H__
#define __SYLAR_TCP_SERVER_H__

#include <functional>
#include "IOManager.h"
#include <memory>
#include "noncopyable.h"
#include "Socket.h"
#include <vector>

namespace sylar {

class TcpServer : public std::enable_shared_from_this<TcpServer>
                , public Noncopyable {
public:
    using ptr = std::shared_ptr<TcpServer>;
    TcpServer(IOManager* woker = IOManager::GetThis()
            , IOManager* accept_woker = IOManager::GetThis());
    virtual ~TcpServer();

    virtual bool bind(Address::ptr addr);
    virtual bool bind(const std::vector<Address::ptr>& addrs, std::vector<Address::ptr>& fails);
    virtual bool start();
    virtual void stop();

    uint64_t getRecvTimeout() const { return m_recvTimeout;}
    std::string getName() const { return m_name;}
    void setRecvTimeout(const uint64_t v) { m_recvTimeout = v;}
    void setName(const std::string& v) { m_name = v;}

    bool isStop() const { return m_isStop;}
protected:
    virtual void handleClient(Socket::ptr client);
    virtual void startAccept(Socket::ptr sock);
private:
    std::vector<Socket::ptr> m_socks;   // socket句柄
    IOManager* m_worker;                // 工作线程
    IOManager* m_acceptWorker;                // 工作线程
    uint64_t m_recvTimeout;             // 读超时
    std::string m_name;
    bool m_isStop;
};


}

#endif