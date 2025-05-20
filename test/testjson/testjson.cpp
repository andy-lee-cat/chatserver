#include "json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <vector>
#include <map>
#include <string>
using namespace std;


// json序列化示例1
string func1() {
    json js;
    // 可以是数组
    js["id"] = {1,2,3,4,5};
    js["msg"]["1"] = "1111";
    js["msg"]["2"] = "222";
    js["msg_type"] = 2;
    js["from"] = "zhangsan";
    js["to"] = "li si";
    string sendBuf = js.dump();
    // cout << sendBuf.c_str() << endl;
    return sendBuf;
}

// json序列化容器
string func2() {
    json js;
    vector<int> vec = {1, 2, 5};
    js["list"] = vec;
    map<int, string> m = {
        {1, "1111"},
        {2, "2222"}
    };
    js["map"] = m;
    string sendBuf = js.dump();
    // cout << js << endl;
    // cout << sendBuf.c_str() << endl;
    return sendBuf;
}

int main() {
    string recvBuf = func1();
    json buf = json::parse(recvBuf);
    cout << buf["msg"] << endl;
    return 0;
}
