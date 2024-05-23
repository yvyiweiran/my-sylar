#ifndef __SYLAR_SOCKET_H__
#define __SYLAR_SOCKET_H__

#include "address.h"
#include "noncopyable.h"
#include "hook.h"
#include <netinet/tcp.h>


namespace sylar{

class Socket : public std::enable_shared_from_this<Socket>, public Noncopyable{
public:
    using ptr = std::shared_ptr<Socket>;
    using weak_ptr = std::weak_ptr<Socket>;

    enum Type {
        TCP = SOCK_STREAM,
        UDP = SOCK_DGRAM
    };

    enum Family{
        IPv4 = AF_INET,
        IPv6 = AF_INET6,
        UNIX = AF_UNIX
    };

    static Socket::ptr CreateTCP(sylar::Address::ptr address);
    static Socket::ptr CreateUDP(sylar::Address::ptr address);

    static Socket::ptr CreateTCP();
    static Socket::ptr CreateUDP();

    static Socket::ptr CreateTCP6();
    static Socket::ptr CreateUDP6();

    static Socket::ptr CreateUnixTCPSocket();
    static Socket::ptr CreateUnixUDPSocket();

    Socket(int family, int type, int proto = 0);
    ~Socket();

    int64_t getSendTimeout();
    void setSendTimeout(int64_t v);

    int64_t getRecvTimeout();
    void setRecvTimeout(int64_t v);

    bool getOption(int level, int option, void* result, socklen_t* len);
    template<class T>
    bool getOption(int level, int option, T& result) {
        socklen_t length = sizeof(T);
        return getOption(level, option, result, &length);
    }

    bool setOption(int level, int option, const void* result, socklen_t len);
    template<class T>
    bool setOption(int level, int option, const T& value) {
        return setOption(level, option, &value, sizeof(T));
    }

    Socket::ptr accept();

    bool init(int sock);

    bool bind(const Address::ptr addr);
    bool connect(const Address::ptr addr, uint64_t timeout_ms = -1);
    bool lisent(int backlog = SOMAXCONN);
    bool close();

    int send(const void* buffer, size_t length, int flag = 0);
    int send(const iovec* buffers, size_t length, int flag = 0);
    int sendTo(const void* buffer, size_t length, const Address::ptr to, int flag = 0);
    int sendTo(const iovec* buffers, size_t length, const Address::ptr to, int flag = 0);

    int recv(void* buffer, size_t length, int flag = 0);
    int recv(iovec* buffer, size_t length, int flag = 0);
    int recvFrom(void* buffer, size_t length, Address::ptr from, int flag = 0);
    int recvFrom(iovec* buffer, size_t length, Address::ptr from, int flag = 0);

    Address::ptr getRemoteAddress();
    Address::ptr getLocalAddress();

    int getFamily() const { return m_family;}
    int getType() const {return m_type;}
    int getProtocol() const { return m_protocol;}

    bool isConnected() const {return m_isConnected;}
    bool isValid() const;
    int getError();

    std::ostream& dump(std::ostream& os) const;
    int getSocket() const { return m_sock;}

    bool cancelRead();
    bool cancelWrite();
    bool cancelAccept();
    bool cancelAll();

private:
    void initSock();
    void newSock();
private:
    int m_sock;
    int m_family;
    int m_type;
    int m_protocol;
    bool m_isConnected;

    Address::ptr m_localAddress;
    Address::ptr m_remoteAddress;

};

std::ostream& operator<<(std::ostream& os, const Socket& addr);

}
#endif