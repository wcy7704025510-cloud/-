#ifndef MYMAP_H
#define MYMAP_H

#include <map>
#include <pthread.h>

/*
 * 模板参数:
 *   K - 键类型
 *   V - 值类型
 */
template<class K, class V>
struct MyMap
{
public:

    //构造函数 - 初始化互斥锁
    MyMap() {
        pthread_mutex_init(&m_lock, NULL);
    }

    //析构函数 - 销毁互斥锁
    ~MyMap() {
        pthread_mutex_destroy(&m_lock);
    }


    //查找键对应的值
    //k要查找的键 v用于接收查找到的值（引用传出）
    bool find(K k, V& v)
    {
        pthread_mutex_lock(&m_lock);
        if(m_map.count(k) == 0) {
            pthread_mutex_unlock(&m_lock);
            return false;
        }
        v = m_map[k];
        pthread_mutex_unlock(&m_lock);
        return true;
    }
    // 插入或更新键值对
    //k 键 v 值
    void insert(K k, V v)
    {
        pthread_mutex_lock(&m_lock);
        m_map[k] = v;
        pthread_mutex_unlock(&m_lock);
    }


     //删除指定键 k 要删除的键


    void erase(K k)
    {
        pthread_mutex_lock(&m_lock);
        m_map.erase(k);
        pthread_mutex_unlock(&m_lock);
    }


    //判断键是否存在 k 要判断的键
    bool IsExist(K k)
    {
        bool flag = false;
        pthread_mutex_lock(&m_lock);
        if(m_map.count(k) > 0)
            flag = true;
        pthread_mutex_unlock(&m_lock);
        return flag;
    }


    // 获取当前Map大小（线程安全）元素数量
    int size()
    {
        pthread_mutex_lock(&m_lock);
        int sz = m_map.size();
        pthread_mutex_unlock(&m_lock);
        return sz;
    }


    //清空所有元素（线程安全）
    void clear()
    {
        pthread_mutex_lock(&m_lock);
        m_map.clear();
        pthread_mutex_unlock(&m_lock);
    }

private:
    pthread_mutex_t m_lock;  //互斥锁，保护内部map
    std::map<K, V> m_map;    //内部存储的std::map
};

#endif // MYMAP_H
