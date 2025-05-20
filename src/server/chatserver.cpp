#include "chatserver.hpp"
#include "json.hpp"
#include "chatservice.hpp"

#include <functional>
#include <string>
using namespace std;
using namespace placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop *loop,
        const InetAddress &listenAddr,
        const string &nameArg)
    : _server(loop, listenAddr, nameArg) {
    // 注册连接回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

    // 注册消息回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    // 设置线程数量
    _server.setThreadNum(4);
}

void ChatServer::start() {
    _server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr& conn) {
    // 客户端断开连接
    if (!conn->connected()) {
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}

void ChatServer::onMessage(const TcpConnectionPtr& conn,
                        Buffer* buffer,
                        Timestamp time) 
{
    string buf = buffer->retrieveAllAsString();
    // 数据的反序列化
    json js = json::parse(buf);
    // 目的：完全解耦网络模块和业务模块的代码
    // 通过js["msgid"] 获取->业务hanlder
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());

    // 回调消息绑定好的事件处理器，执行对应的业务处理
    msgHandler(conn, js, time);
}
