#pragma once

#include "Counter.h"
#include "ConcurrentBloomfilter.h"
#include <math.h>
#include "HashTable2.h"

struct std_XXHash2 : std::hash<__uint128_t>
{
    size_t operator()(const __uint128_t& t)
    {
        return XXH64(&t, sizeof(t), 0);
    }
};

typedef libcuckoo::cuckoohash_map<__uint128_t, uint_32, std_XXHash2> cuckoo_hash_map2;
typedef moodycamel::ConcurrentQueue<c_reads> Concurrent_Queue;
typedef moodycamel::ConcurrentQueue<char**> Concurrent_Queue_char;

class count_33to64 : public Counter
{
public:
	count_33to64(ConcurrentBloomfilter* def_bloom_filter, HashTable2* hash_table_1, cuckoo_hash_map2* hash_table_2, Concurrent_Queue* creads_list, Concurrent_Queue* creads_list_addr, Concurrent_Queue_char* address_array)
	{
		bloom_filter = def_bloom_filter;

		this->hash_table_1 = hash_table_1;
		this->hash_table_2 = hash_table_2;

		this->creads_list = creads_list;
		this->creads_list_addr = creads_list_addr;
		this->address_array = address_array;

		k = exe_arg.k;
		array_m = exe_arg.m;
		filename = exe_arg.filename;
        // batch line = nL + k - 1.
        // for 32 < k <= 64,
        // batch line <= 192.
        // 1 + floor((nL + k - 1) / 16) <= 1 + floor(192 / 16) = 13 -> 16 (cache alignment)
        // column = 1 + (int) ((80 + k - 1 + 15) / 16);
        column = 16;
		// column = 1 + (int)ceil((79 + k) / 16.0);
		get_end_2k_2 = ~(~(static_cast<__uint128_t>(0)) << (2 * k - 2));

		//cout_kall = 0;
		//cout_k1 = 0;
		//cout_k2 = 0;
	}
	void count(char** reads_address);

	void print();

private:
	ConcurrentBloomfilter* bloom_filter;
	HashTable2* hash_table_1;//�������
	cuckoo_hash_map2* hash_table_2; //С�������
	Concurrent_Queue* creads_list;
	Concurrent_Queue* creads_list_addr;
	Concurrent_Queue_char* address_array;
	int array_m;
	int column;
	__uint128_t get_end_2k_2;
	//atomic<uint_64> cout_kall;
	//atomic<uint_64> cout_k1;
	//atomic<uint_64> cout_k2;
};