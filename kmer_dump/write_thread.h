#pragma once

#include "base.h"

using namespace std;
typedef moodycamel::ConcurrentQueue<char**> CQ_kmer_block;

class write_thread
{
public:
	write_thread(char** def_argv,fstream& char_key_value_file, CQ_kmer_block* print_list, CQ_kmer_block* kmer_address_list, bool* run_flag_def);
	~write_thread();
	//void task();

private:
	CQ_kmer_block* print_list; //存放待打印的键值对队列
	CQ_kmer_block* kmer_address_list; //回收存kmer的块
	bool run;
	int fna_array_max;
	thread t_write;
	//fstream char_key_value_file = std::fstream("Result.txt", ios::out | ios::binary);
	fstream& char_key_value_file;
	bool* run_flag;
};

write_thread::write_thread(char** def_argv,fstream &char_key_value_file_def,CQ_kmer_block* print_list_def, CQ_kmer_block* kmer_address_list_def, bool* run_flag_def):char_key_value_file(char_key_value_file_def)
{
	run = true;
	fna_array_max = atoi(def_argv[2])*5;
	//cout << "aaaaaaaaaaaaaaaaa" << array_max << endl;
	print_list = print_list_def;
	kmer_address_list = kmer_address_list_def;
	run_flag = run_flag_def;
	t_write = thread([&]
		{
			char** item;
			int Not_empty;
			//char_key_value_file = char_key_value_file_def;
			//while ((Not_empty = this->print_list->try_dequeue_bulk(item, 30)) || run)
			while ((Not_empty = this->print_list->try_dequeue(item)) || run)
			{
				if (Not_empty) //Not_empty非0时将其内容保存到硬盘
				{
					// 写入硬盘
					int array_num = 0;
					//cout << "item[array_num][0]" << (array_num < array_max)  << endl;
					//cout << array_num << endl;
					//cout <<(item[array_num][0] != '\0')<<endl;
					//cout << "???????????" << endl;
					while ((array_num < fna_array_max) && (item[array_num][0] != '\0')) //小于块上限且非空
					{
						//cout << "hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh" << endl;
						char_key_value_file << item[array_num] << "	" << 1 << "\n";
						item[array_num][0] = '\0';
						array_num++;
					}
					if (*run_flag)
					{
						kmer_address_list->enqueue(item);
					}
					else
					{
						delete[] item[0];
						delete[] item;
						//cout << "write thread 释放空间" << endl;
					}
					//cout << Not_empty <<endl;
					//for (size_t i = 0; i != Not_empty; i++)
					//{

					//	char_key_value_file << item[i]->key << "	" << item[i]->value << "\n";
					//}
				}
				//else
				//{
				//	printf("wait");
				//}
			}
		}
	);
}

write_thread::~write_thread()
{
	run = false;
	t_write.join();
	//char_key_value_file.close();
}
