#ifndef def_H
#define def_H

#include "deal_c_reads.h"
#include "deal_c_reads2.h"

using namespace std;

//void deal_hash(char** argv, cuckoo_hash_map* hash_table,string position)
//{
//	uint_64 sum = 0;
//	uint_64 sum_item = 0;
//	const int array_size = atoi(argv[2]);
//	moodycamel::ConcurrentQueue<key_value*> address_array; //用于回收数组
//	deal_hashtable1* d_hashtable1 = new deal_hashtable1(argv, hash_table, &address_array);
//	auto* thread_pool = new ThreadPool_stable<key_value*>([&](key_value* block_address) { d_hashtable1->hashtable_insert(block_address); }, 50);
//	fstream input = fstream(position, ios::in | ios::binary);
//	if (!input)
//	{
//		printf("\"%s\"文件打开失败\n", position.c_str());
//		exit(-1);
//	}
//	printf("开始\"%s\"\n", position.c_str());
//	key_value* block = new key_value[array_size];
//	uint_32 array_flag = 0; //判断是否数组使用情况
//	//uint_64 runtime = 0;
//	while (input.peek() != EOF)
//	{
//		//if ((runtime % 10000000) == 0)
//		//{
//			//cout << "运行次数：" << runtime << endl;
//		//}
//		//runtime++;
//		if (array_flag > (array_size - 1)) //判断数组是否已满
//		{
//			thread_pool->executor(block); //将数组地址存入线程池，并重置数组flag
//			if (!address_array.try_dequeue(block)) //1:item非空
//			{
//				block = new key_value[array_size];
//			}
//			array_flag = 0; //重置为0
//		}
//		input.read((char*)(&block[array_flag].key), 8);
//		input.read((char*)(&block[array_flag].value), 4);
//		sum = sum + block[array_flag].value;
//		sum_item++;
//		array_flag++;
//	}
//	//将最后一个数组地址存入线程池
//	thread_pool->executor(block);
//	delete thread_pool;
//	input.close();
//	printf("\"%s\"文件处理完成!\n",position.c_str());
//	while (address_array.try_dequeue(block))
//	{
//		delete[] block;
//	}
//	d_hashtable1->print_cout_all();
//	printf("\"%s\"已读入并完成内存释放！\n", position.c_str());
//	
//	cout << "读取放入哈希表value和:" << sum << endl;
//	cout << "读取放入哈希表条数:" << sum_item << endl;
//}

void deal_creads(char** argv, HashTable1* hash_table_1, cuckoo_hash_map* hash_table_2, CQ_kmer_block* print_list, CQ_kmer_block* kmer_address_list, string position, bool* run_flag)
{
	int k = atoi(argv[1]);
	int array_size = atoi(argv[2]);
	moodycamel::ConcurrentQueue<c_reads*> address_array; //用于回收数组

	deal_c_reads* deal_cread = new deal_c_reads(argv, hash_table_1, hash_table_2, print_list, kmer_address_list, &address_array);
	auto* thread_pool = new ThreadPool_stable<c_reads*>([&](c_reads* block_address) { deal_cread->execute(block_address); }, 50);
	fstream input = fstream(position, ios::in | ios::binary);
	if (!input)
	{
		printf("\"%s\"文件打开失败\n", position.c_str());
		exit(-1);
	}
	c_reads* block = new c_reads[array_size];
	int array_flag = 0; //判断是否数组使用情况
	uint_32 index_H = 0; //最后一个的左移次数
	uint_32 index_L = 0; //获取读取次数
	uint_32 cut_flag = 0; //控制c_reads完整度
	uint_32 tmp = 0; //临时存放
	int sum = 0;
	//while (!input.eof()) //会误读一次
	while (input.peek() != EOF)
	{
		//cout << "运行次数:" << ++sum << endl;
		if (array_flag > (array_size - 1)) //判断数组是否已满
		{
			//cout << "block满了" << endl;
			thread_pool->executor(block); //将数组地址存入线程池，并重置数组flag
			if (!address_array.try_dequeue(block)) //1:item非空
			{
				block = new c_reads[array_size];
				sum++;
			}
			array_flag = 0; //重置为0
		}

		if (cut_flag == 0)
		{
			input.read((char*)&index_H, 4);
			index_L = get_end_n_bit(index_H, 16) - 1;
			index_H = index_H >> 16;
			//cout << "读取次数:" << index_L << endl;
			//cout << "end_point:" << index_H << endl;
			block[array_flag].mt = 16 * (index_L - 1) + index_H - k + 1; //使用for读取时可用
			//cout << "mt:" <<  block[array_flag].mt << endl;
			sum = block[array_flag].mt + sum;
			cut_flag = 1;
		}
		else
		{
			input.read((char*)&tmp, 4);
			//cout << "tmp:" << tmp << endl;
			if (index_L > 1) //追加到block中 2 3 4 5
			{
				block[array_flag].creads = (block[array_flag].creads << 32) ^ (uint_320)tmp;
				index_L--;
			}
			else //处理最后一个可能不完整的块，并将array_flag自增
			{
				block[array_flag].creads = (block[array_flag].creads << (2 * index_H)) ^ (uint_320)tmp;
				//cout << "拼好的creads"<< block[array_flag].creads << endl;
				cut_flag = 0;
				array_flag++;
			}
		}
	}
	//cout << "creads文件读完" << endl;
	thread_pool->executor(block); //将最后一个数组地址存入线程池
	delete thread_pool;
	input.close();
	// printf("\"%s\"文件处理完成!\n", position.c_str());
	while (address_array.try_dequeue(block))
	{
		//cout << block << endl;
		delete[] block;
	}
	// printf("\"%s\"已读入并完成内存释放！\n", position.c_str());
	// cout << "c_reads" << sum << endl;
	// deal_cread->print_result();
	*run_flag = false;
}

void deal_creads2(char** argv, HashTable2* hash_table_1, cuckoo_hash_map2* hash_table_2, CQ_kmer_block* print_list, CQ_kmer_block* kmer_address_list, string position, bool* run_flag)
{
	int k = atoi(argv[1]);
	int array_size = atoi(argv[2]);
	moodycamel::ConcurrentQueue<c_reads*> address_array; //用于回收数组

	deal_c_reads2* deal_cread = new deal_c_reads2(argv, hash_table_1, hash_table_2, print_list, kmer_address_list, &address_array);
	auto* thread_pool = new ThreadPool_stable<c_reads*>([&](c_reads* block_address) { deal_cread->execute(block_address); }, 50);
	fstream input = fstream(position, ios::in | ios::binary);
	if (!input)
	{
		printf("\"%s\"文件打开失败\n", position.c_str());
		exit(-1);
	}
	c_reads* block = new c_reads[array_size];
	int array_flag = 0; //判断是否数组使用情况
	uint_32 index_H = 0; //最后一个的左移次数
	uint_32 index_L = 0; //获取读取次数
	uint_32 cut_flag = 0; //控制c_reads完整度
	uint_32 tmp = 0; //临时存放
	int sum = 0;
	//while (!input.eof()) //会误读一次
	while (input.peek() != EOF)
	{
		//cout << "运行次数:" << ++sum << endl;
		if (array_flag > (array_size - 1)) //判断数组是否已满
		{
			//cout << "block满了" << endl;
			thread_pool->executor(block); //将数组地址存入线程池，并重置数组flag
			if (!address_array.try_dequeue(block)) //1:item非空
			{

				if (sum < 20000)
				{
					block = new c_reads[array_size];
					sum++;
				}
				else
				{
					while (!address_array.try_dequeue(block))
					{
						usleep(20);
					}
				}

				//try
				//{
				//	block = new c_reads[array_size];
				//	//cout << "creads读取 new" << endl;
				//}
				//catch (bad_alloc)
				//{
				//	cout << "creads读取，我没内存了" << endl;
				//	throw "creads读取凉了";
				//}

			}
			array_flag = 0; //重置为0
		}

		if (cut_flag == 0)
		{
			input.read((char*)&index_H, 4);
			index_L = get_end_n_bit(index_H, 16) - 1;
			index_H = index_H >> 16;
			//cout << "读取次数:" << index_L << endl;
			//cout << "end_point:" << index_H << endl;
			block[array_flag].mt = 16 * (index_L - 1) + index_H - k + 1; //使用for读取时可用
			//cout << "mt:" <<  block[array_flag].mt << endl;
			sum = block[array_flag].mt + sum;
			cut_flag = 1;
		}
		else
		{
			input.read((char*)&tmp, 4);
			//cout << "tmp:" << tmp << endl;
			if (index_L > 1) //追加到block中 2 3 4 5
			{
				block[array_flag].creads = (block[array_flag].creads << 32) ^ (uint_320)tmp;
				index_L--;
			}
			else //处理最后一个可能不完整的块，并将array_flag自增
			{
				block[array_flag].creads = (block[array_flag].creads << (2 * index_H)) ^ (uint_320)tmp;
				//cout << "拼好的creads"<< block[array_flag].creads << endl;
				cut_flag = 0;
				array_flag++;
			}
		}
	}
	//cout << "creads文件读完" << endl;
	thread_pool->executor(block); //将最后一个数组地址存入线程池
	delete thread_pool;
	input.close();
	// printf("\"%s\"文件处理完成!\n", position.c_str());
	while (address_array.try_dequeue(block))
	{
		//cout << block << endl;
		delete[] block;
	}
	// printf("\"%s\"已读入并完成内存释放！\n", position.c_str());
	// cout << "c_reads" << sum << endl;
	// deal_cread->print_result();
	*run_flag = false;
}
#endif
