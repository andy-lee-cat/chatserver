#include "offlinemessagemodel.hpp"
#include "db.hpp"

// 存储用户的离线消息
void OfflineMsgModel::insert(int userId, const string &msg) {
    char sql[1024] = {0};
    sprintf(sql, "insert into OfflineMessage(userId, message) values(%d, '%s')", userId, msg.c_str());
    MySQL mysql;
    if (mysql.connect()) {
        mysql.update(sql);
    }
}

// 删除用户的离线消息
void OfflineMsgModel::remove(int userId) {
    char sql[1024] = {0};
    sprintf(sql, "delete from OfflineMessage where userId = %d", userId);
    MySQL mysql;
    if (mysql.connect()) {
        mysql.update(sql);
    }
}

// 查询用户的离线消息
vector<string> OfflineMsgModel::query(int userId) {
    char sql[1024] = {0};
    sprintf(sql, "select message from OfflineMessage where userId = %d", userId);

    vector<string> vec;
    MySQL mysql;
    if (mysql.connect()) {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr) {
                vec.push_back(row[0]);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}
