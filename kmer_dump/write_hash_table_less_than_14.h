#pragma once
//#include <unistd.h>

typedef moodycamel::ConcurrentQueue<key_value_32*> CQ_bin_key_value_32;
typedef moodycamel::ConcurrentQueue<char_key_value*> CQ_char_key_value;

class write_hash_table_less_than_14
{
public:
	write_hash_table_less_than_14(char** def_argv, atomic_uint* root_table_def);
	~write_hash_table_less_than_14();
	void read_table();
	void bin2char_block(key_value_32* block_address);
	void bin2char(char_key_value* fna, int& arry_flag, uint_32& kmer, uint_32& value);
	void write_table();

private:
	int k,array_max,length;
	//CQ_bin_key_value_32 CQ_bin_storage; //二进制的储存
	CQ_bin_key_value_32 CQ_bin_recycling; //二进制块回收
	CQ_char_key_value CQ_char_storage; //字符串的储存
	CQ_char_key_value CQ_char_recycling; //字符串块回收
	atomic_uint* root_table;
	fstream char_key_value_file;
	thread t_write;
	bool run;
	bool production;
};

write_hash_table_less_than_14::write_hash_table_less_than_14(char** def_argv, atomic_uint* root_table_def)
{
	k = atoi(def_argv[1]);
	array_max = atoi(def_argv[2]);
	root_table = root_table_def;
	char_key_value_file = fstream("Result.txt", ios::out | ios::binary);
	length = (int)pow(4, k);
	run = true;
	production = true;
}

write_hash_table_less_than_14::~write_hash_table_less_than_14()
{
	run = false;
	t_write.join();
	char_key_value_file.close();
}

inline void write_hash_table_less_than_14::read_table()
{
	int arry_flag = 0; //判断是否数组使用情况
	key_value_32* fna;
	fna = new key_value_32[array_max];
	uint_64 sum = 0;
	uint_64 value_sum = 0;
	auto* thread_pool = new ThreadPool_stable<key_value_32*>([&](key_value_32* block_address) { this->bin2char_block(block_address); }, 30);    ///d_hashtable1->insert_hashtable(block_address)
	for (size_t i = 0; i < length; i++)
	{
		if (arry_flag > (array_max - 1)) //判断数组是否已满
		{
			thread_pool->executor(fna); //将数组地址存入线程池
			if (!CQ_bin_recycling.try_dequeue(fna)) //1:item非空
			{
				fna = new key_value_32[array_max];
			}
			arry_flag = 0; //重置为0
		}
		if (root_table[i])
		{
			fna[arry_flag].key = i;
			fna[arry_flag].value = root_table[i];
			sum++;
			value_sum = value_sum + fna[arry_flag].value;
			arry_flag++;
		}
	}
	thread_pool->executor(fna); //将最后一个数组地址存入线程池
	delete thread_pool;
	production = false;
	while (CQ_bin_recycling.try_dequeue(fna))
	{
		delete[] fna;
	}
	// printf("两个哈希已放入线程池并完成内存释放！\n");
	// cout << "哈希表读取转换阶段项的个数：" << sum << endl;
	// cout << "哈希表读取转换阶段总条数：" << value_sum << endl;
}

void write_hash_table_less_than_14::bin2char_block(key_value_32* block_address)
{
	//cout << "?????????????????" << endl;
	int array_num = 0; //bin
	int arry_flag = 0; //char
	char_key_value* fna;
	if (!CQ_char_recycling.try_dequeue(fna)) //1:item非空
	{
		fna = new char_key_value;
		fna->key = mularr_new(array_max, k + 1); //存放kmer字符串的二维数组(模拟)
		fna->value = new uint_32[array_max]();
	}
	while ((array_num < array_max) && (block_address[array_num].value != 0)) //小于块上限且非空
	{
		bin2char(fna, array_num,block_address[array_num].key, block_address[array_num].value);
		//cout << fna->key[array_num] << "	" << fna->value[array_num] << endl;
		//清除标记
		block_address[array_num].value = 0;
		array_num++;
	}
	CQ_char_storage.enqueue(fna);
	CQ_bin_recycling.enqueue(block_address);
}

inline void write_hash_table_less_than_14::bin2char(char_key_value* fna, int& arry_flag, uint_32& kmer, uint_32& value)
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


inline void write_hash_table_less_than_14::write_table()
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
						//cout << item->key[array_num] << "	" << item->value[array_num] << endl;
						char_key_value_file << item->key[array_num] << "	" << item->value[array_num] << "\n";
						item->value[array_num] = 0;
						array_num++;
					}
					if (production)
					{
						CQ_char_recycling.enqueue(item);
					}
				}
			}
		}
	);
}