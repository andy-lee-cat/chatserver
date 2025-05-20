#ifndef GROUP_H
#define GROUP_H

#include "groupuser.hpp"

#include <vector>
#include <string>

using namespace std;

// Group类的ORM类
class Group {
public:
    Group(int id = -1, string name = "", string desc = "")
        : _id(id), _name(name), _desc(desc) {}
    void setId(int id) { _id = id; }
    void setName(string name) { _name = name; }
    void setDesc(string desc) { _desc = desc; }

    int getId() const { return _id; }
    string getName() const { return _name; }
    string getDesc() const { return _desc; }
    vector<GroupUser> &getUsers() { return _users; }
private:
    int _id; // 群组id
    string _name; // 群组名称
    string _desc; // 群组描述
    vector<GroupUser> _users; // 群组成员列表
};


#endif