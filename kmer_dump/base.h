#ifndef base_H
#define base_H


#include <cstdio>
#include <thread>
#include <atomic>
#include <fstream>
#include <iostream>
#include <math.h>
#include "xxh3.h"
#include <bitset>
#include "concurrentqueue.h"
#include <libcuckoo/cuckoohash_map.hh>
#include "HashTable1.h"
#include "HashTable2.h"
#include "ThreadPool_stable.h"
#include <unistd.h>

#define get_end_n_bit(x,n) ((x) & (~(~0ull << (n)))) //ȡx��ĩnλ

using namespace std;

typedef unsigned int uint_32;
typedef unsigned long long int uint_64;

typedef bitset<320> uint_320;

//struct std_XXHash : std::hash<uint_64>
//{
//
//	std_XXHash(const int k) {
//		this->k = k;
//	}
//
//	size_t operator()(const uint_64& t)
//	{
//		return XXH64(&t, sizeof(t), 0);
//		//return t;
//	}
//
//private:
//	int k = 0;
//};
//
//typedef libcuckoo::cuckoohash_map<uint_64, size_t, std_XXHash> cuckoo_hash_map;

struct key_value
{
	uint_64 key = 0;
	uint_32 value = 0;
};

struct key_value2
{
	__uint128_t key = 0;
	uint_32 value = 0;
};

struct key_value_32
{
	uint_32 key = 0;
	uint_32 value = 0;
};

struct char_key_value
{
	char** key; //һά����ģ��
	uint_32* value; //�ɱ䳤����λ��
};

struct c_reads
{
	uint_320 creads;
	uint_32 mt = 0; //�ƶ�����;
};

inline char** mularr_new(int m, int n) //ʹ��һλ����ģ���ά���飬m��n�У�����ռ����� �磺char str[9] = "12345679"; 8���ַ�+'\0' ռ�Ÿ��ռ� ����ʱ��Ϊ��
{
	char** arr = new char* [m]; //������
	int size = m * n;
	arr[0] = new char[size]; //������
	memset(arr[0], 0, sizeof(char)*size);
	for (int i = 1; i < m; i++) //��һλ����ģ��ɶ�ά����
	{
		arr[i] = arr[i - 1] + n;
	}
	return arr;
}
#endif