# 数据库表设计

---

## User 表

| 字段名称 | 字段类型      | 字段说明     | 约束                           |
|----------|----------------|--------------|--------------------------------|
| id       | INT            | 用户 id      | PRIMARY KEY, AUTO_INCREMENT   |
| name     | VARCHAR(50)    | 用户名       | NOT NULL, UNIQUE              |
| password | VARCHAR(50)    | 用户密码     | NOT NULL                      |
| state    | ENUM('online', 'offline') | 当前登录状态 | DEFAULT 'offline'             |

---

## Friend 表

| 字段名称 | 字段类型 | 字段说明 | 约束                    |
|----------|----------|----------|-------------------------|
| userid   | INT      | 用户 id  | NOT NULL，联合主键      |
| friendid | INT      | 好友 id  | NOT NULL，联合主键      |

---

## AllGroup 表

| 字段名称  | 字段类型       | 字段说明     | 约束                              |
|-----------|----------------|--------------|-----------------------------------|
| id        | INT            | 组 id        | PRIMARY KEY, AUTO_INCREMENT      |
| groupname | VARCHAR(50)    | 组名称       | NOT NULL                         |
| groupdesc | VARCHAR(200)   | 组功能描述   | DEFAULT ''                        |

---

## GroupUser 表

| 字段名称   | 字段类型                        | 字段说明   | 约束               |
|------------|----------------------------------|------------|--------------------|
| groupid    | INT                              | 组 id      |  联合主键       |
| userid     | INT                              | 组员 id    | 联合主键           |
| grouprole  | ENUM('creator', 'normal')        | 组内角色   | DEFAULT 'normal'   |

---

## OfflineMessage 表

| 字段名称    | 字段类型         | 字段说明           | 约束                           |
| ------- | ------------ | -------------- | ---------------------------- |
| id      | INT          | 消息主键，自增唯一标识    | PRIMARY KEY, AUTO\_INCREMENT |
| userid  | INT          | 接收消息的用户 ID     | NOT NULL                     |
| message | VARCHAR(500) | 离线消息（JSON 字符串） | NOT NULL                     |

---
