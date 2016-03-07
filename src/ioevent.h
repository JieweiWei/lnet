/*
 * ==================================================================================
 *
 *          Filename: ioevent.h
 *
 *       Description:
 *
 *            Verson: 1.0
 *           Created: 2016-03-07
 *          Compiler: g++
 *
 *            Author: Jiewei Wei <weijieweijerry@163.com>
 *           Company: Sun Yat-sen University
 *
 * ==================================================================================
 */

#ifndef __LNET_IOEVENT_H__
#define __LNET_IOEVENT_H__

#include "lnet.h"

LNET_NAMESPACE_BEGIN

struct IOEvent {
    int fd;
    int events;
    IOHandler handler;

    IOEvent(int _fd, int _events, const IOHandler &_handler)
        : fd(_fd)
        , events(_events)
        , handler(_handler) {}

}; /* end of struct IOEvent */

LNET_NAMESPACE_END

#endif /* end of define __LNET_IOEVENT_H__ */
