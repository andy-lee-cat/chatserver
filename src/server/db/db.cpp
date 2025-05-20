#include "db.hpp"

// 数据库配置信息
static string server = "127.0.0.1";
static string user = "skycat";
static string password = "skycat";
static string dbname = "chatdb";

// 初始化数据库连接
MySQL::MySQL() {
    _conn = mysql_init(nullptr);
}

// 释放数据库连接资源
MySQL::~MySQL() {
    if (_conn != nullptr)
        mysql_close(_conn);
}

// 连接数据库
bool MySQL::connect() {
    MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(),
                                    password.c_str(), dbname.c_str(), 3306, nullptr, 0);
    if (p != nullptr) {
        // 设置中文编码gbk
        mysql_query(_conn, "set names gbk");
        LOG_INFO << "connect mysql success!";
        return true;
    } else {
        LOG_INFO << "connect mysql fail!";
        return false;
    }
}

// 更新操作
bool MySQL::update(string sql) {
    if (mysql_query(_conn, sql.c_str())) {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
            << sql << "update fail";
        return false;
    }
    return true;
}

// 查询操作
MYSQL_RES *MySQL::query(string sql) {
    if (mysql_query(_conn, sql.c_str())) {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
            << sql << "query fail";
        return nullptr;
    }
    return mysql_use_result(_conn);
}

// 获取连接
MYSQL *MySQL::getConnection() {
    return _conn;
}
