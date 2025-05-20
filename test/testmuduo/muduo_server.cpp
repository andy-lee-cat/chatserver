#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>
#include <string>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

// 一个std::bind的示例
#if 0
class A {
public:
    void foo(int x) {
        std::cout << "x=" << x << std::endl;
    }
};

A a;
auto f = std::bind(&A::foo, &a, 42);  // ✅ 正确
f(); // 调用 a.foo(42)
#endif

class ChatServer 
{
public:
    ChatServer(EventLoop* loop,  // 事件循环
               const InetAddress& listenAddr, // IP+port
               const string& nameArg) // 名字
        : _server(loop, listenAddr, nameArg), _loop(loop) 
    {
        // 给服务器注册 用户连接的创建和断开 回调
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));        

        // 给服务器注册 用户读写事件 回调
        _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

        // 设置服务器端端线程数量, 1个I/O线程，3个worker线程
        _server.setThreadNum(4);
    }

    // 开启事件循环
    void start() {
        _server.start();
    }
private: 
    // 用户连接断开
    void onConnection(const TcpConnectionPtr &conn) 
    {
        if (conn->connected()) {
            cout << conn->peerAddress().toIpPort() << " -> " <<
                conn->localAddress().toIpPort() << " state:online " << endl;
        } else {
            cout << conn->peerAddress().toIpPort() << " -> " <<
                conn->localAddress().toIpPort() << " state:offline " << endl;
            conn->shutdown(); // close(fd)
        }
    }

    // 用户读写事件
    void onMessage(const TcpConnectionPtr &conn,  // 连接
                    Buffer *buffer,  // 缓冲区 
                    Timestamp time) 
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recv data: " << buf << " time: " << time.toString() << endl;
        conn->send(buf);
        // _loop->quit()  // 如果服务器也退出的话
    }

    TcpServer _server;
    EventLoop *_loop;
};

int main() {
    EventLoop loop; // epoll
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "myChatServer");
    
    server.start(); // listenfd epoll_ctl=>epoll
    loop.loop();  // epoll_wait以阻塞方式等待新用户连接

    return 0;
}
