#include "Mysql.h"

// 连接MySQL数据库
int CMysql::ConnectMysql(const char* server, const char* user, const char* password, const char* database)
{
    conn = mysql_init(NULL);                                  // 初始化MySQL对象
    mysql_set_character_set(conn, "utf8");                    // 设置编码为utf8

    // 连接数据库服务器
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
        return FALSE;                                         // 连接失败返回FALSE
    }

    pthread_mutex_init(&m_lock, NULL);                        // 初始化线程互斥锁
    return TRUE;                                              // 连接成功返回TRUE
}

// 查询数据，结果存入list
int CMysql::SelectMysql(char* szSql, int nColumn, list<string>& lst)
{
    MYSQL_RES *results = NULL;

    pthread_mutex_lock(&m_lock);                             // 加锁，保证线程安全

    // 执行SQL查询语句
    if (mysql_query(conn, szSql)) {
        pthread_mutex_unlock(&m_lock);                       // 查询失败，解锁
        return FALSE;
    }

    results = mysql_store_result(conn);                      // 获取查询结果集
    pthread_mutex_unlock(&m_lock);                           // 解锁

    if (results == NULL) return FALSE;                       // 无结果集返回失败

    MYSQL_ROW record;
    // 遍历每一行记录
    while ((record = mysql_fetch_row(results))) {
        // 遍历每一列，存入list
        for (int i = 0; i < nColumn; i++) {
            lst.push_back(record[i]);
        }
    }

    return TRUE;                                             // 查询成功
}

// 执行更新/插入/删除SQL
int CMysql::UpdataMysql(char *szsql)
{
    if (!szsql) return FALSE;                                // SQL为空直接返回

    pthread_mutex_lock(&m_lock);                             // 加锁

    // 执行SQL语句
    if (mysql_query(conn, szsql)) {
        pthread_mutex_unlock(&m_lock);                       // 执行失败解锁
        return FALSE;
    }

    pthread_mutex_unlock(&m_lock);                           // 解锁
    return TRUE;                                             // 执行成功
}

// 断开数据库连接
void CMysql::DisConnect()
{
    mysql_close(conn);                                       // 关闭MySQL连接
}
