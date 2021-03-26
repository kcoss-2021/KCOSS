#pragma once
#include <string>
#include <atomic>
#include "xxh3.h"
using namespace std;

//typedef unsigned int uint32; //32位
typedef unsigned long long int uint_64; //64位
typedef atomic<uint64_t> _bloom_filter_segment;
const size_t _BLOOM_FILTER_SEGMENT_LEN = sizeof(uint64_t) * 8;

class ConcurrentBloomfilter
{
public:
	ConcurrentBloomfilter(uint_64 bitarray_length);
	~ConcurrentBloomfilter();
	bool try_insert(const string kmer);
	bool try_insert(const uint_64 kmer);
	bool try_insert(const __uint128_t kmer);

private:
	_bloom_filter_segment *bit_array;
	uint_64 bit_array_len;
	bool try_set(const uint_64 hash1, const uint_64 hash2, const uint_64 hash3, const uint_64 hash4);

	//uint_64 TCGA2int(const string& str);//测试直接处理数字对结果是否有影响 xxhash
	//uint64_t hash1(uint64_t key);
};

inline ConcurrentBloomfilter::ConcurrentBloomfilter(uint_64 bitarray_length)
{
	/*初始化生成长度为bitarray_length的uint32数组，共32*bitarray_length位*/
	bit_array_len = bitarray_length;
	bit_array = new _bloom_filter_segment[bitarray_length];
}

inline ConcurrentBloomfilter::~ConcurrentBloomfilter()
{
	delete[] bit_array;
}

inline bool ConcurrentBloomfilter::try_set(const uint_64 hash1, const uint_64 hash2, const uint_64 hash3, const uint_64 hash4)
{
	size_t segment_index = hash1 % bit_array_len;
	size_t i1 = hash2 & (_BLOOM_FILTER_SEGMENT_LEN - 1);
	size_t i2 = hash3 & (_BLOOM_FILTER_SEGMENT_LEN - 1);
	size_t i3 = hash4 & (_BLOOM_FILTER_SEGMENT_LEN - 1);

	uint_64 to_insert = 0;

	to_insert |= (1 << i1);
	to_insert |= (1 << i2);
	to_insert |= (1 << i3);

	if ((to_insert & bit_array[segment_index]) == to_insert)
	{
		return 0;
	}

	uint_64 prev_val = bit_array[segment_index].fetch_or(to_insert);

	// 0 -> 出现过, 1 -> 没出现过
	return (to_insert & prev_val) != to_insert;
}

inline bool ConcurrentBloomfilter::try_insert(const string kmer)
{
	/*
	测试结果：k=32
	字符kmer+xxhash                                      单次kmer数量：2517890268  good
	kmer+kmer +xxhash                                    单次kmer数量：2517904096   excellent
	二进制流+xxhash size*2                                单次kmer数量：2541154024   excellent   结果完美 但是错误
	二进制流+xxhash                                       单次kmer数量：2512742258
	二进制流                                              单次kmer数量：2465652964
	二进制流 +cityhash                                    单次kmer数量：2512735149
	二进制转string +cityhash                              单次kmer数量：2512733259
	二进制string + xxhash                                 单次kmer数量：2512736008
	二进制变128 + xxhash                                  单次kmer数量：2512735933
	二进制 +hash1                                         单次kmer数量：2512726841
	(key << 64) + (~key) +xxhash                          单次kmer数量：2512729575
	(key << 64) + XXH64(&key, sizeof(key), 1);            单次kmer数量：2512724109
	*/
	string kmer1 = kmer + kmer;

	//XXH64_hash_t kmer_hash1 = XXH64(kmer1.data(), kmer1.length(), 0);
	//uint_64 pos = kmer_hash1 % (32 * bit_array_len);

	uint_64 hash1 = XXH64(kmer1.data(), kmer1.length(), 0);
	uint_64 hash2 = XXH64(kmer1.data(), kmer1.length(), 1);
	uint_64 hash3 = XXH64(kmer1.data(), kmer1.length(), 2);
	uint_64 hash4 = XXH64(kmer1.data(), kmer1.length(), 3);

	return try_set(hash1, hash2, hash3, hash4);
}

inline bool ConcurrentBloomfilter::try_insert(const uint_64 key)
{
	//XXH64_hash_t kmer_hash1 = XXH64(&key, 8, 0);
	//uint64 pos = kmer_hash1 % (32 * bit_array_len);

	uint_64 hash1 = XXH64(&key, 8, 0);
	uint_64 hash2 = XXH64(&key, 8, 1);
	uint_64 hash3 = XXH64(&key, 8, 2);
	uint_64 hash4 = XXH64(&key, 8, 3);

	return try_set(hash1, hash2, hash3, hash4);
}

inline bool ConcurrentBloomfilter::try_insert(const __uint128_t key)
{
	uint_64 hash1 = XXH64(&key, 16, 0);
	uint_64 hash2 = XXH64(&key, 16, 1);
	uint_64 hash3 = XXH64(&key, 16, 2);
	uint_64 hash4 = XXH64(&key, 16, 3);

	return try_set(hash1, hash2, hash3, hash4);
}

// /*尝试将pos位置为1，插入前为1返回1，插入前为0返回0*/
// inline bool ConcurrentBloomfilter::try_set(const uint_64 pos)
// {
// 	const uint_64 array_num = pos >> 5; //pos / 32; // 如：63/32 = 1
// 	//int array_pos = ((array_num + 1) * 32 - 1) - pos; // 值域 0~31
// 	const uint_64 aim = 1U << ((pos & 31) +1);
// 	//uint32 prev = bit_array[array_num].load();
// 	/*先进行一个判断存在直接返回1，不存在置为1并返回0*/
// 	//cout << "bit_array[" << array_num << "]:" << bit_array[array_num] << endl;
// 	//cout << "aim:" << aim << endl;
// 	if (bit_array[array_num] & aim)
// 	{
// 		return 1; //已经被置为1
// 	}
// 	//bool succeed = bit_array[array_num].compare_exchange_strong(prev, prev | aim);
// 	//return succeed;
// 	uint_64 prev = bit_array[array_num].fetch_or(aim);
// 	return !((prev ^ bit_array[array_num]) & aim); //插入前仍然为0
// }

//inline uint_64 ConcurrentBloomfilter::TCGA2int(const string& str)
//{
//	uint_64 TCGA2int = 0;
//	for (uint_32 num = 0; num < str.size(); num++)
//	{
//		switch (str.at(num))
//		{
//		case 'A': // A碱基为0b00
//			TCGA2int <<= 2;
//			break;
//		case 'C': //01
//			TCGA2int = (TCGA2int << 2) + 0b1;
//			break;
//		case 'G': //10
//			TCGA2int = (TCGA2int << 2) + 0b10;
//			break;
//		case 'T': //11
//			TCGA2int = (TCGA2int << 2) + 0b11;
//			break;
//		case 'a': // A碱基为0b00
//			TCGA2int <<= 2;
//			break;
//		case 'c': //01
//			TCGA2int = (TCGA2int << 2) + 0b1;
//			break;
//		case 'g': //10
//			TCGA2int = (TCGA2int << 2) + 0b10;
//			break;
//		case 't': //11
//			TCGA2int = (TCGA2int << 2) + 0b11;
//			break;
//		default:
//			//cout << "reads含有奇怪的字符" << (int)str.at(num) << "." << endl;
//			break;
//		}
//	}
//	return TCGA2int;
//}
//
//inline uint64_t ConcurrentBloomfilter::hash1(uint64_t key)
//{
//	key = (~key) + (key << 21); // key = (key << 21) - key - 1;
//
//	key = key ^ (key >> 24);
//
//	key = (key + (key << 3)) + (key << 8); // key * 265
//
//	key = key ^ (key >> 14);
//
//	key = (key + (key << 2)) + (key << 4); // key * 21
//
//	key = key ^ (key >> 28);
//
//	key = key + (key << 31);
//
//	return key;
//}
