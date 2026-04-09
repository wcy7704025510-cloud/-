#ifndef _MYSQL_H
#define _MYSQL_H

#include <mysql/mysql.h>
#include <list>
#include <string>
#include "common/packdef.h"

using namespace std;

class CMysql {
public:
    // 连接数据库
    int ConnectMysql(const char *server, const char *user, const char *password, const char *database);
    // 查询数据
    int SelectMysql(char* szSql, int nColumn, list<string>& lst);
    // 更新/插入/删除数据
    int UpdataMysql(char *szsql);
    // 断开数据库连接
    void DisConnect();

private:
    MYSQL *conn;                 // MySQL连接句柄
    pthread_mutex_t m_lock;      // 线程互斥锁，保证多线程安全
};

#endif // _MYSQL_H

/*
CREATE TABLE IF NOT EXISTS `t_user` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '用户唯一ID',
  `tel` VARCHAR(20) NOT NULL COMMENT '用户手机号',
  `password` VARCHAR(50) NOT NULL COMMENT '用户密码',
  `name` VARCHAR(50) NOT NULL COMMENT '用户昵称',
  `icon` INT NOT NULL DEFAULT 1 COMMENT '头像标识（默认1）',
  `feeling` VARCHAR(255) NOT NULL DEFAULT '比较懒，什么也没有写' COMMENT '个性签名',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_tel` (`tel`),  -- 手机号唯一，防止重复注册
  UNIQUE KEY `idx_name` (`name`)  -- 用户名唯一，防止重复注册
) ;
 */
