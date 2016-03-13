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

using namespace std;

LNET_NAMESPACE_BEGIN

template <typename Derivied>
Connector<Derivied>::Connector() {
}

template <typename Derivied>
Connector<Derivied>::~Connector() {
}

template <typename Derivied>
int Connector<Derivied>::connect(IOLoop *loop, const Address &addr,
    const ConnectorCallback &callback, const std::string &device) {
    int sockFd = SockUtil::create();
    if (sockFd < 0) {
        return sockFd;
    }
    if (!device.empty()) {
         SockUtil::bindDevice(sockFd, device);
    }
    m_connnection = make_shared<Connection>(loop, m_sockFd);
    m_connnection->setEventCallback(
        bind(&Connector<Derivied>::onConnnectEvent, shared_from_this(), _1, _2, _3, callback)
    );
    m_connnection->connect(addr);
}

template <typename Derivied>
void Connector<Derivied>::send(const std::string &data) {
    // ???
    shared_ptr<Connection> con = m_connnection.lock();
    if (con) {
        con->send(data);
    }
}

template <typename Derivied>
void Connector<Derivied>::shutdown() {
    shared_ptr<Connection> con = m_connnection.lock();
    if (con) {
        con->shutdown();
    }
}

template <typename Derivied>
void Connector<Derivied>::onConnnectEvent(const shared_ptr<Connection> &con,
    CONNECT_EVENT event, const void *context, const ConnectorCallback &callback) {
    switch (event) {
        case CONNECTING:
            return;
        case CONNECT:
            con->setEventCallback(
                bind(&Connector<Derivied>::onConnnectEvent, shared_from_this(), _1, _2, _3)
            );
            callback(shared_from_this(), true);
            break;
        default:
            callback(shared_from_this(), false);
            return;

    }
}

template <typename Derivied>
void Connector<Derivied>::onConnnectEvent(const shared_ptr<Connection> &con,
    CONNECT_EVENT event, const void *context) {
    shared_ptr<Derivied> t = shared_from_this();
    switch(event) {
        case READ: {
            const string *buf = (const string*)context;
            t->handleRead(buf->data(), buf->size());
            break;
        }
        case  WRITE_COMPLETE:
            t->handWriteConmplete(context);
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
