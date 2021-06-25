#include "count_15to32.h"
#include "function_def2.h"

void count_15to32::count(char* reads_address)
{
	int arr_num = 0;

	bool succeed;                                        //判断哈希表是否插入成功
	auto update_function = [](uint_32& num)
	{ ++num; }; //布谷鸟的更新函数
	bool cut_flag = 0;                                    // 重叠群切割标志

	uint_32* creads_Block;     //存放creads的内存块

	//记录当前使用的creads块标志块的位置
	//内存块的第0块存放当前数组的使用块数量
	//(creads_end << 16) + creads_len;
	//creadsbiao标志块前16位存放creads_end，后16位存放creads块的使用个数（第0块记录值-标志块位置+1）
	int creads_flag_loc = 0;   // 记录当前使用的creads块标志块的位置
	uint_32 creads_end;        // 记录unsinged int中的空间剩余 0-16个碱基

	uint_32 reads_len = 0; //当reads的长度超过指定阈值时进行截断


	while (!creads_list_addr->try_dequeue(creads_Block))
	{
//        cout << " try直到获取到内存空间! " << endl;
	} //try直到获取到内存空间
	creads_Block[0] = 0; //内存块标记为空
	/********************************************************************************************************************************************************************************************/
	bool flag = false; //标志是否跨行了
	for (int i = 0; i < k; i++)
	{ //检测是否跨行
		if (reads_address[i] == '\n')
		{
			flag = true;
		}
	}

	int length = strlen(reads_address);
	int i = 0;
	if (!flag)
	{ //如果没有跨行
		i = 1;
	}

	int k_flag = 0;           //用于判断kmer是否完整
	uint_32 new_bit = 0;   //用于存放刚拿到的bit
	uint_64 kmer_tmp = 0;  //存放拼接好的kmer
	for (; i < length; i++)
	{
		switch (reads_address[i])
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
		case '\n':
			continue;
		default:
			k_flag = -1;
			reads_len = 0; //重置reads长度计数器
			cut_flag = 0; // 重叠群不再连续
			break;
		}
		if (k_flag == -1)
		{
			k_flag = 0;
			kmer_tmp = 0;
		}
		else if (k_flag < (k - 1))
		{
			kmer_tmp = (kmer_tmp << 2) + new_bit;
			//kmer_tmp_char = kmer_tmp_char + new_char;
			k_flag++;
		}
		else //拼接完整！！！！！！！！！！！！！！！！！
		{
			kmer_tmp = (kmer_tmp << 2) + new_bit;
			reads_len++;
			if (reads_len > 120)
			{
				reads_len = 0; //重置reads长度计数器
				cut_flag = 0;  // 重叠群不再连续
			}
			/********************************************************************************************************************************************************************************************/

			if (bloom_filter->try_insert(kmer_tmp)) // 0:出现过 1:第一次出现
			{
				//第一次出现   继续拼接或者新建(新建时发现满了则提交后继续新建)
				if (cut_flag == 1) // creads连续，继续拼接,临时变量未处理
				{
					// 将kmer最后一个碱基添加到cell末尾
					if (creads_end < 16)
					{
						creads_Block[creads_Block[0]] = (creads_Block[creads_Block[0]] << 2) + (kmer_tmp & 0b11);
						creads_end++;
					}
					else
					{
						creads_Block[++creads_Block[0]] = kmer_tmp & 0b11;
						creads_end = 1;
					}
				}
				else // creads不再连续，将临时变量中的值处理然后新建
				{
					// 结束上一个creads块，将标志块填充完整
					if (creads_Block[0] > 0)
					{
						creads_Block[creads_flag_loc] = (creads_end << 16) + (creads_Block[0] - creads_flag_loc + 1);
					}
					// 检查空间是否足够，足够则继续使用当前块，不够则提交到写线程，同时更替新的内存块
					if ((creads_Block[0] + column) > (array_m - 1))
					{
						creads_list->enqueue(creads_Block); //将内存块址存入线程池
						while (!creads_list_addr->try_dequeue(creads_Block))
						{
//                            cout << " try直到获取到内存空间! " << endl;
						} //try直到获取到内存空间
						creads_Block[0] = 0; //内存块标记为空
					}
					// 新建creads块
					creads_flag_loc = creads_Block[0] + 1;     // 标志块的位置
					creads_Block[0] = creads_Block[0] + 2;      // 内容块位置
					if (k < 17)
					{
						creads_Block[creads_Block[0]] = kmer_tmp;  // 填充内容块
						creads_end = k;
					}
					else
					{
						creads_Block[creads_Block[0]++] = kmer_tmp >> (2 * k - 32);//放入前半截
						creads_Block[creads_Block[0]] = get_end_n_bit(kmer_tmp, 2 * k - 32); //放入后半截
						creads_end = k - 16;
					}
					cut_flag = 1;
				}
			}
			else     // creads不再连续
			{
				//非第一次出现
				//将存放在临时变量中的值放入creads_set，将新kmer放入哈希表中
				//cout_k2++;
				cut_flag = 0;
				succeed = hash_table_1->upsert(kmer_tmp, 1);
				if (!succeed)
				{
					hash_table_2->upsert(kmer_tmp, update_function, 1);
				}
			}

			/********************************************************************************************************************************************************************************************/
			kmer_tmp = kmer_tmp & get_end_2k_2;
		}
	}
	// 结束上一个creads块，将标志块填充完整
	if (creads_Block[0] > 0)
	{
		creads_Block[creads_flag_loc] = (creads_end << 16) + (creads_Block[0] - creads_flag_loc + 1);
		creads_list->enqueue(creads_Block); //将内存块址存入线程池
	}
	else
	{
//        cout << "???" << endl;
		creads_list_addr->enqueue(creads_Block);
	}

	address_array->enqueue(reads_address); //将数组回收
}

void count_15to32::print()
{
	//cout << "size 1 = " << hash_table_1->size() << endl;
	//cout << "load factor 1 = " << hash_table_1->load_factor() << endl;
	//cout << "size 2 = " << hash_table_2->size() << endl;
	//cout << "load factor 2 = " << hash_table_2->load_factor() << endl;

	//cout_kall = cout_k1 + cout_k2;
	//cout << "kmer总数：" << cout_kall << endl;
	//cout << "单次kmer数量：" << cout_k1 << endl;
	//cout << "非单次kmer数量：" << cout_k2 << endl;

	// 存放大表内容到硬盘
	hash_table_1->save(filename);
	delete hash_table_1;

	int hash_table_2_file = open((filename + ".HT2").c_str(), O_RDWR | O_CREAT, 0664);
	const size_t len = hash_table_2->size() * 12ull;
	ftruncate(hash_table_2_file, len);
	int* mapped = (int*)mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, hash_table_2_file, 0);
	auto it = hash_table_2->lock_table();
	uint_64 num = 0;
	uint_64 key;
	uint_64 value;
	for (const auto& hash_table_2_item : it)
	{
		key = hash_table_2_item.first;
		value = hash_table_2_item.second;
		memcpy(mapped + num * 3, &key, 8);
		memcpy(mapped + num * 3 + 2, &value, 4);
		num++;
	}
	munmap((void*)mapped, len);
	close(hash_table_2_file);
}
