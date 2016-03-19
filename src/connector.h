/*
 * ==================================================================================
 *
 *          Filename: connector.h
 *
 *       Description:
 *
 *            Verson: 1.0
 *           Created: 2016-03-10
 *          Compiler: g++
 *
 *            Author: Jiewei Wei <weijieweijerry@163.com>
 *           Company: Sun Yat-sen University
 *
 * ==================================================================================
 */

#ifndef __LNET_CONNECTOR_H__
#define __LNET_CONNECTOR_H__

#include "lnet.h"

LNET_NAMESPACE_BEGIN

class Address;

template <typename Derived>
class Connector : public std::enable_shared_from_this<Derived> {
public:
    using ConnectorCallback = std::function<void (const std::shared_ptr<Derived> &con, bool connected)>;

    Connector();
    Connector(const std::string &device);
    ~Connector();
    DISALLOW_COPY_AND_ASSIGN(Connector);

    int connect(
        IOLoop *loop,
        const Address &addr,
        const ConnectorCallback &callback,
        const std::string &device = ""
    );

    std::weak_ptr<Connection> getConnection() const { return m_connnection; }
    std::shared_ptr<Connection> lockConnection() { return m_connnection.lock(); }

    void send(const std::string &data);
    void shutdown();

protected:
    void handleRead(const char*, size_t) {}
    void handWriteConmplete(const void*) {}
    void handleError(const void*) {}
    void handleClose(const void*) {}

private:
    void onConConnnectEvent(
        const std::shared_ptr<Connection> &con,
        CONNECT_EVENT event,
        const void *context,
        const ConnectorCallback &callback
    );
    void onConnnectEvent(
        const std::shared_ptr<Connection> &con,
        CONNECT_EVENT event,
        const void *context
    );

private:
    std::weak_ptr<Connection> m_connnection;

}; /* end of class Connector */

LNET_NAMESPACE_END

#endif /* end of define __LNET_CONNECTOR_H__ */

