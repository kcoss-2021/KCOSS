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
 * �޶��߳��������̳߳�,
 * ���������� - ������ģʽʵ��.
 * https://github.com/progschj/ThreadPool
 * ��������һ�£�
 * enqueue �������ٴ����������Ǵ�һ��ģ������ T.
 * Ȼ���ڹ����̳߳�ʱ��ʽ�ش���һ��
 */
template <typename T>
class fixed_size_thread_pool_parameterized {
private:
    // �߳��б�
    vector<thread> workers;
    // ������У����������Ĳ���
    queue<T> tasks_queue;

    // �õ�һ������Ĳ���֮�󣬽��䴫��˺�����ִ��.
    const function<void(T)> task;

    // ���ٱ�ǣ�Ϊ���ʱ����Ҫ��ն��У��ù����߳�ִ����������գ���Ȼ��ͣ���߳�.
    bool stop;

    // ������еĶ�ռ��
    mutex queue_mutex;
    condition_variable condition;

private:
    void add_worker(size_t worker_id);

public:
    /**
     * �����߳�������һ���̳߳�
     * @param threads �߳���Ŀ
     */
    explicit fixed_size_thread_pool_parameterized(const function<void(T)> &task, size_t threads = thread::hardware_concurrency());

    /**
     * ���̳߳��ύһ������
     * @param args ִ��һ����������Ӧ�Ĳ���
     * @return
     */
    void enqueue(const T &arg);

    /**
     * �����̳߳�,
     * �������̳߳ص����������б����֮ǰ��һֱ�������õ��߳�.
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
    // �ڴ˹��칤���߳�, �佫��Ϊ�����ߵĽ�ɫ.
    // �书����Ҫ�ǲ��ϵش���������������������ִ��.
    this->workers.emplace_back([this, worker_id]{
        while (true) {
            T task_arg;
            // �ڴ�������������������ʱ��������
            // �� unique_lock �Ļ��Ϳ��������ڹ���ʱ������������ʱ����.
            // ������Ļ���ȷ��һ�´������ڵĴ���Ҫ��������ִ��.
            {
                unique_lock<mutex> lock(this->queue_mutex);

                // ���������Ϊ�յ�ʱ���ù����߳���������Ϊ��Ϊֹ
                // ��Ȼ�������̳߳�Ҫ�����ˣ���ô�����߳�Ҳ���ܼ���������ȥ��.
                this->condition.wait(lock, [this] {
                    return this->stop || !this->tasks_queue.empty();
                });

                // �����ٱ�־Ϊ��ʱ�˳��̣߳�
                // ��������֮ǰҪ��ȷ����������Ѿ������.
                if (this->stop && this->tasks_queue.empty()) {
                    return;
                }

                // �Ӷ�����һ���������
                task_arg = move(this->tasks_queue.front());
                this->tasks_queue.pop();
            }

            // ִ������
            this->task(task_arg);
        }
    });
}

template<typename T>
void fixed_size_thread_pool_parameterized<T>::enqueue(const T &arg) {
    // ��������������һ�����񣬼�������߳�ʵ���Ͼ�����Ϊ�����ߵĽ�ɫ.

    // ��������м�����ʱע�����.
    unique_lock<mutex> lock(queue_mutex);

    // �����ٵ�ʱ����Ȼ�Ͳ��ܼ�������.
    if (stop) {
        throw runtime_error("Enqueuing on stopped thread pool.");
    }

    tasks_queue.emplace(arg);

    condition.notify_one();
}

template <typename T>
fixed_size_thread_pool_parameterized<T>::~fixed_size_thread_pool_parameterized() {
    {
        // �޸����ٱ��֮ǰҪ���õ�������е���
        unique_lock<mutex> lock(this->queue_mutex);
        this->stop = true;
    }

    // ���������߳�ȥ���������
    this->condition.notify_all();

    // ������ǰ�߳�ֱ��������ж���Ϊֹ
    for (auto &thread : workers) {
        thread.join();
    }
}

#endif //LINUXPROGRAMMING_FIXED_SIZE_THREAD_POOL_PARAMETERIZED_H
