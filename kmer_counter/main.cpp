#include <iostream>
#include <chrono>
#include <climits>
#include "definition.h"
#include "Counter.h"
#include "count_15to32.h"
#include "count_15to32_2.h"
#include "count_33to64.h"
#include "counter_less_than_14.h"
//#include "counter_15_28_32bit.h"
//#include "ThreadPool.h"
//#include "ThreadPool_stable.h"
#include "executor.h"
#include "concurrent_bloom_filter.h"
//#include "ConcurrentBloomfilter.h"
#include "write_thread.h"

using namespace std;

inline void print_current_time(const char *label) {
    static const auto zero = std::chrono::steady_clock::now();
    const auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> d = now - zero;
    printf("Time point %s - %.8lfs elpased.\n", label, d.count());
}

int main(int argc, char* argv[])
{
	// for (size_t i = 0; i < argc; i++)
	// {
	// 	cout << argv[i] << endl;
	// }
	int opt = 0;
	int arg_count = 0;
	while ((opt = getopt(argc, argv, "k:i:t:m:o:n:d:x:yvh")) != -1)
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
        case 'x':
            exe_arg.passes = atoll(optarg);
            break;
        case 'y':
            exe_arg.use_compact_hash_map = true;
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
            printf("  -y \t\t\t\tusing compact hash map (maximum frequency should not greater than 65535)\n");
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
        printf("  -y \t\t\t\tusing compact hash map (maximum frequency should not greater than 65535)\n");
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
//	ConcurrentBloomfilter* bloom_filter = nullptr;
//    MultipleConcurrentBloomFilter

    size_t x = exe_arg.passes;

    kcoss::MultipleConcurrentBloomFilter *bloom_filter = nullptr;
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
        bloom_filter = new kcoss::MultipleConcurrentBloomFilter(((bloomfilter_size / 2ull) * 64ull) / x);
//		bloom_filter = new ConcurrentBloomfilter(bloomfilter_size / 2ull); // p = 0.005 m/n = 16  3000000000/2
		t_write = new write_thread(&creads_list, &creads_list_addr);
		uint64_t ans = find_table_size(hashtable_size) - 1;

//        auto *hash_table_1 = new FixedSizeHashMap<uint64_t, uint32_t>(1ull << ans);
//		HashTable1* hash_table_1 = new HashTable1(
//			1u << ans);                                                                   //1u << 28
		auto* hash_table_2 = new cuckoo_hash_map(1ull << find_table_size(hashtable_size - pow(2, ans)),
			std_XXHash()); //小表可扩容 //1u << 18
		//cout << "hashtable1:" << ans << "  " <<(1u << ans) << endl;
		//cout << "hashtable2:" << find_table_size(hashtable_size - pow(2, ans)) << "  " << (1u << find_table_size(hashtable_size - pow(2, ans))) << endl;
//        printf("Using MultipleBloomFilter with x = %lu\n", x);
        if (!exe_arg.use_compact_hash_map) {
//            printf("Using FixedSizeHashMap with size = %llu\n", 1ull << ans);
            auto *hash_table_1 = new FixedSizeHashMap<uint64_t, uint32_t>(1ull << ans);
            counter = new count_15to32(bloom_filter, hash_table_1, hash_table_2, &creads_list, &creads_list_addr, &address_array);
        } else {
//            printf("Using CompactFixedSizeHashMap with size = %llu\n", 1ull << ans);
            auto *hash_table_1 = new CompactFixedSizeHashMap<uint64_t, uint16_t>(1ull << ans, k * 2);
            counter =
                    new count_15to32_2(bloom_filter, hash_table_1, hash_table_2, &creads_list, &creads_list_addr, &address_array);
        }
	}
	else if (33 <= k && k <= 64)
	{
        bloom_filter = new kcoss::MultipleConcurrentBloomFilter(((bloomfilter_size / 2ull) * 64ull) / x);
//		bloom_filter = new ConcurrentBloomfilter(
//			bloomfilter_size / 2ull); // p = 0.005 m/n = 16  n=32    3000000000/2 估计数除2
		t_write = new write_thread(&creads_list, &creads_list_addr);
		uint64_t ans = find_table_size(hashtable_size) - 1;
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
//	auto* thread_pool = new ThreadPool_stable<char*>([&](char* reads_address)
//		{ counter->count(reads_address); },
//		core_num);

    auto *executor = new kcoss::Executor<char*>([&](char* read_address) {counter->count(read_address);}, core_num);

	/************************************************************************************************************************************************************************************************************/
	// 如果k大于14需要用到creads则新建一片内存空间
	if (15 <= k && k <= 64)
	{
		/*初试化creads内存块*/
		int creads_blocks_num = core_num * 96; //内存块个数
		int creads_blockSize = m; // 每个内存块由512个uint32组成 //每个内存块由多少个uint32组成 4096字节/4字节 后期应该弄小一点,到时候调调
		uint_32* creads_bigBlock = new uint_32[creads_blocks_num * creads_blockSize]; //整一大块连续的内存空间 加()进行初始化

		/*把creads内存块放进队列*/
		for (int i = 0; i < creads_blocks_num; i++)
		{
			creads_bigBlock[0] = 0;
//			creads_list_addr.enqueue(&creads_bigBlock[i * creads_blockSize]); //creads_bigBlock + (i * creads_blockSize)
			creads_list_addr.enqueue(creads_bigBlock); //creads_bigBlock + (i * creads_blockSize)
			creads_bigBlock = creads_bigBlock + creads_blockSize;
		}
	}
	/************************************************************************************************************************************************************************************************************/

	/*打开文件 mmap映射*/
	int file = open(path.c_str(), O_RDONLY);
	if (file < 0)
	{
		throw runtime_error("The file does not exist or has no read permission\n");
	}

	size_t file_size = lseek(file, 0, SEEK_END); //总文件大小
	char* mapped = (char*)mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, file, 0);
	if (mapped == MAP_FAILED)
	{
		printf("source data mmap fail\n");
	}
	//printf(mapped);
	close(file);

	/*初试化内存块*/
	size_t blocks = core_num * 12;
	size_t block_size = 4096; // 4K一个块
//    block_size++; //+1留给'\0'
	char* bigBlock = (char*)malloc(sizeof(char) * block_size * blocks);
    char buf[128];

	/*把内存块放进队列*/
	for (int i = 0; i < blocks; i++)
	{
		bigBlock[(i + 1) * block_size - 1] = '\0';
		address_array.enqueue(&bigBlock[i * block_size]);
	}

//    print_current_time("initial");

    /*读取序列文件放入内存块并放入线程池*/
    char *tmp = nullptr;
    // 每个块的最后一位都要置零作为结束位，所以每个块的实际长度需要减 1.
    size_t actual_block_size = block_size - 1;
    size_t offset = 0;

    // 准备分 x 趟所需标志
    // 处理 y 个块后需要进行中断, 对 y 进行适当高估以确保实分趟数不超过 x.
    size_t average_block_offset = actual_block_size - k;
    size_t total_blocks = (file_size + average_block_offset - 1) / average_block_offset;
    size_t y = (total_blocks + x - 1) / x;
    if (x <= 1) {
        y = SIZE_MAX;
    }
    size_t z = 0;
    size_t proceed_blocks = 0;

    // 第一次读
    /**
    if (!address_array.try_dequeue(tmp)) {
        tmp = (char *) malloc(block_size);
        tmp[block_size - 1] = 0;
    }
    **/
    while (!address_array.try_dequeue(tmp));

    if (file_size <= actual_block_size) {
        // 第一个块太小，处理完下面的循环就不进入了
        memcpy(tmp, mapped, file_size);
        offset = file_size;
    } else {
        // 第一个块够大, 复制一个完整块, 同时为下一块预留 k 个字符以便连接.
        // 举例说明. 设 actual_block_size = 10, k = 5, 有某 read 如下：
        // {0  1  2  3  4 [5  6 \n  8 (9} 0  1  2  3] 4  5  6  7  8) 9 ...
        //                 |           |  |        |  |           |  |
        //                 z1          x1 y1       x2 y2          x3 y3
        //                             z2
        // x1: 第一个块最后一位
        // y1: actual_block_size 指向的位
        // z1: actual_block_size - k 指向的位, 即第二块的起始位, 最后一位为 x2,
        // 可见第二个块的首端是第一个块末端的 k 个位.
        // 预留 k 个位是出于避免换行符干扰的目的，下游消费者会根据首端 k 个位有无换行符来进行相应处理.
        // 如果重叠部分有换行符，那么换行符在下游会被忽略，重叠部分实际共享 k - 1 个位.
        // 如果重叠部分无换行符，那么下游会忽略第一个位，依然实际与上一块共享 k - 1 个位.
        // 见 count_15to32.cpp 以及 count_33to64.cpp.
        memcpy(tmp, mapped, actual_block_size);
        offset = actual_block_size - k;
    }
    executor->enqueue(tmp);

    // 分趟中断器
    if ((++proceed_blocks) % y == 0) {
        sprintf(buf, "part #%lu enqueued", z);
        print_current_time(buf);
        executor->join();
//        print_current_time("part consumed");
//            bloom_filter->next();
        bloom_filter->clear();
//        print_current_time("bloom filter reset");
        ++z;

        // 确保分趟情况下布隆过滤器能记录所有元素.
        assert(z < x);
    }

    // 第二个块 - 最后一块 (如果够大, 不够大就进入下面的兜底逻辑)
    while (offset + actual_block_size <= file_size) {
        /**
        if (!address_array.try_dequeue(tmp)) {
            tmp = (char *) malloc(block_size);
            tmp[block_size - 1] = 0;
        }
        **/
        while (!address_array.try_dequeue(tmp));

        memcpy(tmp, mapped + offset, actual_block_size);
        executor->enqueue(tmp);
        offset += actual_block_size - k;

        // 分趟中断器
        if ((++proceed_blocks) % y == 0) {
            sprintf(buf, "part #%lu enqueued", z);
            print_current_time(buf);
            executor->join();
//            print_current_time("part consumed");
//            bloom_filter->next();
            bloom_filter->clear();
//            print_current_time("bloom filter reset");
            ++z;
            assert(z < x);
        }
    }

    // 最后一块，长度小于一个块
    // 长度小过 k 就不要了
    if (offset + k <= file_size) {
        /**
        if (!address_array.try_dequeue(tmp)) {
            tmp = (char *) malloc(block_size);
            tmp[block_size - 1] = 0;
        }
        **/
        while (!address_array.try_dequeue(tmp));

        memset(tmp, 0, block_size);
        memcpy(tmp, mapped + offset, file_size - offset);
        executor->enqueue(tmp);
        offset = file_size;

        // 分趟中断器
        if ((++proceed_blocks) % y == 0) {
            sprintf(buf, "part #%lu enqueued", z);
            print_current_time(buf);
            executor->join();
//            print_current_time("part consumed");
//            bloom_filter->next();
            bloom_filter->clear();
//            print_current_time("bloom filter reset");
            ++z;
        }
    }

    /************************************************************************************************************************************************************************************************************/
//    print_current_time("all enqueued");

    executor->join();
    delete executor;

//    print_current_time("all consumed");

//    printf("应分 %lu 趟, 实分 %lu 趟\n", x, z);
    // fclose(fp);
	//cout << "释放布隆过滤器" << endl;
    //counter->release_fna_block(&address_array,&creads_list_addr);
    counter->print();

//    print_current_time("hashmap written");

    delete t_write;
//    print_current_time("all written");
    delete bloom_filter;
//    print_current_time("all finished");

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
