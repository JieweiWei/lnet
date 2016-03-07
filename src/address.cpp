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

#include "sockutil.h"
#include "logger.h"

#include <string.h>
#include <arpa/inet.h>

LNET_NAMESPACE_BEGIN

Address::Address(uint16_t port) {
    bzero(&m_addr, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_addr.sin_port = htons(port);
}

Address::Address(const std::string &ip, uint16_t port) {
    bzero(&m_addr, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &m_addr.sin_addr) <= 0) {
        uint32_t ret = SockUtil::getHostByName(ip);
        if (ret != uint32_t(-1)) {
            m_addr.sin_addr.s_addr = ret;
            return;
        }
        m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        LOG_ERROR("invalid ip %s, use 0.0.0.0 instead", ip.c_str());
    }
}

Address::Address(const struct sockaddr_in &addr) {
    memcpy(&m_addr, &addr, sizeof(addr));
}

uint32_t Address::Address::ip() const {
    return ntohl(m_addr.sin_addr.s_addr);
}

std::string Address::ipstr() const {
    char buf[32] = {'\0'};
    inet_ntop(AF_INET, &m_addr.sin_addr, buf, static_cast<socklen_t>(sizeof(buf)));
    return std::string(buf, 32);
}

uint16_t Address::port() const {
    return ntohs(m_addr.sin_port);
}

LNET_NAMESPACE_END
