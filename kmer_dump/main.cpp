#include "base.h"
#include "def2.h"
#include "def.h"
#include "write_thread.h"
#include "write_hash_table_less_than_14.h"
#include "write_hash_table_15_32.h"
#include "write_hash_table_33_64.h"

using namespace std;

int main(int argc, char* argv[])
{
	//const int k = atoi(argv[1]);
	//const int array_max = atoi(argv[2]);
	const string filename = argv[1]; //读入的文件名前缀

    fstream input = fstream(filename+".txt", ios::in | ios::binary);
    char charK[5];
    int intK;
    input.read((char*)&intK, 4);
    input.close();
	char* arg[4];
    snprintf(charK, sizeof(charK), "%d", intK);
	arg[1]= charK;
	arg[2]="360";
	arg[3]=argv[1];

    const int k = intK;
    const int array_max=360;

	if (1 <= k && k <= 14)
	{
		fstream input = fstream((filename + ".data").c_str(), ios::in | ios::binary);
		if (!input)
		{
			printf("File open failed\n");
			exit(-1);
		}
		atomic_uint* const root_table = new atomic_uint[(int)pow(4, k)];
		input.read((char*)root_table, (int)pow(4, k) * 4);
		write_hash_table_less_than_14* write_hashtable_less_than_14 = new write_hash_table_less_than_14(arg, root_table);
		write_hashtable_less_than_14->write_table();
		// printf("哈希表开始放入\n");
		write_hashtable_less_than_14->read_table();
		delete write_hashtable_less_than_14;
	}
	else if (15 <= k && k <= 32)
	{
		HashTable1* hash_table_1;
		cuckoo_hash_map* hash_table_2;

		hash_table_1 = new HashTable1(1u << 28); //1u << 28
		hash_table_2 = new cuckoo_hash_map(1u << 18, std_XXHash());

		thread t1(generate_hash_table, arg, hash_table_1, hash_table_2, filename + ".HT1", filename + ".HT2");
		t1.join();
		// cout << "size 1 = " << hash_table_1->size() << endl;
		// cout << "load factor 1 = " << hash_table_1->load_factor() << endl;
		// cout << "size 2 = " << hash_table_2->size() << endl;
		// cout << "load factor 2 = " << hash_table_2->load_factor() << endl;
		CQ_kmer_block print_list; //存放待打印的键值对队列
		CQ_kmer_block kmer_address_list; //回收存kmer的块
		bool run_flag = true; //用于释放空间
		thread t3(deal_creads, arg, hash_table_1, hash_table_2, &print_list, &kmer_address_list, filename + ".creads", &run_flag); //处理序列提取与转换为字符串

		fstream char_key_value_file = fstream("Result.txt", ios::out | ios::binary);
		write_thread* print_key_values = new write_thread(arg, char_key_value_file, &print_list, &kmer_address_list, &run_flag); //同时进行写入
		t3.join();

		delete print_key_values; // 写入结束
		write_hash_table_15_32* write_hashtable_15_32 = new write_hash_table_15_32(arg, hash_table_1, hash_table_2, char_key_value_file);
		write_hashtable_15_32->write_table();
		// printf("哈希表开始放入\n");
		write_hashtable_15_32->read_table();

		delete write_hashtable_15_32;
		// 关闭最终文本文件
		char_key_value_file.close();
	}
	else if (33 <= k && k <= 64)
	{
		HashTable2* hash_table_1;
		cuckoo_hash_map2* hash_table_2;

		hash_table_1 = new HashTable2(1u << 20); //1u << 28
		hash_table_2 = new cuckoo_hash_map2(1u << 18);

		thread t1(generate_hash_table2, arg, hash_table_1, hash_table_2, filename + ".HT1", filename + ".HT2");
		t1.join();
		// cout << "size 1 = " << hash_table_1->size() << endl;
		// cout << "load factor 1 = " << hash_table_1->load_factor() << endl;
		// cout << "size 2 = " << hash_table_2->size() << endl;
		// cout << "load factor 2 = " << hash_table_2->load_factor() << endl;
		CQ_kmer_block print_list; //存放待打印的键值对队列
		CQ_kmer_block kmer_address_list; //回收存kmer的块
		bool run_flag = true; //用于释放空间
		thread t3(deal_creads2, arg, hash_table_1, hash_table_2, &print_list, &kmer_address_list, filename + ".creads", &run_flag); //处理序列提取与转换为字符串

		fstream char_key_value_file = fstream("Result.txt", ios::out | ios::binary);
		// cout << "已开始读取creads，准备写入" << endl;
		write_thread* print_key_values = new write_thread(arg, char_key_value_file, &print_list, &kmer_address_list, &run_flag); //同时进行写入
		t3.join();
		// cout << "Creads读完了" << endl;
		delete print_key_values; // 写入结束

		// cout << "creads写完了" << endl;

		write_hash_table_33_64* write_hashtable_33_64 = new write_hash_table_33_64(arg, hash_table_1, hash_table_2, char_key_value_file);
		write_hashtable_33_64->write_table();
		// printf("哈希表开始放入\n");
		write_hashtable_33_64->read_table();

		delete write_hashtable_33_64;
		// 关闭最终文本文件
		char_key_value_file.close();
	}
	else
	{
		throw runtime_error("K值范围1~64\n");
	}

	printf("ok ！\n");

	return 0;
}


