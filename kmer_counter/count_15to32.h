#pragma once

#include "Counter.h"
#include "ConcurrentBloomfilter.h"
#include <math.h>
#include "HashTable1.h"

struct std_XXHash : std::hash<uint_64> {

	//std_XXHash(const int k) {
	//	this->k = k;
	//}

	std::size_t operator()(const uint_64& t) {
		return XXH64(&t, 8, 0);
	}
//private:
//	int k = 0;
};

typedef libcuckoo::cuckoohash_map<uint_64, uint_32, std_XXHash> cuckoo_hash_map;
typedef moodycamel::ConcurrentQueue<c_reads> Concurrent_Queue;
typedef moodycamel::ConcurrentQueue<char**> Concurrent_Queue_char;

class count_15to32 : public Counter
{
public:
	count_15to32(ConcurrentBloomfilter* def_bloom_filter, HashTable1* hash_table_1, cuckoo_hash_map* hash_table_2, Concurrent_Queue* creads_list, Concurrent_Queue* creads_list_addr, Concurrent_Queue_char* address_array)
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
		// for k <= 32,
		// nL <= 80, k - 1 <= 31.
		// 1 + floor((nL + k - 1) / 16) <= 1 + floor(111 / 16) = 8 -> 8 (byte alignment)
        // column = 1 + (int) ((80 + k - 1 + 15) / 16);
		column = 8;
		get_end_2k_2 = ~(~0ull << (2 * k - 2));

		block_sum = 0;
		//cout_kall = 0;
		//cout_k1 = 0;
		//cout_k2 = 0;
	}
	void count(char** reads_address);
	void print();

private:
	ConcurrentBloomfilter* bloom_filter;
	HashTable1* hash_table_1;//大表不扩容
	cuckoo_hash_map* hash_table_2; //小表可扩容
	Concurrent_Queue* creads_list;
	Concurrent_Queue* creads_list_addr;
	Concurrent_Queue_char* address_array;
	int array_m;
	int column;
	uint_64 get_end_2k_2;
	//atomic<uint_64> cout_kall;
	//atomic<uint_64> cout_k1;
	//atomic<uint_64> cout_k2;
};