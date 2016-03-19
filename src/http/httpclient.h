/*
 * ==================================================================================
 *
 *          Filename: httpclient.h
 *
 *       Description:
 *
 *            Verson: 1.0
 *           Created: 2016-03-13
 *          Compiler: g++
 *
 *            Author: Jiewei Wei <weijieweijerry@163.com>
 *           Company: Sun Yat-sen University
 *
 * ==================================================================================
 */

#ifndef __LNET_HTTPCLINET_H__
#define __LNET_HTTPCLINET_H__

#include "lnethttp.h"

extern "C" {
    #include "http_parser.h"
}

LNET_NAMESPACE_BEGIN

class HttpConnector;

class HttpClient : public std::enable_shared_from_this<HttpClient> {
public:
    HttpClient(IOLoop *loop, int maxClients = 10);
    ~HttpClient();
    DISALLOW_COPY_AND_ASSIGN(HttpClient);

    void bindDevice(const std::string &device) { m_device = device; }

    void request(
        const std::string &url,
        const ResponseCallback &callback,
        enum http_method method = HTTP_GET,
        const Headers &headers = Headers(),
        const std::string &body = ""
    );

private:
    void request(HttpRequest &request, const ResponseCallback &callback);
    void onResponse(
        const std::shared_ptr<HttpConnector> &wcon,
        const HttpResponse &resp,
        RESPONSE_EVENT event,
        const ResponseCallback &callback
    );
    void onConnect(
        const std::shared_ptr<HttpConnector> &con,
        bool connected,
        const std::string &requestData,
        const ResponseCallback &callback
    );
    void pushConnect(const std::shared_ptr<HttpConnector> &con);
    std::shared_ptr<HttpConnector> popConnect(uint32_t ip);

private:
    IOLoop *m_loop;
    int m_maxClients;
    using IpConnector = std::multimap<uint32_t, std::weak_ptr<HttpConnector>>;
    IpConnector m_connectors;
    std::string m_device;

}; /* end of class HttpClient */

LNET_NAMESPACE_END

#endif /* end of define __LNET_HTTPCLINET_H__ */
