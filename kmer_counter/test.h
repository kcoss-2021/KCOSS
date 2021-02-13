
//
// Created by assau on 2019/10/30.
//

#ifndef LINUXPROGRAMMING_FIXED_SIZE_THREAD_POOL_NEW_H
#define LINUXPROGRAMMING_FIXED_SIZE_THREAD_POOL_NEW_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

using namespace std;

template <typename T>
class fixed_size_thread_pool_new
{
private:
	vector<thread> workers;
	queue<T> tasks_queue;
	const function<void(T)> task;
	bool stop;
	mutex queue_mutex;
	condition_variable condition;

private:
	void add_worker(size_t worker_id);

public:
	explicit fixed_size_thread_pool_new(const function<void(T)>& task, size_t threads = thread::hardware_concurrency());
	void enqueue(const T& arg);
	~fixed_size_thread_pool_new();
};


/********************************************************************************************************************/



//构造函数
template <typename T>
fixed_size_thread_pool_new<T>::fixed_size_thread_pool_new(const function<void(T)>& task, size_t threads) : task(task), stop(false)
{
	for (size_t i = 0; i < threads; ++i)
	{
		add_worker(i);
	}
}

template <typename T>
void fixed_size_thread_pool_new<T>::add_worker(size_t worker_id)
{
	this->workers.emplace_back([this, worker_id]
		{
			while (true) {
				T task_arg;
				{
					unique_lock<mutex> lock(this->queue_mutex);
					this->condition.wait(lock, [this]{return this->stop || !this->tasks_queue.empty();});
					if (this->stop && this->tasks_queue.empty())
					{
						return;
					}
					task_arg = move(this->tasks_queue.front());
					this->tasks_queue.pop();
				}

				// 执行任务
				this->task(task_arg);
			}
		});
}

template<typename T>
void fixed_size_thread_pool_new<T>::enqueue(const T& arg)
{
	unique_lock<mutex> lock(queue_mutex);
	if (stop)
	{
		throw runtime_error("Enqueuing on stopped thread pool.");
	}
	tasks_queue.emplace(arg);
	condition.notify_one();
}

template <typename T>
fixed_size_thread_pool_new<T>::~fixed_size_thread_pool_new()
{
	{
		unique_lock<mutex> lock(this->queue_mutex);
		this->stop = true;
	}
	this->condition.notify_all();
	for (auto& thread : workers) {
		thread.join();
	}
}

#endif //LINUXPROGRAMMING_FIXED_SIZE_THREAD_POOL_NEW_H
