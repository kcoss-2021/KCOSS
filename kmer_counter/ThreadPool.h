﻿#pragma once
#include <vector>
#include <thread>
#include "concurrentqueue.h"

using namespace std;

template <typename T>
class ThreadPool
{
public:
	ThreadPool(const function<void(T)>& def_task, int def_core_num, moodycamel::ConcurrentQueue<T>* reads_list_def, moodycamel::ProducerToken* tok_def);
	~ThreadPool();
	void executor(const T& reads);

private:
	bool run;
	const function<void(T)> task;
	vector<thread>workers; //存放工作线程列表
	moodycamel::ConcurrentQueue<T> *reads_list; //存放任务队列
	moodycamel::ProducerToken *tok;
private:
	void add_worker();
};


template <typename T>
inline ThreadPool<T>::ThreadPool(const function<void(T)>& def_task, int def_core_num, moodycamel::ConcurrentQueue<T>* reads_list_def, moodycamel::ProducerToken* tok_def) : task(def_task)
{
	reads_list = reads_list_def;
	tok = tok_def;
	run = true;
	for (int i = 0; i < def_core_num; i++)
	{
		add_worker();
	}
}

template <typename T>
inline ThreadPool<T>::~ThreadPool()
{
	run = false;
	for (auto& thread : workers)
	{
		thread.join();
	}
}

template <typename T>
inline void ThreadPool<T>::executor(const T& reads)
{
	//cout << "入队元素：" << reads << endl;
	reads_list->enqueue(*tok,reads);
}

template <typename T>
inline void ThreadPool<T>::add_worker()
{
	workers.emplace_back([this]
		{
			T item;
			bool Not_empty = 0;
			//当队列为空且生产者不再生产时，停止
			while ((Not_empty = reads_list->try_dequeue_from_producer(*tok,item)) || run)
			{
				//cout << "出队元素：" << item << endl;
				//任务队列不为空则执行任务，否则自旋等待任务队列非空
				if (Not_empty)
				{
					//cout << "kkk:" << Not_empty << endl;
					this->task(item);
				}
			}
		});
}