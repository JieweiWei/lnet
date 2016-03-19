#include "httpclient.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "ioloop.h"

#include <stdio.h>
#include <memory>

using namespace lnet;
using namespace std;

void onResponse(IOLoop *loop, const HttpResponse &response) {

    printf("%d\n", response.code);
    const Headers &headers = response.headers;
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        printf("%s: %s\n", it->first.c_str(), it->second.c_str());
    }
    printf("%d\n", (int)response.body.size());
    loop->stop();
}

int main() {
    IOLoop loop;
    shared_ptr<HttpClient> client = make_shared<HttpClient>(&loop);
    client->request("http://127.0.0.1:11192/", bind(&onResponse, &loop, _1));
    loop.start();
    printf("exit\n");
    return 0;
}
