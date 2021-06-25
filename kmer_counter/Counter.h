#ifndef Counter_H
#define Counter_H
#include "definition.h"

typedef moodycamel::ConcurrentQueue<char*> Concurrent_Queue_char;
typedef moodycamel::ConcurrentQueue<c_reads> Concurrent_Queue_c_reads;

class Counter
{
public:
	int find_N(const string& str); // str含N检测
	str2bin_return find_N2(const string& str); // str含N检测
	virtual void count(char* reads_address) = 0;
//	void release_fna_block(Concurrent_Queue_char* address_array, Concurrent_Queue_c_reads* creads_list_addr);
	virtual void print() = 0;
	uint_64 TCGA2int(const string& str);
	uint_64 TCGA2intV2(uint_64& prev, char ch);
	//uint_32 get_CCounter(const uint_64& root_table_H); //从64位中截取CCounter
	uint_32 get_VCounter(const uint_64& root_table_H); //从64位中截取VCounter
	uint_64 update_counter(const uint_32& flag, uint_64 ccounter, const uint_64& vcounter);
public:
	string num2tcga(__uint128_t kmer, int num);//将uint转换成指定长度的tcga串,len表示转化后的长度

protected:
	int k;
	string filename;
	atomic<uint_32> block_sum;
	int c2k_24;
};

inline int Counter::find_N(const string& str) // str含N检测
{
	/***************
	找到返回1 找不到返回0
	****************/
	/*************************************************************************
	const char* char_str = str.c_str();
	for (size_t i = 0; i < str.size(); i++)
	{
		switch (char_str[i])
		{
		case 'U':
			return 1;
		case 'R':
			return 1;
		case 'Y':
			return 1;
		case 'K':
			return 1;
		case 'M':
			return 1;
		case 'S':
			return 1;
		case 'W':
			return 1;
		case 'B':
			return 1;
		case 'D':
			return 1;
		case 'H':
			return 1;
		case 'V':
			return 1;
		case 'N':
			return 1;
		case '-':
			return 1;
		}
	}
	return 0;
	*************************************************************************/
	const char* char_str = str.c_str();
	for (size_t i = 0; i < str.size(); i++)
	{
		switch (char_str[i])
		{
		case 'A':
			break;
		case 'T':
			break;
		case 'C':
			break;
		case 'G':
			break;
		case 'a':
			break;
		case 't':
			break;
		case 'c':
			break;
		case 'g':
			break;
		default:
			return 1;
			break;
		}
	}
	return 0;
	/*************************************************************************/
}

inline str2bin_return Counter::find_N2(const string& str) // str含N检测
{
	uint_64 TCGA2int = 0;
	str2bin_return str_retrun;
	const char* char_str = str.c_str();
	for (size_t i = 0; i < str.size(); i++)
	{
		switch (char_str[i])
		{
		case 'A': // A碱基为0b00
			TCGA2int <<= 2;
			break;
		case 'C': //01
			TCGA2int = (TCGA2int << 2) + 0b1;
			break;
		case 'G': //10
			TCGA2int = (TCGA2int << 2) + 0b10;
			break;
		case 'T': //11
			TCGA2int = (TCGA2int << 2) + 0b11;
			break;
		case 'a': // A碱基为0b00
			TCGA2int <<= 2;
			break;
		case 'c': //01
			TCGA2int = (TCGA2int << 2) + 0b1;
			break;
		case 'g': //10
			TCGA2int = (TCGA2int << 2) + 0b10;
			break;
		case 't': //11
			TCGA2int = (TCGA2int << 2) + 0b11;
			break;
		default:
			str_retrun.flag = 1;
			return str_retrun;
			break;
		}
	}
	str_retrun.bin = TCGA2int;
	str_retrun.flag = 0;
	return str_retrun;
}

// inline void Counter::release_fna_block(Concurrent_Queue_char* address_array, Concurrent_Queue_c_reads* creads_list_addr)
// {
// 	char** fna; //存放reads的二维数组(模拟)
// 	while (address_array->try_dequeue(fna))
// 	{
// 		delete[] fna[0];
// 		delete[] fna;
// 	}
// 	c_reads item;
// 	while (creads_list_addr->try_dequeue(item))
// 	{
// 		delete[] item[0];
// 		delete[] item;
// 	}
// }


// inline void Counter::count(const string& reads) {}

inline uint_64 Counter::TCGA2int(const string& str)
{
	uint_64 TCGA2int = 0;
	for (uint_32 num = 0; num < str.size(); num++)
	{
		switch (str.at(num))
		{
		case 'A': // A碱基为0b00
			TCGA2int <<= 2;
			break;
		case 'C': //01
			TCGA2int = (TCGA2int << 2) + 0b1;
			break;
		case 'G': //10
			TCGA2int = (TCGA2int << 2) + 0b10;
			break;
		case 'T': //11
			TCGA2int = (TCGA2int << 2) + 0b11;
			break;
		case 'a': // A碱基为0b00
			TCGA2int <<= 2;
			break;
		case 'c': //01
			TCGA2int = (TCGA2int << 2) + 0b1;
			break;
		case 'g': //10
			TCGA2int = (TCGA2int << 2) + 0b10;
			break;
		case 't': //11
			TCGA2int = (TCGA2int << 2) + 0b11;
			break;
		default:
			//cout << "reads含有奇怪的字符" << (int)str.at(num) << "." << endl;
			break;
		}
	}
	return TCGA2int;
}

inline uint_64 Counter::TCGA2intV2(uint_64& prev, char ch)
{
	switch (ch)
	{
	case'A':
		prev <<= 2;
		break;
	case 'C':
		prev = (prev << 2) + 0b1;
		break;
	case 'G':
		prev = (prev << 2) + 0b10;
		break;
	case 'T':
		prev = (prev << 2) + 0b11;
		break;
	case'a':
		prev <<= 2;
		break;
	case 'c':
		prev = (prev << 2) + 0b1;
		break;
	case 'g':
		prev = (prev << 2) + 0b10;
		break;
	case 't':
		prev = (prev << 2) + 0b11;
		break;
	default:
		cout << "reads含有奇怪的字符" << (int)ch << "." << endl;
		break;
	}
	return prev;
}

//inline uint_32 Counter::get_CCounter(const uint_64& root_table_H)
//{
//	return root_table_H << 2u >> 34u;
//}

inline uint_32 Counter::get_VCounter(const uint_64& root_table_H)
{
	return root_table_H & 0b11111111111111111111111111111111; //32个
}

inline uint_64 Counter::update_counter(const uint_32& flag, uint_64 ccounter, const uint_64& vcounter)
{
	uint_64 new_h;
	switch (flag)
	{
	case 10:
		new_h = 9223372036854775808ULL + (ccounter << 32) + vcounter;
		break;
	case 11:
		new_h = 13835058055282163712ULL + (ccounter << 32) + vcounter;
		break;
	}
	return new_h;
}

#endif