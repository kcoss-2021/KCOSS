#pragma once
//#include <unistd.h>
//#include "HashTable2.h"

typedef moodycamel::ConcurrentQueue<key_value2*> CQ_bin_key_value2;
typedef moodycamel::ConcurrentQueue<char_key_value*> CQ_char_key_value;

class write_hash_table_33_64
{
public:
	write_hash_table_33_64(char** def_argv, HashTable2* hash_table_1_def, cuckoo_hash_map2* hash_table_2_def, fstream& char_key_value_file_def);
	~write_hash_table_33_64();
	void read_table();
	void bin2char_block(key_value2* block_address);
	void bin2char(char_key_value* fna, int& arry_flag, __uint128_t& kmer, uint_32& value);
	void write_table();

private:
	int k;
	int array_max;
	//CQ_bin_key_value2 CQ_bin_storage; //二进制的储存
	CQ_bin_key_value2 CQ_bin_recycling; //二进制块回收
	CQ_char_key_value CQ_char_storage; //字符串的储存
	CQ_char_key_value CQ_char_recycling; //字符串块回收
	HashTable2* hash_table_1;
	cuckoo_hash_map2* hash_table_2;
	fstream& char_key_value_file;
	thread t_write;
	bool run;
	bool production;
};

write_hash_table_33_64::write_hash_table_33_64(char** def_argv, HashTable2* hash_table_1_def, cuckoo_hash_map2* hash_table_2_def, fstream& char_key_value_file_def) :char_key_value_file(char_key_value_file_def)
{
	k = atoi(def_argv[1]);
	array_max = atoi(def_argv[2]);
	hash_table_1 = hash_table_1_def;
	hash_table_2 = hash_table_2_def;
	run = true;
	production = true;
}

write_hash_table_33_64::~write_hash_table_33_64()
{
	run = false;
	t_write.join();
}

inline void write_hash_table_33_64::read_table()
{
	int arry_flag = 0; //判断是否数组使用情况
	key_value2* fna;
	fna = new key_value2[array_max];
	uint_64 sum = 0;
	uint_64 value_sum = 0;
	auto* thread_pool = new ThreadPool_stable<key_value2*>([&](key_value2* block_address) { this->bin2char_block(block_address); }, 30);    ///d_hashtable1->insert_hashtable(block_address)

	auto hashtable1struct = hash_table_1->table_struct();
	for (size_t i = 0; i < hashtable1struct.m_arraySize; i++)
	{
		if (arry_flag > (array_max - 1)) //判断数组是否已满
		{
			thread_pool->executor(fna); //将数组地址存入线程池
			if (!CQ_bin_recycling.try_dequeue(fna)) //1:item非空
			{
				fna = new key_value2[array_max];
			}
			arry_flag = 0; //重置为0
		}
		if (hashtable1struct.table[i].value != 0)
		{
			fna[arry_flag].key = hashtable1struct.table[i].key;
			fna[arry_flag].value = hashtable1struct.table[i].value;
			value_sum = value_sum + fna[arry_flag].value;
			sum++;
			arry_flag++;
		}
	}

	arry_flag = array_max; //把上一个哈希表的最后一项放入队列中
	auto it = hash_table_2->lock_table();
	for (const auto& hash_table_2_item : it)
	{
		if (arry_flag > (array_max - 1)) //判断数组是否已满
		{
			thread_pool->executor(fna); //将数组地址存入线程池
			if (!CQ_bin_recycling.try_dequeue(fna)) //1:item非空
			{
				fna = new key_value2[array_max];
			}
			arry_flag = 0; //重置为0
		}
		fna[arry_flag].key = hash_table_2_item.first;
		fna[arry_flag].value = hash_table_2_item.second;
		value_sum = value_sum + fna[arry_flag].value;
		sum++;
		arry_flag++;
	}
	//cout << fna << endl;
	thread_pool->executor(fna); //将最后一个数组地址存入线程池
	delete thread_pool;
	production = false;
	//cout << fna << endl;
	while (CQ_bin_recycling.try_dequeue(fna))
	{
		delete[] fna;
	}
	// printf("两个哈希已放入线程池并完成内存释放！\n");
	// cout << "哈希表读取转换阶段" << sum << endl;
	// cout << "哈希表读取转换阶段value和" << value_sum << endl;
}

void write_hash_table_33_64::bin2char_block(key_value2* block_address)
{
	//printf("????????????????????????????????????????????????????????");
	//cout << block_address << endl;
	//usleep(100000000);
	int array_num = 0; //bin
	int arry_flag = 0; //char
	char_key_value* fna;
	if (!CQ_char_recycling.try_dequeue(fna)) //1:item非空
	{
		fna = new char_key_value;
		fna->key = mularr_new(array_max, k + 1); //存放kmer字符串的二维数组(模拟)
		fna->value = new uint_32[array_max]();
		//cout << "fna->value[2]"<<fna->value[2] << endl;
	}
	while ((array_num < array_max) && (block_address[array_num].value != 0)) //小于块上限且非空
	{
		//cout << block_address[array_num].key << endl;
		bin2char(fna, array_num, block_address[array_num].key, block_address[array_num].value);
		//清除标记
		block_address[array_num].value = 0;
		array_num++;
	}
	CQ_char_storage.enqueue(fna);
	CQ_bin_recycling.enqueue(block_address);
}

inline void write_hash_table_33_64::bin2char(char_key_value* fna, int& arry_flag, __uint128_t& kmer, uint_32& value)
{
	for (int i = k - 1; i >= 0; i--)
	{
		int tmp = get_end_n_bit(kmer, 2);
		kmer >>= 2;
		switch (tmp)
		{
		case 0b00:
			fna->key[arry_flag][i] = 'A';
			break;
		case 0b01:
			fna->key[arry_flag][i] = 'C';
			break;
		case 0b10:
			fna->key[arry_flag][i] = 'G';
			break;
		case 0b11:
			fna->key[arry_flag][i] = 'T';
			break;
		}
	}
	fna->value[arry_flag] = value;
}


inline void write_hash_table_33_64::write_table()
{
	t_write = thread([&]
		{
			char_key_value* item;
			int Not_empty;
			while ((Not_empty = this->CQ_char_storage.try_dequeue(item)) || run)
			{
				if (Not_empty) //Not_empty非0时将其内容保存到硬盘
				{
					// 写入硬盘
					int array_num = 0;
					while ((array_num < array_max) && (item->value[array_num] != 0)) //小于块上限且非空
					{
						//cout << item->key[array_num] << endl;
						char_key_value_file << item->key[array_num] << "	" << item->value[array_num] << "\n";
						item->value[array_num] = 0;
						array_num++;
					}
					if (production)
					{
						CQ_char_recycling.enqueue(item);
					}
					//else
					//{
					//	//delete   反正结束了懒得delete
					//}
				}
			}
		}
	);
}