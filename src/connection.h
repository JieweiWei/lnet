/*
 * ==================================================================================
 *
 *          Filename: connection.h
 *
 *       Description:
 *
 *            Verson: 1.0
 *           Created: 2016-03-09
 *          Compiler: g++
 *
 *            Author: Jiewei Wei <weijieweijerry@163.com>
 *           Company: Sun Yat-sen University
 *
 * ==================================================================================
 */

#ifndef __LNET_CONNECTION_H__
#define __LNET_CONNECTION_H__

#include "lnet.h"

#include <string>

LNET_NAMESPACE_BEGIN

class Address;

class Connection : public std::enable_shared_from_this<Connection> {
public:
    enum Status {
        None,
        Connecting,
        Connected,
        DisConnecting,
        Disconnected
    };

public:
    Connection(IOLoop *loop, int sockFd);
    ~Connection();
    DISALLOW_COPY_AND_ASSIGN(Connection);

    int getSockFd() const { return m_sockFd; }

    void setEventCallback(const ConnectionEventCallback &callback) { m_callback = callback; }
    void clearEventCallback();

    void shutdown(int after = 0);

    int send(const std::string &data);

    void onEstablished();
    void connect(const Address &addr);

    uint64_t lastActiveTime() const { return m_lastActiveTime; }

    bool isConnected() const { return m_status == Connected; }
    bool isConnecting() const { return m_status == Connecting; }

private:
    void onHandler(IOLoop*, int event);
    void handleRead();
    void handleWrite(const std::string &data = "");
    void handleError();
    void handleClose();
    void handleConnect();

    void updateActiveTime();
    bool disconnect();

private:
    ConnectionEventCallback m_callback;
    IOLoop *m_loop;
    int m_sockFd;
    Status m_status;

    uint64_t m_lastActiveTime;
    std::string m_sendBuf;

}; /* end of class Connection */

LNET_NAMESPACE_END

#endif /* end of define __LNET_CONNECTION_H__ */
