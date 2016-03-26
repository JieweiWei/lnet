#include "connection.h"
#include "httpconnection.h"
#include "tcpserver.h"
#include "httpserver.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "logger.h"
#include "address.h"

#include <fcntl.h>
#include <unistd.h>

using namespace std;
using namespace lnet;
using namespace std::placeholders;

void handler(const shared_ptr<HttpConnection> &con, const HttpRequest &request) {
    HttpResponse response;
    response.code = 200;
    response.setContentType("text/html");
    response.setKeepAlive(true);
    response.enableDate();

    response.body.append("hello world");
    con->send(response);
}

int main() {
    TcpServer tcpserver;
    HttpServer httpserver(&tcpserver);
    httpserver.setHttpCallback("/", bind(&handler, _1, _2));
    httpserver.listen(Address(11192));
    LOG_INFO("start server");
    tcpserver.start(4);
    LOG_INFO("stop server");
    return 0;
}
