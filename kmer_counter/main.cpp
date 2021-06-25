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
	while ((opt = getopt(argc, argv, "k:i:t:m:o:n:d:vh")) != -1)
	{
		switch (opt)
		{
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
			exe_arg.m = 512;
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
			printf(
				"Example:./kcoss -k 32 -i \"../test_file/test_data.fa\" -t 48 -m 360 -o out_file -n 3000000000 -d 268697600\n");
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

	if (arg_count < 5)
	{
		// 帮助信息
		printf("Too few arguments\n");
		printf(
			"Example:./kcoss -k 32 -i \"../test_file/test_data.fa\" -t 48 -m 360 -o out_file -n 3000000000 -d 268697600\n");
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

	const size_t k = exe_arg.k;
	const string path = exe_arg.path;
	const int core_num = exe_arg.core_num;
	const int m = exe_arg.m;
	const string filename = exe_arg.filename;

	if (k > 14 && arg_count < 7)
	{
		printf("Parameters: \n");
		printf("-n <value> is necessary when k > 14.\n");
		printf(
			"-d <value> sizes of hash table(Number of k-mer species,Non-single occurrence)  is necessary when k > 14.\n");
		exit(-1);
	}

	const uint_64 bloomfilter_size = exe_arg.bloomfilter_size;
	const uint_64 hashtable_size = exe_arg.hashtable_size;

	//cout << "k值" << k << "   path" << path << "   核心数量" << core_num << endl;
	//printf("k值:%d  path:\"%s\"  线程数:%d  块大小:%d条 输出文件名:\"%s\"\n", k, path.c_str(), core_num, m, filename.c_str());
	ConcurrentBloomfilter* bloom_filter = nullptr;
	write_thread* t_write = nullptr;
	moodycamel::ConcurrentQueue<c_reads> creads_list;      //creads存储队列 存放非空块
	moodycamel::ConcurrentQueue<c_reads> creads_list_addr; //creads回收队列 存放空块
	moodycamel::ConcurrentQueue<char*> address_array; //用于回收数组
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
		int ans = find_table_size(hashtable_size) - 1;
		HashTable1* hash_table_1 = new HashTable1(
			1u << ans);                                                                   //1u << 28
		cuckoo_hash_map* hash_table_2 = new cuckoo_hash_map(1u << find_table_size(hashtable_size - pow(2, ans)),
			std_XXHash()); //小表可扩容 //1u << 18
		//cout << "hashtable1:" << ans << "  " <<(1u << ans) << endl;
		//cout << "hashtable2:" << find_table_size(hashtable_size - pow(2, ans)) << "  " << (1u << find_table_size(hashtable_size - pow(2, ans))) << endl;
		counter =
			new count_15to32(bloom_filter, hash_table_1, hash_table_2, &creads_list, &creads_list_addr, &address_array);
	}
	else if (33 <= k && k <= 64)
	{
		bloom_filter = new ConcurrentBloomfilter(
			bloomfilter_size / 2ull); // p = 0.005 m/n = 16  n=32    3000000000/2 估计数除2
		t_write = new write_thread(&creads_list, &creads_list_addr);
		int ans = find_table_size(hashtable_size) - 1;
		HashTable2* hash_table_1 = new HashTable2(
			1u << ans);                                                       //1u << 28
		cuckoo_hash_map2* hash_table_2 = new cuckoo_hash_map2(
			1u << find_table_size(hashtable_size - pow(2, ans))); //小表可扩容 //1u << 18

		//cout << "hashtable1:" << ans << "  " << (1u << ans) << endl;
		//cout << "hashtable2:" << find_table_size(hashtable_size - pow(2, ans)) << "  " << (1u << find_table_size(hashtable_size - pow(2, ans))) << endl;
		counter =
			new count_33to64(bloom_filter, hash_table_1, hash_table_2, &creads_list, &creads_list_addr, &address_array);
	}
	else
	{
		throw runtime_error("k value 0<k<64");
	}
	auto* thread_pool = new ThreadPool_stable<char*>([&](char* reads_address)
		{ counter->count(reads_address); },
		core_num);
	/************************************************************************************************************************************************************************************************************/
	// 如果k大于14需要用到creads则新建一片内存空间
	if (15 <= k && k <= 64)
	{
		/*初试化creads内存块*/
		int creads_blocks_num = core_num * 96; //内存块个数
//        int creads_blockSize = 1024; //每个内存块由多少个uint32组成 4096字节/4字节 后期应该弄小一点,到时候调调
		int creads_blockSize = m; // 每个内存块由512个uint32组成
		uint_32* creads_bigBlock = new uint_32[creads_blocks_num * creads_blockSize](); //整一大块连续的内存空间 加()进行初始化

		/*把creads内存块放进队列*/
		for (int i = 0; i < creads_blocks_num; i++)
		{
			creads_list_addr.enqueue(&creads_bigBlock[i * creads_blockSize]);
		}
	}
	/************************************************************************************************************************************************************************************************************/

	/*打开文件 mmap映射*/
	int file = open(path.c_str(), O_RDONLY);
	if (file < 0)
	{
		throw runtime_error("The file does not exist or has no read permission\n");
	}

	long long fileSize = lseek(file, 0, SEEK_END); //总文件大小
	char* mapped = (char*)mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, file, 0);
	if (mapped == MAP_FAILED)
	{
		printf("source data mmap fail\n");
	}
	//printf(mapped);
	close(file);

	/*初试化内存块*/
	int blocks = core_num * 12;
	int blockSize = 4096; // 16k一个块
//    blockSize++; //+1留给'\0'
	char* bigBlock = (char*)malloc(sizeof(char) * blockSize * blocks);

	/*把内存块放进队列*/
	for (int i = 0; i < blocks; i++)
	{
		bigBlock[(i + 1) * blockSize - 1] = '\0';
		address_array.enqueue(&bigBlock[i * blockSize]);
	}

	/*读取序列文件放入内存块并放入线程池*/
	char* tmp = nullptr;
	bool firstRead = true;
	uint64_t cursor = 0;
	while (true)
	{
		if (address_array.try_dequeue(tmp))
		{
			if (cursor + blockSize - 1 - k > fileSize)
			{ //最后一块
				if (firstRead)
				{
					memcpy(tmp, mapped, fileSize);
					thread_pool->executor(tmp);
					break;
				}
				else
				{
					memset(tmp, '\0', blockSize);
					memcpy(tmp, mapped + cursor - k, fileSize - cursor + k - 1);
					//cout << "fileSize - cursor + k : " << fileSize - cursor + k << endl;
					//cout << "tmp:" << tmp << endl;
					thread_pool->executor(tmp);
					break;
				}
			}
			else
			{
				if (firstRead)
				{
					memcpy(tmp, mapped + cursor, blockSize - 1);
					cursor += blockSize - 1;
					firstRead = false;
				}
				else
				{
					memcpy(tmp, mapped + cursor - k, blockSize - 1);
					cursor += blockSize - 1 - k;
				}
				//cout << "tmp:" << tmp << endl;
				thread_pool->executor(tmp);
			}
		}
//        else {
//            cout << " 生产者需要更多内存块 " << endl;
//        }
	}

	/************************************************************************************************************************************************************************************************************/
	delete thread_pool;
	// fclose(fp);

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
