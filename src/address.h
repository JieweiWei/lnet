/*
 * ==================================================================================
 *
 *          Filename: address.h
 *
 *       Description:
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

#ifndef __LNET_ADDRESS_H_
#define __LNET_ADDRESS_H_

#include "lnet.h"

#include <netinet/in.h>
#include <string>

LNET_NAMESPACE_BEGIN

class Address {
public:
    Address(uint16_t port);
    Address(const std::string &ip, uint16_t port);
    Address(const struct sockaddr_in &addr);
    Address& operator=(const Address &) = default;

    uint32_t ip() const;
    std::string ipstr() const;
    uint16_t port() const;

    const struct sockaddr_in& sockaddr() const { return m_addr; }

private:
    struct sockaddr_in m_addr;
}; /* end of class Address */

LNET_NAMESPACE_END

#endif /* end of define __LNET_ADDRESS_H_ */
