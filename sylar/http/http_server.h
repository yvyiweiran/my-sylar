#ifndef __SYLAR_HTT_SERVER_H__
#define __SYLAR_HTT_SERVER_H__

#include "sylar/tcp_server.h"
#include "http_session.h"

namespace sylar {
namespace http {

class HttpServer : public TcpServer {
public:
    using ptr = std::shared_ptr<HttpServer>;
    HttpServer(bool keepalive = false
        , IOManager* worker = IOManager::GetThis()
        , IOManager* accept_worker = IOManager::GetThis());
protected:
    void handleClient(Socket::ptr client) override;
private:
    bool m_isKeepalive;

};


}
}

#endif