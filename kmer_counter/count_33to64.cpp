#include "count_33to64.h"
#include "function_def2.h"

void count_33to64::count(char** reads_address)
{
	int arr_num = 0;

	bool succeed; //判断哈希表是否插入成功
	auto update_function = [](uint_32& num) { ++num; };//布谷鸟的更新函数
	bool cut_flag = 0; // 重叠群切割标志

	uint_32** creads_set;          //存放creads的二维数组(模拟)
	int creads_len = 0;              //临时存放creads的长度
	uint_32 creads_end = 0;           // 记录unsinged int中的空间剩余 1-32
	int creads_set_num = 0;     //判断creads_set数组使用情况
	uint_32 cell = 0;

	if (!creads_list_addr->try_dequeue(creads_set)) //1:item非空
	{
		creads_set = mularr_uint_32(array_m, column);
	}

	while ((arr_num < array_m) && (strlen(reads_address[arr_num]) != 0))
	{
		//cout << "kmer in  " << reads_address[arr_num] << endl;
		int k_flag = 0; //用于判断kmer是否完整
		uint_32 new_bit; //用于存放刚拿到的bit
		//string new_char; //用于存放刚拿到的char
		//string kmer_tmp_char; //存放拼接好的string 用于布隆过滤器
		__uint128_t kmer_tmp = 0; //存放拼接好的kmer

		cut_flag = 0;// 重叠群切割标志

		if (creads_len > 0)
		{
			creads_set[creads_set_num][creads_len++] = cell;
			creads_set[creads_set_num++][0] = (creads_end << 16) + creads_len;
			creads_len = 0;
		}

		for (size_t i = 0, len = strlen(reads_address[arr_num]); i < len; i++) //处理一条reads
		{
			switch (reads_address[arr_num][i])
			{
			case 'A':
			case 'a':
				new_bit = 0b00;
				//new_char = "A";
				break;
			case 'C':
			case 'c':
				new_bit = 0b01;
				//new_char = "C";
				break;
			case 'G':
			case 'g':
				new_bit = 0b10;
				//new_char = "G";
				break;
			case 'T':
			case 't':
				new_bit = 0b11;
				//new_char = "T";
				break;
			default:
				k_flag = -1;
				break;
			}
			if (k_flag == -1) //遇到奇奇怪怪的字符，重置
			{
				k_flag = 0;
				kmer_tmp = 0;
				//kmer_tmp_char.clear();
			}
			else if (k_flag < (k - 1)) //未拼接完整
			{
				kmer_tmp = (kmer_tmp << 2) + new_bit;
				//kmer_tmp_char = kmer_tmp_char + new_char;
				k_flag++;
			}
			else //拼接完整！！！！！！！！！！！！！！！！！
			{
				kmer_tmp = (kmer_tmp << 2) + new_bit;
				//kmer_tmp_char = kmer_tmp_char + new_char;
				//cout << "kmer_tmp1:" << num2tcga(kmer_tmp,k) << endl;
				///cout << "kmer_tmp2:" << kmer_tmp_char << endl;
				/***********************************************************************************************************/
				//cout_kall++;
				if (bloom_filter->try_insert(kmer_tmp))//0:出现过 1:第一次出现
				{
					//cout_k1++;
					if (cut_flag != 0) //新建重叠群，并把原重叠群vector（如果非空）提交到写入线程
					{
						// 将kmer最后一个碱基添加到cell末尾
						if (creads_end < 16)
						{
							cell = (cell << 2) + (kmer_tmp & 0b11);
							creads_end++;
						}
						else
						{
							creads_set[creads_set_num][creads_len++] = cell;
							cell = kmer_tmp & 0b11;
							creads_end = 1;
						}
					}
					else
					{

						if (creads_set_num > (array_m - 1)) //判断数组是否已满
						{
							creads_list->enqueue(creads_set); //将数组地址存入线程池
							if (!creads_list_addr->try_dequeue(creads_set)) //1:item非空
							{
								creads_set = mularr_uint_32(array_m, column);
							}
							creads_set_num = 0; //重置为0
						}

						// 把kmer放入 vector中
						if (k < 49)
						{
							creads_set[creads_set_num][1] = kmer_tmp >> (2 * k - 32);//放入前半截
							creads_set[creads_set_num][2] = (kmer_tmp >> (2 * k - 64)) & 0xffffffffu;//放入中半截
							cell = get_end_n_bit(kmer_tmp, 2 * k - 64); //放入后半截
							creads_len = 3;
							creads_end = k - 32;
						}
						else
						{
							creads_set[creads_set_num][1] = kmer_tmp >> (2 * k - 32);//放入前半截
							creads_set[creads_set_num][2] = (kmer_tmp >> (2 * k - 64)) & 0xffffffffu;//放入中1半截
							creads_set[creads_set_num][3] = (kmer_tmp >> (2 * k - 96)) & 0xffffffffu;//放入中2半截
							cell = get_end_n_bit(kmer_tmp, 2 * k - 96); //放入后半截
							creads_len = 4;
							creads_end = k - 48;
						}
						cut_flag = 1;
					}

				}
				else
				{
					//非第一次出现
					if (creads_len > 0)
					{
						creads_set[creads_set_num][creads_len++] = cell;
						creads_set[creads_set_num++][0] = (creads_end << 16) + creads_len;
						creads_len = 0;
					}
					//cout_k2++;
					cut_flag = 0;
					//cout << "进哈希表:" << num2tcga(kmer_tmp, k) << endl;
					succeed = hash_table_1->upsert(kmer_tmp, 1);
					if (!succeed)
					{
						hash_table_2->upsert(kmer_tmp, update_function, 1);
					}
				}
				/**********************************************************************************************************/
				kmer_tmp = kmer_tmp & get_end_2k_2;
				//cout << "get_end_2k_2:" << num2tcga(kmer_tmp, k-1) << endl;
				//kmer_tmp_char = kmer_tmp_char.erase(0, 1);
			}
		}
		//清空该条
		reads_address[arr_num][0] = '\0';
		arr_num++;
	}
	if (cut_flag == 0)
	{
		if (creads_set[0][0] != 0)
		{
			creads_list->enqueue(creads_set); //将数组地址存入线程池
		}
	}
	else
	{
		creads_set[creads_set_num][creads_len++] = cell;
		creads_set[creads_set_num++][0] = (creads_end << 16) + creads_len;
		creads_list->enqueue(creads_set); //将数组地址存入线程池
	}
	address_array->enqueue(reads_address); //将数组回收
}

void count_33to64::print()
{
	//cout << "size 1 = " << hash_table_1->size() << endl;
	//cout << "load factor 1 = " << hash_table_1->load_factor() << endl;
	//cout << "size 2 = " << hash_table_2->size() << endl;
	//cout << "load factor 2 = " << hash_table_2->load_factor() << endl;

	//cout << "kmer总数：" << cout_kall << endl;
	//cout << "单次kmer数量：" << cout_k1 << endl;
	//cout << "非单次kmer数量：" << cout_k2 << endl;

	// 存放大表内容到硬盘
	hash_table_1->save(filename, k);
	delete hash_table_1;

	int hash_table_2_file = open((filename + ".HT2").c_str(), O_RDWR | O_CREAT, 0664);
	auto it = hash_table_2->lock_table();
	__uint128_t key = 0;
	uint_32 value = 0;
	//cout << "!!!!" << endl;
	uint_64 byte_len = 0;
	//cout << "!kkk!!!"<<k << endl;
	if (k > 32 && k <= 48)
	{
		byte_len = 12ull;
	}
	else
	{
		byte_len = 16ull;
	}
	const uint_64 slot = byte_len / 4ull + 1ull;
	const uint_64 len = hash_table_2->size() * sizeof(uint_32) * slot;
	ftruncate(hash_table_2_file, len);
	uint_32*  mapped = (uint_32*)mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, hash_table_2_file, 0);
	uint_64 num = 0;

	for (const auto& hash_table_2_item : it)
	{
		key = hash_table_2_item.first;
		value = hash_table_2_item.second;
		memcpy(mapped + num * slot, &key, byte_len);
		memcpy(mapped + num * slot + (slot-1ull), &value, 4);
		num++;
	}

	munmap((void*)mapped, len);
	close(hash_table_2_file);

}

