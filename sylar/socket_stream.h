#ifndef __SYLAR_SOCKET_STREAM_H__
#define __SYLAR_SOCKET_STREAM_H__

#include "stream.h"
#include "Socket.h"

namespace sylar{

class SocketStream : public Stream {
public:
    using ptr = std::shared_ptr<SocketStream>;
    SocketStream(Socket::ptr sock, bool owner = true);
    ~SocketStream();
    int read(void* buffer, size_t length) override;
    int read(ByteArray::ptr ba, size_t length) override;
    int write(const void* buffer, size_t length) override;
    int write(ByteArray::ptr ba, size_t length) override;
    void close() override;

    Socket::ptr getSocket() const { return m_sock;}
    bool isConnected() const;
private:
    Socket::ptr m_sock;
    bool m_owner;
};

}

#endif