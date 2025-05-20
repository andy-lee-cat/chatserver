#ifndef PUBLIC_H
#define PUBLIC_H

/*
server和client的公共文件
*/
/*
登陆示例：
{"msgid":1,"id":1,"password":"123456"}
{"msgid":1,"id":3,"password":"222"}
{"msgid":1,"id":100,"password":"123"}
留的可能的漏洞？
{"msgid":1,"id":-1,"password":""}

聊天消息示例：
{"msgid":5, "id":1, "from":"zhangsan", "toid":3, "msg":"hello2"}
{"msgid":5, "id":3, "from":"lisi", "toid":1, "msg":"你也好"}

添加好友示例：
{"msgid":6, "id":1, "friendid":3}
*/
enum EnMsgType {
    LOGIN_MSG = 1, // 登陆消息
    LOGIN_MSG_ACK, // 登陆响应消息
    LOGINOUT_MSG, // 注销消息
    REG_MSG, // 注册消息
    REG_MSG_ACK, // 注册响应消息
    ONE_CHAT_MSG, // 聊天消息
    ADD_FRIEND_MSG, // 添加好友消息

    CREATE_GROUP_MSG, // 创建群组消息
    ADD_GROUP_MSG, // 加入群组消息
    GROUP_CHAT_MSG, // 群组聊天消息
};

#endif