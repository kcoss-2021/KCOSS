#pragma once
#include <vector>
#include <thread>
#include "concurrentqueue.h"

using namespace std;

template <typename T>
class ThreadPool_stable
{
public:
	ThreadPool_stable(const function<void(T)>& def_task, int def_core_num);
	~ThreadPool_stable();
	void executor(const T& reads);

private:
	bool run;
	const function<void(T)> task;
	vector<thread>workers; //��Ź����߳��б�
	moodycamel::ConcurrentQueue<T> reads_list; //����������
private:
	void add_worker();
};


template <typename T>
inline ThreadPool_stable<T>::ThreadPool_stable(const function<void(T)>& def_task, int def_core_num) : task(def_task)
{
	//reads_list = reads_list_def;
	//tok = tok_def;
	run = true;
	for (int i = 0; i < def_core_num; i++)
	{
		add_worker();
	}
}

template <typename T>
inline ThreadPool_stable<T>::~ThreadPool_stable()
{
	run = false;
	for (auto& thread : workers)
	{
		thread.join();
	}
}

template <typename T>
inline void ThreadPool_stable<T>::executor(const T& reads)
{
	//cout << "���Ԫ�أ�" << reads << endl;
	reads_list.enqueue(reads);
}

template <typename T>
inline void ThreadPool_stable<T>::add_worker()
{
	workers.emplace_back([this]
		{
			T item;
			bool Not_empty = 0;
			//������Ϊ���������߲�������ʱ��ֹͣ
			while ((Not_empty = reads_list.try_dequeue(item)) || run)
			{
				//cout << "����Ԫ�أ�" << item << endl;
				if (Not_empty) //������в�Ϊ����ִ�����񣬷��������ȴ�������зǿ�
				{
					//cout << "kkk:" << Not_empty << endl;
					this->task(item);
				}
			}
		});
}