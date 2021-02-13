#pragma once


typedef moodycamel::ConcurrentQueue<c_reads*> Concurrent_Queue_c_reads;
typedef moodycamel::ConcurrentQueue<char**> CQ_kmer_block;

class deal_c_reads
{
public:
	deal_c_reads(char** def_argv, HashTable1* hash_table_1, cuckoo_hash_map* hash_table_2, CQ_kmer_block* print_list,CQ_kmer_block* kmer_address_list, Concurrent_Queue_c_reads* address_array)
	{
		k = atoi(def_argv[1]);
		array_max = atoi(def_argv[2]);
		fna_array_max = 5 * array_max;
		//cout << array_max << endl;
		this->hash_table_1 = hash_table_1;
		this->hash_table_2 = hash_table_2;
		this->print_list = print_list;
		this->kmer_address_list = kmer_address_list;
		this->address_array = address_array;
		cout_one = 0;
		cout_divide = 0;
	}

	void execute(c_reads* block_address)
	{
		//cout << block_address << endl;
		//usleep(100000000);
		int array_num = 0;
		const uint_320 zeros;
		const uint_320 ones = ~(~zeros << (2*k));
		auto update_function = [](size_t& num) { ++num; };
		uint_64 kmer = 0; //临时存放二进制的kmer
		int arry_flag = 0; //判断是否数组使用情况
		char** fna; //存放reads的二维数组(模拟)
		if (!kmer_address_list->try_dequeue(fna)) //1:item非空
		{
			fna = mularr_new(fna_array_max, k + 1);
		}
		while ((array_num < array_max) && (block_address[array_num].mt != 0)) //小于块上限且非空
		{
			for (size_t i = 0; i < block_address[array_num].mt; i++)
			{
				//tmp为kmer的二进制流
				kmer = (block_address[array_num].creads & ones).to_ullong();
				block_address[array_num].creads = block_address[array_num].creads >> 2;
				//cout << block_address[array_num].creads << endl;
				if (!(hash_table_1->upsertIfPresent(kmer,1) || hash_table_2->update_fn(kmer, update_function)))
				{
					// cout_one++;
					//print_list->enqueue(bin2str(kmer));
					if (arry_flag > (fna_array_max - 1)) //判断数组是否已满
					{
						print_list->enqueue(fna); //将数组地址存入线程池，并重置数组
						if (!kmer_address_list->try_dequeue(fna)) //1:item非空
						{
							fna = mularr_new(fna_array_max, k + 1);
							//sum++;
						}
						arry_flag = 0; //重置为0
					}
					bin2str(fna, arry_flag,kmer);
					arry_flag++;
				}
				else
				{
					// cout_divide++;
				}
			}
			// 清空creads及mt计数器
			block_address[array_num].creads.reset();
			block_address[array_num].mt = 0;
			array_num++;
		}
		print_list->enqueue(fna);//存入kmer的块
		address_array->enqueue(block_address);//回收c_reads的块
	};

	void bin2str(char** fna, int& arry_flag,uint_64& kmer)
	{
		for (int i = k-1; i >= 0; i--)
		{
			//cout << "i:" << i << endl;
			int tmp = get_end_n_bit(kmer, 2);
			//cout << "tmp:" <<  tmp << endl;
			kmer >>= 2;
			switch (tmp)
			{
			case 0b00:
				//cout << "before:" << fna[arry_flag][i] << endl;
				fna[arry_flag][i] = 'A';
				//cout << "after:" << fna[arry_flag][i] << endl;
				//str = "A" + str;
				break;
			case 0b01:
				//cout << "before:" << fna[arry_flag][i] << endl;
				fna[arry_flag][i] = 'C';
				//cout << "after:" << fna[arry_flag][i] << endl;
				//str = "C" + str;
				break;
			case 0b10:
				//cout <<"before:" <<fna[arry_flag][i] << endl;
				fna[arry_flag][i] = 'G';
				//cout << "after:" << fna[arry_flag][i] << endl;
				//str = "G" + str;
				break;
			case 0b11:
				//cout << "before:" << fna[arry_flag][i] << endl;
				fna[arry_flag][i] = 'T';
				//cout << "after:" << fna[arry_flag][i] << endl;
				//str = "T" + str;
				break;
			}
		}
	}

	// void print_result()
	// {
	// 	cout << "分流后单次：" << cout_one<< endl;
	// 	cout << "分流后多次：" << cout_divide << endl;
	// }

private:
	int array_max;
	int k;
	int fna_array_max;
	HashTable1* hash_table_1;
	cuckoo_hash_map* hash_table_2;
	CQ_kmer_block* print_list;
	CQ_kmer_block* kmer_address_list;
	Concurrent_Queue_c_reads* address_array;
	atomic<uint_64> cout_one;
	atomic<uint_64> cout_divide;
};