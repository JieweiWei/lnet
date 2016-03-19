#include "ioloop.h"
#include "logger.h"
#include "connection.h"
#include "lnet.h"
#include "sockutil.h"
#include "address.h"

using namespace lnet;
using namespace std;

int i = 0;

void onConnectEvent(const shared_ptr<Connection> &con, CONNECT_EVENT event, const void *context) {
    switch (event) {
        case READ: {
            const string *buf = (const string*)context;
            LOG_INFO("echo %s", buf->c_str());
            char buffer[1024] = {'\0'};
            int n = snprintf(buffer, sizeof(buffer), "hello %d", i);
            con->send(string(buffer, n));
            if (++i > 10) {
                con->shutdown();
            }
            break;
        }
        case CLOSE: {
            LOG_INFO("close");
            break;
        }
        case ERROR: {
            LOG_INFO("error");
            break;
        }
        case CONNECT: {
            LOG_INFO("connect");
            con->send("hello world");
            break;
        }
        default: {
            break;
        }
    }
}

int main() {
    IOLoop loop;
    shared_ptr<Connection> con = make_shared<Connection>(&loop, SockUtil::createSocket());
    con->setEventCallback(bind(&onConnectEvent, _1, _2, _3));
    con->connect(Address(11192));
    loop.start();
    return 0;
}
