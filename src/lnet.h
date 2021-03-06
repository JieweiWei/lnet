/*
 * ==================================================================================
 *
 *          Filename: lnet.h
 *
 *       Description: Global definition of lnet.
 *
 *            Verson: 1.0
 *           Created: 2016-03-06
 *          Compiler: g++
 *
 *            Author: Jiewei Wei <weijieweijerry@163.com>
 *           Company: Sun Yat-sen University
 *
 * ==================================================================================
 */

#ifndef __LNET_H__
#define __LNET_H__

#define LNET_NAMESPACE_BEGIN namespace lnet {
#define LNET_NAMESPACE_END   }

#define DISALLOW_COPY_AND_ASSIGN(type)    \
    type(const type&) = delete;           \
    type& operator=(const type&) = delete

#include <functional>
#include <memory>

using namespace std::placeholders;

LNET_NAMESPACE_BEGIN

class IOLoop;
class Signaler;
class Notifier;
class Timer;
class TimingWheel;
class Connection;

enum LNET_EVENT {
    LNET_NONE  = 0x00000000,
    LNET_READ  = 0x00000001,
    LNET_WRITE = 0x00000002,
    LNET_ERROR = 0x80000000
};

enum CONNECT_EVENT {
    ESTABLISHED,
    CONNECT,
    CONNECTING,
    READ,
    WRITE_COMPLETE,
    ERROR,
    CLOSE
};

using IOHandler               = std::function<void (IOLoop*, int event)>;
using ProcessCallback         = std::function<void ()>;
using SignalerHandler         = std::function<void (const std::shared_ptr<Signaler>&, int sig)>;
using NotifierHandler         = std::function<void (const std::shared_ptr<Notifier>&)>;
using NewConnectCallback      = std::function<void (IOLoop*, int clientFd)>;
using TimerHandler            = std::function<void (const std::shared_ptr<Timer>&)>;
using TimingWheelHandler      = std::function<void (const std::shared_ptr<TimingWheel>&)>;
using ConnectionEventCallback = std::function<void (const std::shared_ptr<Connection>&, CONNECT_EVENT, const void *context)>;
using Callback                = std::function<void ()>;
using ServerRunCallback       = std::function<void (IOLoop*)>;

LNET_NAMESPACE_END

#endif /* end of define __LNET_H__ */
