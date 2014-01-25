#include "Metazion/Net/ListenSocket.hpp"

DECL_NAMESPACE_MZ_NET_BEGIN

ListenSocket::ListenSocket()
    : m_listenStrategy(*this) {}

ListenSocket::~ListenSocket() {}

void ListenSocket::Reset() {
    Socket::Reset();
    m_localHost.Reset();
    m_listenStrategy.Reset();
}

void ListenSocket::OnStarted() {
    Socket::OnStarted();
    m_listenStrategy.Reset();
}

bool ListenSocket::IsAlive() const {
    bool ret = Socket::IsActive();
    if (ret) {
        return true;
    }

    ret = m_listenStrategy.IsBusy();
    if (ret) {
        return true;
    }

    return false;
}

void ListenSocket::SetLocalHost(const char* ip, int port) {
    m_localHost.SetFamily(AF_INET);
    m_localHost.SetIp(ip);
    m_localHost.SetPort(port);
}

bool ListenSocket::Listen(int backlog) {
    const SockId_t sockId = CreateSockId(TRANSPORT_TCP);
    if (INVALID_SOCKID == sockId) {
        return false;
    }

    int optValue = 1;
    SockLen_t optLength = sizeof(optValue);
    int ret = SetSockOpt(sockId, SOL_SOCKET, SO_REUSEADDR, &optValue, optLength);
    if (SOCKET_ERROR == ret) {
        DestroySockId(sockId);
        return false;
    }

    SockAddr_t* sockAddr = m_localHost.SockAddr();
    SockLen_t sockAddrLen = m_localHost.SockAddrLen();
    ret = ::bind(sockId, sockAddr, sockAddrLen);
    if (SOCKET_ERROR == ret) {
        DestroySockId(sockId);
        return false;
    }

    ret = ::listen(sockId, backlog);
    if (SOCKET_ERROR == ret) {
        DestroySockId(sockId);
        return false;
    }

    AttachSockId(sockId);
    return true;
}

DECL_NAMESPACE_MZ_NET_END