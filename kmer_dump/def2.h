#ifndef def2_H
#define def2_H


#include "g_hashtable.h"
#include "g_hashtable2.h"

using namespace std;

string num2tcga_128(__uint128_t kmer, int num)
{
	string k_str = "";
	for (int i = 0; i < num; i++)
	{
		switch (kmer & 0b11)
		{
		case 0b00:
			k_str = "A" + k_str;
			break;
		case 0b01:
			k_str = "C" + k_str;
			break;
		case 0b10:
			k_str = "G" + k_str;
			break;
		case 0b11:
			k_str = "T" + k_str;
			break;
		}
		kmer >>= 2;
	}
	return k_str;
}

void generate_hash_table(char** argv, HashTable1* hash_table1, cuckoo_hash_map* hash_table2, string position1, string position2)
{
	uint_64 sum = 0;
	uint_64 sum_item = 0;
	const int array_size = atoi(argv[2]);
	moodycamel::ConcurrentQueue<key_value*> address_array; //用于回收数组
	g_hashtable* deal_hashtable = new g_hashtable(argv, hash_table1, hash_table2, &address_array);
	auto* thread_pool = new ThreadPool_stable<key_value*>([&](key_value* block_address) { deal_hashtable->hashtable_insert(block_address); }, 50);
	key_value* block = new key_value[array_size];
	uint_32 array_flag = 0; //判断是否数组使用情况

	fstream input1 = fstream(position1, ios::in | ios::binary);
	if (!input1)
	{
		printf("\"%s\"文件打开失败\n", position1.c_str());
		exit(-1);
	}
	// printf("开始\"%s\"\n", position1.c_str());
	while (input1.peek() != EOF)
	{
		if (array_flag > (array_size - 1)) //判断数组是否已满
		{
			thread_pool->executor(block); //将数组地址存入线程池，并重置数组flag
			if (!address_array.try_dequeue(block)) //1:item非空
			{
				block = new key_value[array_size];
			}
			array_flag = 0; //重置为0
		}
		input1.read((char*)(&block[array_flag].key), 8);
		input1.read((char*)(&block[array_flag].value), 4);
		sum = sum + block[array_flag].value;
		sum_item++;
		array_flag++;
	}

	fstream input2 = fstream(position2, ios::in | ios::binary);
	if (!input2)
	{
		printf("\"%s\"文件打开失败\n", position2.c_str());
		exit(-1);
	}
	// printf("开始\"%s\"\n", position2.c_str());
	while (input2.peek() != EOF)
	{
		if (array_flag > (array_size - 1)) //判断数组是否已满
		{
			thread_pool->executor(block); //将数组地址存入线程池，并重置数组flag
			if (!address_array.try_dequeue(block)) //1:item非空
			{
				block = new key_value[array_size];
			}
			array_flag = 0; //重置为0
		}
		input2.read((char*)(&block[array_flag].key), 8);
		input2.read((char*)(&block[array_flag].value), 4);
		sum = sum + block[array_flag].value;
		sum_item++;
		array_flag++;
	}

	//将最后一个数组地址存入线程池
	thread_pool->executor(block);
	delete thread_pool;
	input1.close();
	input2.close();
	// printf("\"%s\"文件处理完成!\n", (position1 + position2).c_str());
	while (address_array.try_dequeue(block))
	{
		delete[] block;
	}
	// deal_hashtable->print_cout_all();

	// printf("\"%s\"已读入并完成内存释放！\n", (position1+position2).c_str());

	// cout << "读取放入哈希表value和:" << sum << endl;
	// cout << "读取放入哈希表条数:" << sum_item << endl;
}


void generate_hash_table2(char** argv, HashTable2* hash_table1, cuckoo_hash_map2* hash_table2, string position1, string position2)
{
	uint_64 sum = 0;
	uint_64 sum_item = 0;
	const int k = atoi(argv[1]);
	const int byte_len = ceil(k / 48.0)*4 +8;
	const int array_size = atoi(argv[2]);

	moodycamel::ConcurrentQueue<key_value2*> address_array; //用于回收数组
	g_hashtable2* deal_hashtable = new g_hashtable2(argv, hash_table1, hash_table2, &address_array);
	auto* thread_pool = new ThreadPool_stable<key_value2*>([&](key_value2* block_address) { deal_hashtable->hashtable_insert(block_address); }, 50);
	key_value2* block = new key_value2[array_size];
	uint_32 array_flag = 0; //判断是否数组使用情况

	fstream input1 = fstream(position1, ios::in | ios::binary);
	if (!input1)
	{
		printf("\"%s\"文件打开失败\n", position1.c_str());
		exit(-1);
	}
	// printf("开始\"%s\"\n", position1.c_str());
	while (input1.peek() != EOF)
	{
		if (array_flag > (array_size - 1)) //判断数组是否已满
		{
			thread_pool->executor(block); //将数组地址存入线程池，并重置数组flag
			if (!address_array.try_dequeue(block)) //1:item非空
			{
				block = new key_value2[array_size];
			}
			array_flag = 0; //重置为0
		}
		input1.read((char*)(&block[array_flag].key), byte_len);
		input1.read((char*)(&block[array_flag].value), 4);
		//cout << "byte_len" << byte_len << endl;
		//cout << num2tcga_128(block[array_flag].key, k) <<"	"<< block[array_flag].value << endl;
		sum = sum + block[array_flag].value;
		sum_item++;
		array_flag++;
	}

	fstream input2 = fstream(position2, ios::in | ios::binary);
	if (!input2)
	{
		printf("\"%s\"文件打开失败\n", position2.c_str());
		exit(-1);
	}
	// printf("开始\"%s\"\n", position2.c_str());
	while (input2.peek() != EOF)
	{
		if (array_flag > (array_size - 1)) //判断数组是否已满
		{
			thread_pool->executor(block); //将数组地址存入线程池，并重置数组flag
			if (!address_array.try_dequeue(block)) //1:item非空
			{
				block = new key_value2[array_size];
			}
			array_flag = 0; //重置为0
		}
		input2.read((char*)(&block[array_flag].key), byte_len);
		input2.read((char*)(&block[array_flag].value), 4);
		sum = sum + block[array_flag].value;
		sum_item++;
		array_flag++;
	}

	//将最后一个数组地址存入线程池
	thread_pool->executor(block);
	delete thread_pool;
	input1.close();
	input2.close();
	// printf("\"%s\"文件处理完成!\n", (position1 + position2).c_str());
	while (address_array.try_dequeue(block))
	{
		delete[] block;
	}
	// deal_hashtable->print_cout_all();

	// printf("\"%s\"已读入并完成内存释放！\n", (position1 + position2).c_str());

	// cout << "读取放入哈希表value和:" << sum << endl;
	// cout << "读取放入哈希表条数:" << sum_item << endl;
}
#endif