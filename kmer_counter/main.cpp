#include <iostream>
#include <chrono>
#include "definition.h"
#include "Counter.h"
//#include "count_15_32_N.h"
//#include "count_15_32_test.h"
#include "count_15to32.h"
#include "count_33to64.h" 
#include "counter_less_than_14.h"
//#include "counter_15_28_32bit.h"
//#include "ThreadPool.h"
#include "ThreadPool_stable.h"
#include "ConcurrentBloomfilter.h"
#include "write_thread.h"

using namespace std;

int main(int argc, char* argv[])
{
	// for (size_t i = 0; i < argc; i++)
	// {
	// 	cout << argv[i] << endl;
	// }
	int opt = 0;
	int arg_count = 0;
	while ((opt = getopt(argc, argv, "k:i:t:m:o:n:d:vh")) != -1) {
		switch (opt) {
		case 'k':
			arg_count++;
			exe_arg.k = atoi(optarg);
			break;
		case 'i':
			arg_count++;
			exe_arg.path = optarg;
			break;
		case 't':
			arg_count++;
			exe_arg.core_num = atoi(optarg);
			break;
		case 'm':
			arg_count++;
			exe_arg.m = atoi(optarg);
			break;
		case 'o':
			arg_count++;
			exe_arg.filename = optarg;
			break;
		case 'n':
			arg_count++;
			exe_arg.bloomfilter_size = atoll(optarg); //3000000000
			break;
		case 'd':
			arg_count++;
			exe_arg.hashtable_size = atoll(optarg); //268697600
			break;
		case 'v':
			printf("KCOSS - 1.0.0\n");
			exit(-1);
			break;
		case 'h':
			printf("Example:./kcoss -k 32 -i \"../test_file/test_data.fa\" -t 48 -m 360 -o out_file -n 3000000000 -d 268697600\n");
			printf("Parameters:\n");
			printf("  -k <value>\t\t\tk-mer length (k from 1 to 64)\n");
			printf("  -i <string>\t\t\tinput file path\n");
			printf("  -t <value>\t\t\tnumber of threads\n");
			printf("  -m <value>\t\t\tsizes of block (m >1)\n");
			printf("  -o <string>\t\t\toutput file path\n");
			printf("  -n <value>\t\t\tsizes of bloomfilter(Number of k-mer species)\n");
			printf("  -d <value>\t\t\tsizes of hash table(Number of k-mer species,Non-single occurrence)\n");
			exit(-1);
			break;
		default:
			printf("Unknown command");
			exit(-1);
			break;
		}
	}

	if (arg_count < 5) {
		// 帮助信息
		printf("Too few arguments\n");
		printf("Example:./kcoss -k 32 -i \"../test_file/test_data.fa\" -t 48 -m 360 -o out_file -n 3000000000 -d 268697600\n");
		printf("Parameters:\n");
		printf("  -k <value>\t\t\tk-mer length (k from 1 to 64)\n");
		printf("  -i <string>\t\t\tinput file path\n");
		printf("  -t <value>\t\t\tnumber of threads\n");
		printf("  -m <value>\t\t\tsizes of block (m >1)\n");
		printf("  -o <string>\t\t\toutput file path\n");
		printf("  -n <value>\t\t\tsizes of bloomfilter(Number of k-mer species)\n");
		printf("  -d <value>\t\t\tsizes of hash table(Number of k-mer species,Non-single occurrence)\n");
		exit(-1);
	}

	////auto tp1 = std::chrono::steady_clock::now();
	//const int k = atoi(argv[1]);
	//const string path = argv[2];
	//const int core_num = atoi(argv[3]);
	//const int m = atoi(argv[4]); //定义表大小
	//const string filename = argv[5];

	const int k = exe_arg.k;
	const string path = exe_arg.path;
	const int core_num = exe_arg.core_num;
	const int m = exe_arg.m;
	const string filename = exe_arg.filename;

	if (k > 14 && arg_count <7)
	{
		printf("Parameters: \n");
		printf("-n <value> is necessary when k > 14.\n");
		printf("-d <value> sizes of hash table(Number of k-mer species,Non-single occurrence)  is necessary when k > 14.\n");
		exit(-1);
	}

	const uint_64 bloomfilter_size = exe_arg.bloomfilter_size;
	const uint_64 hashtable_size = exe_arg.hashtable_size;

	//cout << "k值" << k << "   path" << path << "   核心数量" << core_num << endl;
	//printf("k值:%d  path:\"%s\"  线程数:%d  块大小:%d条 输出文件名:\"%s\"\n", k, path.c_str(), core_num, m, filename.c_str());
	ConcurrentBloomfilter* bloom_filter = nullptr;
	write_thread* t_write = nullptr;
	moodycamel::ConcurrentQueue<c_reads> creads_list;
	moodycamel::ConcurrentQueue<c_reads> creads_list_addr;
	moodycamel::ConcurrentQueue<char**> address_array; //用于回收数组
	Counter* counter;

	/***************************************************************************************************/
	// 新方法
	if (0 < k && k <= 14)
	{
		// 存放4^k个数 的数组长度
		atomic_uint* const root_table = new atomic_uint[(int)pow(4, k)];
		counter = new counter_less_than_14(root_table, &address_array);
	}
	else if (15 <= k && k <= 32)
	{
		bloom_filter = new ConcurrentBloomfilter(bloomfilter_size / 2ull); // p = 0.005 m/n = 16  3000000000/2
		t_write = new write_thread(&creads_list, &creads_list_addr);
		int ans = find_table_size(hashtable_size) -1;
		HashTable1* hash_table_1 = new HashTable1(1u << ans); //1u << 28
		cuckoo_hash_map* hash_table_2 = new cuckoo_hash_map(1u << find_table_size(hashtable_size - pow(2, ans)), std_XXHash()); //小表可扩容 //1u << 18
		//cout << "hashtable1:" << ans << "  " <<(1u << ans) << endl;
		//cout << "hashtable2:" << find_table_size(hashtable_size - pow(2, ans)) << "  " << (1u << find_table_size(hashtable_size - pow(2, ans))) << endl;
		counter = new count_15to32(bloom_filter, hash_table_1, hash_table_2, &creads_list, &creads_list_addr, &address_array);
	}
	else if (33 <= k && k <= 64)
	{
		bloom_filter = new ConcurrentBloomfilter(bloomfilter_size / 2ull); // p = 0.005 m/n = 16  n=32    3000000000/2 估计数除2
		t_write = new write_thread(&creads_list, &creads_list_addr);
		int ans = find_table_size(hashtable_size) - 1;
		HashTable2* hash_table_1 = new HashTable2(1u << ans); //1u << 28
		cuckoo_hash_map2* hash_table_2 = new cuckoo_hash_map2(1u << find_table_size(hashtable_size - pow(2, ans))); //小表可扩容 //1u << 18
		//cout << "hashtable1:" << ans << "  " << (1u << ans) << endl;
		//cout << "hashtable2:" << find_table_size(hashtable_size - pow(2, ans)) << "  " << (1u << find_table_size(hashtable_size - pow(2, ans))) << endl;
		counter = new count_33to64(bloom_filter, hash_table_1, hash_table_2, &creads_list, &creads_list_addr, &address_array);
	}
	else
	{
		throw runtime_error("k值范围0<k<64");
	}
	auto* thread_pool = new ThreadPool_stable<char**>([&](char** reads_address) { counter->count(reads_address); }, core_num);
	FILE* fp;
	if ((fp = fopen(path.c_str(), "r+")) == NULL) //判断文件是否存在或可读
	{
		throw runtime_error("文件不存在或者没有读权限\n");
	}
	char s[1024] = { '\0' };
	char str_end[1024] = { '\0' };
	const int str_len = 80 + k; // fna文件默认长度一行80个碱基
	//char* item;
	char** fna; //存放reads的二维数组(模拟)
	int sum = 0;
	int arry_flag = 0; //判断是否数组使用情况
	fna = mularr_char(m, str_len); //初始化数组
	while (fgets(s, 1024, fp))
	{
		if (s[0] == '>')
		{
			str_end[0] = '\0';
		}
		else
		{
			s[strlen(s) - 1] = '\0';
			strcat(str_end, s); //拼接上一段
			if (arry_flag > (m - 1)) //判断数组是否已满
			{
				thread_pool->executor(fna); //将数组地址存入线程池，并重置数组
				if (!address_array.try_dequeue(fna)) //1:item非空
				{
					fna = mularr_char(m, str_len);
				}
				arry_flag = 0; //重置为0
			}
			strcpy(fna[arry_flag], str_end); //将reads放入新空间
			arry_flag++;
			memcpy(str_end, &s[strlen(s) - (k - 1)], k);
		}
	}
	thread_pool->executor(fna); //将最后一个数组地址存入线程池

	delete thread_pool;
	fclose(fp);

	delete bloom_filter;
	//cout << "释放布隆过滤器" << endl;
	//counter->release_fna_block(&address_array,&creads_list_addr);
	counter->print();
	delete t_write;
	//if (bloom_filter != nullptr) {
	//	delete bloom_filter;
	//}
	//auto tp3 = std::chrono::steady_clock::now();
	//printf("总耗时：%lf秒\n", (chrono::duration_cast<chrono::microseconds>(tp3 - tp1).count() / 1000000.0));

	//cout << "size 1 = " << hash_table_1->size() << endl;
	//cout << "load factor 1 = " << hash_table_1->load_factor() << endl;
	//cout << "size 2 = " << hash_table_2->size() << endl;
	//cout << "load factor 2 = " << hash_table_2->load_factor() << endl;

	return 0;
}