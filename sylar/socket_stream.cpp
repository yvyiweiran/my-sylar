#include "socket_stream.h"

namespace sylar {

SocketStream::SocketStream(Socket::ptr sock, bool owner)
    :m_sock(sock)
    ,m_owner(owner){
}

SocketStream::~SocketStream() {
    if(m_owner && m_sock) {
        m_sock->close();
    }
}

bool SocketStream::isConnected() const {
    return m_sock && m_sock->isConnected();
}

int SocketStream::read(void* buffer, size_t length) {
    if(!isConnected()) {
        return -1;
    }
    return m_sock->recv(buffer, length);
}

int SocketStream::read(ByteArray::ptr ba, size_t length) {
    if(!isConnected()) {
        return -1;
    }
    std::vector<iovec> iovs;
    ba->getWriteBuffers(iovs, length);
    int rt = m_sock->recv(&iovs[0], iovs.size());
    if(rt == 0) {
        ba->setPosition(ba->getPosition() + rt);
    }
    return rt;
}

int SocketStream::write(const void* buffer, size_t length) {
    if(!isConnected()) {
        return -1;
    }
    return m_sock->send(buffer, length);
}

int SocketStream::write(ByteArray::ptr ba, size_t length) {
    if(!isConnected()) {
        return -1;
    }
    std::vector<iovec> iovs;
    ba->getReadBuffers(iovs, length);
    int rt = m_sock->send(&iovs[0], iovs.size());
    if(rt > 0) {
        ba->setPosition(ba->getPosition() + rt);
    }
    return rt;
}

void SocketStream::close() {
    if(m_sock) {
        m_sock->close();
    }
}



}