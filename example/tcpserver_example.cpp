#include "lnet.h"
#include "tcpserver.h"
#include "connection.h"
#include "address.h"
#include "logger.h"

using namespace lnet;
using namespace std;


void onConnectEvent(const shared_ptr<Connection> &con, CONNECT_EVENT event, const void *context) {
    switch(event) {
        case READ: {
            const string *buf = static_cast<const string*>(context);
            LOG_INFO("rev %s", buf->c_str());
            con->send(*buf);
            break;
        }
        default: {
            break;
        }
    }
}

int main() {
    TcpServer server;
    server.listen(Address(11192), bind(&onConnectEvent, _1, _2, _3));
    server.start(1);
    LOG_INFO("server stop");
    return 0;
}
