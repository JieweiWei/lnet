/*
 * ==================================================================================
 *
 *          Filename: connector.inl
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

#ifndef __LNET_CONNECTOR_INL__
#define __LNET_CONNECTOR_INL__

#include "connection.h"
#include "sockutil.h"
#include "logger.h"

using namespace std;

LNET_NAMESPACE_BEGIN

template <typename Derived>
Connector<Derived>::Connector() {
}

template <typename Derived>
Connector<Derived>::~Connector() {
}

template <typename Derived>
int Connector<Derived>::connect(IOLoop *loop, const Address &addr,
    const ConnectorCallback &callback, const std::string &device) {
    int sockFd = SockUtil::createSocket();
    if (sockFd < 0) {
        return sockFd;
    }
    if (!device.empty()) {
         SockUtil::bindDevice(sockFd, device);
    }
    shared_ptr<Connection> con = make_shared<Connection>(loop, sockFd);
    m_connnection = con;

    con->setEventCallback(
        bind(&Connector<Derived>::onConConnectEvent, this->shared_from_this(), _1, _2, _3, callback)
    );
    con->connect(addr);
    return 0;
}

template <typename Derived>
void Connector<Derived>::send(const std::string &data) {
    shared_ptr<Connection> con = m_connnection.lock();
    if (con) {
        con->send(data);
    }
}

template <typename Derived>
void Connector<Derived>::shutdown() {
    shared_ptr<Connection> con = m_connnection.lock();
    if (con) {
        con->shutdown();
    }
}

template <typename Derived>
void Connector<Derived>::onConConnectEvent(const shared_ptr<Connection> &con,
    CONNECT_EVENT event, const void *context, const ConnectorCallback &callback) {
    switch (event) {
        case CONNECTING: {
            break;
        }
        case CONNECT: {
            ConnectorCallback cb = move(callback);
            con->setEventCallback(
                bind(&Connector<Derived>::onConnectEvent, this->shared_from_this(), _1, _2, _3)
            );
            cb(this->shared_from_this(), true);
            break;
        }
        default: {
            callback(this->shared_from_this(), false);
            break;
        }
    }
}

template <typename Derived>
void Connector<Derived>::onConnectEvent(const shared_ptr<Connection> &con,
    CONNECT_EVENT event, const void *context) {
    shared_ptr<Derived> t = this->shared_from_this();
    switch(event) {
        case READ: {
            const string *buf = (const string*)context;
            //LOG_DEBUG("handle str %s", buf->c_str());
            t->handleRead(buf->data(), buf->size());
            break;
        }
        case  WRITE_COMPLETE:
            t->handleWriteComplete(context);
            break;
        case ERROR:
            t->handleError(context);
            break;
        case CLOSE:
            t->handleClose(context);
            break;
        default:
            break;
    }
}

LNET_NAMESPACE_END

#endif /* end of define of __LNET_CONNECTOR_INL__ */
