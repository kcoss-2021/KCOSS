//
// Created by assau on 2019/10/30.
//

#ifndef LINUXPROGRAMMING_FIXED_SIZE_THREAD_POOL_PARAMETERIZED_H
#define LINUXPROGRAMMING_FIXED_SIZE_THREAD_POOL_PARAMETERIZED_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

using namespace std;

/**
 * 限定线程数量的线程池,
 * 基于生产者 - 消费者模式实现.
 * https://github.com/progschj/ThreadPool
 * 参数化了一下，
 * enqueue 方法不再传函数，而是传一个模板类型 T.
 * 然后在构造线程池时显式地传入一个
 */
template <typename T>
class fixed_size_thread_pool_parameterized {
private:
    // 线程列表
    vector<thread> workers;
    // 任务队列，里面放任务的参数
    queue<T> tasks_queue;

    // 得到一个任务的参数之后，将其传入此函数以执行.
    const function<void(T)> task;

    // 销毁标记，为真的时候需要清空队列（用工作线程执行任务来清空），然后停掉线程.
    bool stop;

    // 任务队列的独占锁
    mutex queue_mutex;
    condition_variable condition;

private:
    void add_worker(size_t worker_id);

public:
    /**
     * 根据线程数构造一个线程池
     * @param threads 线程数目
     */
    explicit fixed_size_thread_pool_parameterized(const function<void(T)> &task, size_t threads = thread::hardware_concurrency());

    /**
     * 向线程池提交一个任务
     * @param args 执行一次任务所对应的参数
     * @return
     */
    void enqueue(const T &arg);

    /**
     * 销毁线程池,
     * 不过在线程池的里的任务队列被清空之前会一直阻塞调用的线程.
     */
    ~fixed_size_thread_pool_parameterized();
};

template <typename T>
fixed_size_thread_pool_parameterized<T>::fixed_size_thread_pool_parameterized(const function<void(T)> &task, size_t threads) : task(task), stop(false) {
    for (size_t i = 0; i < threads; ++i) {
        add_worker(i);
    }
}

template <typename T>
void fixed_size_thread_pool_parameterized<T>::add_worker(size_t worker_id) {
    // 在此构造工作线程, 其将作为消费者的角色.
    // 其功能主要是不断地从任务队列里面拿任务出来执行.
    this->workers.emplace_back([this, worker_id]{
        while (true) {
            T task_arg;
            // 在从任务队列拿任务出来的时候先上锁
            // 用 unique_lock 的话就可以让其在构造时上锁，在析构时解锁.
            // 在这里的话能确保一下大括号内的代码要上锁才能执行.
            {
                unique_lock<mutex> lock(this->queue_mutex);

                // 在任务队列为空的时候让工作线程阻塞到不为空为止
                // 当然，若是线程池要销毁了，那么工作线程也不能继续阻塞下去了.
                this->condition.wait(lock, [this] {
                    return this->stop || !this->tasks_queue.empty();
                });

                // 在销毁标志为真时退出线程，
                // 不过在那之前要先确保任务队列已经清空了.
                if (this->stop && this->tasks_queue.empty()) {
                    return;
                }

                // 从队列拿一个任务出来
                task_arg = move(this->tasks_queue.front());
                this->tasks_queue.pop();
            }

            // 执行任务
            this->task(task_arg);
        }
    });
}

template<typename T>
void fixed_size_thread_pool_parameterized<T>::enqueue(const T &arg) {
    // 向任务队列里面加一个任务，加任务的线程实际上就是作为生产者的角色.

    // 往任务队列加任务时注意加锁.
    unique_lock<mutex> lock(queue_mutex);

    // 在销毁的时候自然就不能加任务了.
    if (stop) {
        throw runtime_error("Enqueuing on stopped thread pool.");
    }

    tasks_queue.emplace(arg);

    condition.notify_one();
}

template <typename T>
fixed_size_thread_pool_parameterized<T>::~fixed_size_thread_pool_parameterized() {
    {
        // 修改销毁标记之前要先拿到任务队列的锁
        unique_lock<mutex> lock(this->queue_mutex);
        this->stop = true;
    }

    // 唤醒所有线程去读任务队列
    this->condition.notify_all();

    // 阻塞当前线程直到任务队列读完为止
    for (auto &thread : workers) {
        thread.join();
    }
}

#endif //LINUXPROGRAMMING_FIXED_SIZE_THREAD_POOL_PARAMETERIZED_H
