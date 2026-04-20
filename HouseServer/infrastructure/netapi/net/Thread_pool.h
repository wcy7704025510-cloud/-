#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include "common/packdef.h"

// 任务结构体：封装任务函数与参数
typedef struct
{
    void *(*task)(void*);  // 任务处理函数（返回值也是任意类型）
    void *arg;             // 任务参数
} task_t;

// 线程池核心结构体：管理线程、任务队列、同步锁
typedef struct STRU_POOL_T
{
    STRU_POOL_T(int max, int min, int que_max);

    int thread_max;        // 最大线程数
    int thread_min;        // 最小线程数
    int thread_alive;      // 存活线程数
    int thread_busy;       // 忙碌线程数
    int thread_shutdown;   // 线程池关闭标志
    int thread_wait;       // 需要关闭的线程数

    int queue_max;         // 任务队列最大容量
    int queue_cur;         // 当前任务数
    int queue_front;       // 队头索引
    int queue_rear;        // 队尾索引

    pthread_cond_t not_full;    // 队列非满条件变量
    pthread_cond_t not_empty;   // 队列非空条件变量
    pthread_mutex_t lock;        // 线程池互斥锁

    task_t *queue_task;    // 任务队列
    pthread_t *tids;       // 工作线程ID数组
    pthread_t manager_tid; // 管理线程ID
} pool_t;

// 线程池管理类：封装线程池创建、销毁、任务添加
class thread_pool
{
public:
    // 创建线程池：指定最大/最小线程数、任务队列长度
    bool Pool_create(int max, int min, int que_max);

    // 销毁线程池：释放所有资源
    void pool_destroy();

    // 生产者添加任务：将任务加入队列
    int Producer_add(void *(*task)(void*), void *arg);

    // 工作线程：消费者，处理任务
    static void *Custom(void *arg);

    // 管理线程：动态调整线程数量
    static void *Manager(void *arg);

    // 判断线程是否存活
    static int if_thread_alive(pthread_t tid);

    //用来获取当前任务队列中的任务数量
    int GetQueueSize();

private:
    pool_t *m_pool; // 线程池核心结构指针
};

#endif
