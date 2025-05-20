#include "json.hpp"

#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <unordered_map>

using namespace std;
using json = nlohmann::json;

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "group.hpp"
#include "user.hpp"
#include "public.hpp"

// 控制主菜单程序是否运行
bool isMainMenuRunning = false;
User g_currentUser;
vector<User> g_currentUserFriendList;
vector<Group> g_currentUserGroupList;

void showCurrentUserData();
void readTaskHandler(int clientfd);
string getCurrentTime();
void mainMenu(int clientfd);

int main(int argc, char **argv) {
    char *ip;
    uint16_t port;

    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <ip> <port>" << endl;
        cout << "Use default ip and port" << endl;
        // exit(-1);
        ip = (char *)"127.0.0.1";
        port = 8000;
    }
    ip = argv[1];
    port = atoi(argv[2]);

    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd < 0) {
        cerr << "socket error" << endl;
        exit(-1);
    }

    sockaddr_in server;
    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    if (connect(clientfd, (sockaddr *)&server, sizeof(server)) < 0) {
        cerr << "connect error" << endl;
        exit(-1);
    }

    for (;;) {
        cout << "====================" << endl;
        cout << "1. Login" << endl;
        cout << "2. Register" << endl;
        cout << "3. Exit" << endl;
        cout << "====================" << endl;

        int choice;
        cin >> choice;
        cin.get();

        switch(choice) {
        case 1: {
            // login
            int id = 0;
            char pwd[50] = {0};
            cout << "Enter your id: ";
            cin >> id;
            cin.get();
            cout << "Enter your password: ";
            cin.getline(pwd, 50);

            json js;
            js["msgid"] = LOGIN_MSG;
            js["id"] = id;
            js["password"] = pwd;
            string request = js.dump();

            int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
            if (len < 0) {
                cerr << "send login msg error" << endl;
            } else {
                char buffer[1024] = {0};
                len = recv(clientfd, buffer, sizeof(buffer), 0);
                if (len < 0) {
                    cerr << "recv login msg error" << endl;
                } else {
                    json response = json::parse(buffer);
                    if (response["errno"].get<int>() != 0) {
                        cerr << response["errmsg"] << endl;
                    } else {
                        // 登录成功
                        g_currentUser.setId(response["id"].get<int>());
                        g_currentUser.setName(response["name"]);

                        if (response.contains("friends")) {
                            vector<string> vec = response["friends"];
                            for (string &str : vec) {
                                json js = json::parse(str);
                                User user;
                                user.setId(js["id"].get<int>());
                                user.setName(js["name"]);
                                user.setState(js["state"]);
                                g_currentUserFriendList.push_back(user);
                            }
                        }

                        if (response.contains("groups")) {
                            vector<string> vec = response["groups"];
                            for (string &str : vec) {
                                json js = json::parse(str);
                                Group group;
                                group.setId(js["id"].get<int>());
                                group.setName(js["groupname"]);
                                group.setDesc(js["groupdesc"]);

                                vector<string> vec2 = js["users"];
                                for (string &str2 : vec2) {
                                    json js2 = json::parse(str2);
                                    GroupUser user;
                                    user.setId(js2["id"].get<int>());
                                    user.setName(js2["name"]);
                                    user.setState(js2["state"]);
                                    user.setRole(js2["role"]);
                                    group.getUsers().push_back(user);
                                }
                                g_currentUserGroupList.push_back(group);
                            }
                        }

                        showCurrentUserData();

                        if (response.contains("offlinemsg")) {
                            vector<string> vec = response["offlinemsg"];
                            for (string &str : vec) {
                                json js = json::parse(str);
                                int msgtype = js["msgid"].get<int>();
                                if (msgtype == ONE_CHAT_MSG) {
                                    cout << js["time"].get<string>() << " [" << js["id"] << "]" 
                                        << js["name"].get<string>()
                                        << " said: " << js["msg"].get<string>() << endl;
                                } else if (msgtype == GROUP_CHAT_MSG) {
                                    cout << "群消息[" << js["groupid"] << "]:" << js["time"].get<string>() 
                                        << " [" << js["id"] << "]" << js["name"].get<string>()
                                        << " said: " << js["msg"].get<string>() << endl;
                                }
                            }
                        }

                        static int readthreadnumber = 0;
                        if (readthreadnumber == 0) {
                            std::thread readTask(readTaskHandler, clientfd);
                            readTask.detach();
                            readthreadnumber++;
                        }

                        isMainMenuRunning = true;
                        mainMenu(clientfd);
                    }
                }
            }
        }
        break;
        case 2: {
            // register
            char name[50];
            char pwd[50];
            cout << "Enter your name: ";
            cin.getline(name, 50);
            cout << "Enter your password: ";
            cin.getline(pwd, 50);
            json js;
            js["msgid"] = REG_MSG;
            js["name"] = name;
            js["password"] = pwd;
            string request = js.dump();
            int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
            if (len < 0) {
                cerr << "send reg msg error" << endl;
            } else {
                char buffer[1024] = {0};
                len = recv(clientfd, buffer, sizeof(buffer), 0);
                if (len < 0) {
                    cerr << "recv reg msg error" << endl;
                } else {
                    json response = json::parse(buffer);
                    if (response["errno"].get<int>() != 0) {
                        cerr << name << " is already registered" << endl;
                    } else {
                        cout << name << " register success, userid is " << response["id"]
                            << ", do not forget it!" << endl;
                    }
                }
            }
        }
        break;
        case 3: {
            close(clientfd);
            exit(0);
        }        
        default: {
            cout << "Invalid choice" << endl;
            break;
        }
        }
    }
}

void showCurrentUserData() {
    cout << "========login user========" << endl;
    cout << "current login user => id:" << g_currentUser.getId() 
        << " name:" << g_currentUser.getName() << endl;
    cout << "--------friend list--------" << endl;
    if (!g_currentUserFriendList.empty()) {
        for (User &user : g_currentUserFriendList) {
            cout << user.getId() << " " << user.getName() 
                << " " << user.getState() << endl;
        }
    }
    cout << "--------group list---------" << endl;
    if (!g_currentUserGroupList.empty()) {
        for (Group &group : g_currentUserGroupList) {
            cout << group.getId() << " " << group.getName() 
                << " " << group.getDesc() << endl;
            cout << "--------group user list---------" << endl;
            for (GroupUser &user : group.getUsers()) {
                cout << user.getId() << " " << user.getName() 
                    << " " << user.getState() 
                    << " " << user.getRole() << endl;
            }
        }
    }
    cout << "============================" << endl;
}

void readTaskHandler(int clientfd) {
    for (;;) {
        char buffer[1024] = {0};
        int len = recv(clientfd, buffer, sizeof(buffer), 0);
        if (len == -1 || len == 0) {
            close(clientfd);
            exit(-1);
        }

        // 接受chatserver转发的数据，反序列化生成json对象
        json js = json::parse(buffer);
        int msgtype = js["msgid"].get<int>();
        if (msgtype == ONE_CHAT_MSG) {
            cout << js["time"].get<string>() << " [" << js["id"] << "]" 
                << js["name"].get<string>()
                << " said: " << js["msg"].get<string>() << endl;
        } else if (msgtype == GROUP_CHAT_MSG) {
            cout << "群消息[" << js["groupid"] << "]:" << js["time"].get<string>() 
                << " [" << js["id"] << "]" << js["name"].get<string>()
                << " said: " << js["msg"].get<string>() << endl;
        }
    }
}

// "help" command handler
void help(int fd = 0, string str = "");
// "chat" command handler
void chat(int, string);
// "addfriend" command handler
void addfriend(int, string);
// "creategroup" command handler
void creategroup(int, string);
// "addgroup" command handler
void addgroup(int, string);
// "groupchat" command handler
void groupchat(int, string);
// "loginout" command handler
void loginout(int, string);

// 系统支持的客户端命令列表
unordered_map<string, string> commandMap = {
    {"help", "显示所有支持的命令，格式help"},
    {"chat", "一对一聊天，格式chat:friendid:message"},
    {"addfriend", "添加好友，格式addfriend:friendid"},
    {"creategroup", "创建群组，格式creategroup:groupname:groupdesc"},
    {"addgroup", "加入群组，格式addgroup:groupid"},
    {"groupchat", "群聊，格式groupchat:groupid:message"},
    {"loginout", "注销，格式loginout"}
};

// 注册系统支持的客户端命令处理
unordered_map<string, function<void(int, string)>> commandHandlerMap = {
    {"help", help},
    {"chat", chat},
    {"addfriend", addfriend},
    {"creategroup", creategroup},
    {"addgroup", addgroup},
    {"groupchat", groupchat},
    {"loginout", loginout}
};

string getCurrentTime() {
    auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm *ptm = localtime(&tt);
    char date[60] = {0};
    sprintf(date, "%d-%02d-%02d %02d:%02d:%02d",
            (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
            (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
    return std::string(date);
}

void mainMenu(int clientfd) {
    help();
    char buffer[1024] = {0};
    while (isMainMenuRunning) {
        cin.getline(buffer, sizeof(buffer));
        string commandbuf(buffer);
        string command;
        int idx = commandbuf.find(":");
        if (idx == -1) {
            command = buffer;
        } else {
            command = commandbuf.substr(0, idx);
        }
        auto it = commandHandlerMap.find(command);
        if (it == commandHandlerMap.end()) {
            cout << "Invalid command" << endl;
            continue;
        } else {
            it->second(clientfd, commandbuf.substr(idx + 1, commandbuf.size() - idx)); 
        }
    }
}

void help(int, string) {
    cout << "show command list >>> " << endl;
    for (auto &[cmd, desc] : commandMap) {
        cout << cmd << " : " << desc << endl;
    }
    cout << "============================" << endl;
}

void addfriend(int clientfd, string str) {
    int friendid = atoi(str.c_str());
    json js;
    js["msgid"] = ADD_FRIEND_MSG;
    js["id"] = g_currentUser.getId();
    js["friendid"] = friendid;
    string buffer = js.dump();

    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (len == -1) {
        cerr << "send addfriend msg error -> " << buffer << endl;
    }
}

void chat(int clientfd, string str) {
    int idx = str.find(":"); // friendid:message
    if (-1 == idx) {
        cerr << "chat command invalid!" << endl;
        return;
    }

    int friendid = atoi(str.substr(0, idx).c_str());
    string message = str.substr(idx + 1, str.size() - idx);

    json js;
    js["msgid"] = ONE_CHAT_MSG;
    js["id"] = g_currentUser.getId();
    js["name"] = g_currentUser.getName();
    js["toid"] = friendid;
    js["msg"] = message;
    js["time"] = getCurrentTime();
    string buffer = js.dump();

    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (len == -1) {
        cerr << "send chat msg error -> " << buffer << endl;
    }
}

void creategroup(int clientfd, string str) {
    int idx = str.find(":"); // groupname:groupdesc
    if (idx == -1) {
        cerr << "creategroup command invalid!" << endl;
        return;
    }

    string groupname = str.substr(0, idx);
    string groupdesc = str.substr(idx + 1, str.size() - idx);

    json js;
    js["msgid"] = CREATE_GROUP_MSG;
    js["id"] = g_currentUser.getId();
    js["groupname"] = groupname;
    js["groupdesc"] = groupdesc;
    string buffer = js.dump();

    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (len == -1) {
        cerr << "send creategroup msg error -> " << buffer << endl;
    }
}

void addgroup(int clientfd, string str) {
    int groupid = atoi(str.c_str());
    json js;
    js["msgid"] = ADD_GROUP_MSG;
    js["id"] = g_currentUser.getId();
    js["groupid"] = groupid;
    string buffer = js.dump();

    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (len == -1) {
        cerr << "send addgroup msg error -> " << buffer << endl;
    }
}

void groupchat(int clientfd, string str) {
    int idx = str.find(":"); // groupid:message
    if (idx == -1) {
        cerr << "groupchat command invalid!" << endl;
        return;
    }

    int groupid = atoi(str.substr(0, idx).c_str());
    string message = str.substr(idx + 1, str.size() - idx);

    json js;
    js["msgid"] = GROUP_CHAT_MSG;
    js["id"] = g_currentUser.getId();
    js["name"] = g_currentUser.getName();
    js["groupid"] = groupid;
    js["msg"] = message;
    js["time"] = getCurrentTime();
    string buffer = js.dump();

    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (len == -1) {
        cerr << "send groupchat msg error -> " << buffer << endl;
    }
}

void loginout(int clientfd, string) {
    json js;
    js["msgid"] = LOGINOUT_MSG;
    js["id"] = g_currentUser.getId();
    string buffer = js.dump();

    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (len == -1) {
        cerr << "send loginout msg error -> " << buffer << endl;
    } else {
        g_currentUserFriendList.clear();
        g_currentUserGroupList.clear();
        isMainMenuRunning = false;
    }
}
