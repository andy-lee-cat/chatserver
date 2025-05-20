#include "chatserver.hpp"
#include "chatservice.hpp"

#include <iostream>
#include <signal.h>

using namespace std;

// 处理服务器^C结束后，重制user的状态信息
void resetHandler(int) {
    ChatService::instance()->reset();
    exit(0);
}

int main(int argc, char **argv) {
    char *ip;
    uint16_t port;
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <ip> <port>" << endl;
        cout << "Use default ip and port" << endl;
        ip = (char *)"127.0.0.1";
        port = 6000;
    } else {
        ip = argv[1];
        port = atoi(argv[2]);
    }

    signal(SIGINT, resetHandler);

    EventLoop loop;
    InetAddress addr(ip, port);
    ChatServer server(&loop, addr, "myChatServer");

    server.start();
    loop.loop();

    return 0;
}