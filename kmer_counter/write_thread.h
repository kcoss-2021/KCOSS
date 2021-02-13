#pragma once
#include "concurrentqueue.h"
#include "definition.h"

using namespace std;
class write_thread
{
public:
	write_thread(moodycamel::ConcurrentQueue<c_reads>* creads_list_def, moodycamel::ConcurrentQueue<c_reads>* creads_list_addr_def);
	~write_thread();

private:
	moodycamel::ConcurrentQueue<c_reads>* creads_list; //存放重叠群队列
	moodycamel::ConcurrentQueue<c_reads>* creads_list_addr; //回收重叠群队列
	bool run;
	thread t_write;
	int k;
	int array_m;
	string filename;
	//uint_64 sum;
};

write_thread::write_thread(moodycamel::ConcurrentQueue<c_reads>* creads_list_def, moodycamel::ConcurrentQueue<c_reads>* creads_list_addr_def)
{
	run = true;
	creads_list = creads_list_def;
	creads_list_addr = creads_list_addr_def;
	k = exe_arg.k;
	array_m = exe_arg.m;
	filename = exe_arg.filename;

	t_write = thread([this]
		{
			c_reads item; //包含creads的块 处理完后回收
			int Not_empty ;
			//uint_64 size;
			//int c_reads_file = open("c_reads_file.dat", O_RDWR | O_CREAT, 0664);
			//const uint_64 len = (1 << 30) * 3ull;
			//uint_64 pos = 0;
			//ftruncate(c_reads_file, len);
			//int* mapped = (int*)mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, c_reads_file, 0);

			fstream c_reads_file = std::fstream((filename + ".creads").c_str(), ios::out | ios::binary);

			/**************************************************************************/
			while ((Not_empty = this->creads_list->try_dequeue(item)) || run)
			{
				if (Not_empty) //Not_empty非0时将其内容保存到硬盘
				{
					int arr_num = 0;
					//int size;
					//int end;
					while ((arr_num < array_m) && (item[arr_num][0] != 0))
					{
						//size = item[arr_num][0] & 0xffff;
						//end = item[arr_num][0] >> 16;
						//sum = sum + ((size - 2) * 16 - k + 1 + end);

						c_reads_file.write((char*)item[arr_num], 4 * (item[arr_num][0] & 0xffff));

						//memcpy(mapped + pos, item[arr_num], 4 * size);
						//pos += size;

						//cout << size << endl;
						item[arr_num][0] = 0;
						arr_num++;
					}
					creads_list_addr->enqueue(item);
				}
			}
			/**************************************************************************/
			//munmap((void*)mapped, len);
			//ftruncate(c_reads_file, pos * 4ull);

			c_reads_file.close();

			//close(c_reads_file);
			//释放空间
			//while (creads_list_addr->try_dequeue(item))
			//{
			//	delete[] item[0];
			//	delete[] item;
			//}

		}
	);
}

write_thread::~write_thread()
{
	run = false;
	t_write.join();
	//cout << "保存的单次出现kmer数量：" << sum << endl;
}
