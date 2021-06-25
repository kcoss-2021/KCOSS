#ifndef definition_H
#define definition_H

////////////////////////
#include <bitset>
///////////////////////
#include <getopt.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<unistd.h>
#include <queue>
#include <string>
#include <stack>
#include <string.h>
#include <atomic>
#include <unistd.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <mutex>
#include <libcuckoo/cuckoohash_map.hh>
#include "concurrentqueue.h"
#include "function_def.h"
#define get_end_n_bit(x,n) ((x) & (~(~0ull << (n)))) //取x的末n位

using namespace std;

typedef unsigned int uint_32;
typedef unsigned long long int uint_64;

struct
{
	int k;
	string path;
	int core_num;
	int m;
	string filename;
	uint_64 bloomfilter_size;
	uint_64 hashtable_size;
}exe_arg;

struct root_hash_node_14
{
	atomic<uint_32> node;
};

/****************************************************************************************/
//struct c_reads
//{
//	vector<uint_32> cell;
//	int end_point = 0; // 记录unsinged int中的空间剩余
//};
typedef uint_32* c_reads;

/****************************************************************************************/
struct str2bin_return
{
	uint_64 bin;
	int flag;
};

/***************************************************************************************/
//大于14情况下根表的定义
struct root_hash_table
{
	uint_64 H = 0; // 8字节 64位 vs中 unsigned long int 只有4字节
	uint_64 L = 0; // 8字节 64位
	mutex root_mutex;
};

//二级哈希表的定义
struct secondary_hash_table
{
	unsigned long long int s_hash_table[16] = { 0 };
	mutex second_mutex[16];
};

/******************************************************************************************/
struct node_15_28_32bit
{
	uint_32 node_kmer;
	uint_64 node_counter = 0;
};

typedef struct hash_node_15_28_32bit_type
{
	node_15_28_32bit node[16];
	struct hash_node_15_28_32bit_type* pointer;
}hash_node_15_28_32bit;

typedef struct Find_15_28_return_list
{
	stack<hash_node_15_28_32bit*> node_pointer_stack; //用于存放节点的指针
	int aim_num = -1; //命中元素所在位置,未命中时为-1
	int end_p = 17; //最后一次检查的位置
}Find_15_28_return, check_reshape_15_28;
/******************************************************************************************/
#endif