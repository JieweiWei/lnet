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

LNET_NAMESPACE_BEGIN

class IOLoop;
using IOHandler = std::function<void (IOLoop*, int)>;

LNET_NAMESPACE_END


#endif /* end of define __LNET_H__ */
