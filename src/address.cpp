/*
 * ==================================================================================
 *
 *          Filename: address.cpp
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

#include "address.h"

#include <arpa/inet.h>

LNET_NAMESPACE_BEGIN

Address::Address(uint16_t port) {
    bzero(&m_addr, sizeof(m_addr));
}

Address::Address(const std::string &ip, uint16_t port);

uint32_t Address::ip() const;
uint16_t Address::port() const;

LNET_NAMESPACE_END
