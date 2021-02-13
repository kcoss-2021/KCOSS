#include "counter_less_than_14.h"

void counter_less_than_14::count(char** reads_address)
{
	//uint_32 kmers_bit_tmp;
	int arr_num = 0;

	string kmer, reads;

	//int k_flag; //用于判断kmer是否完整
	//uint_32 kmer_tmp; //存放拼接好的kmer
	//uint_32 new_bit; //用于存放刚拿到的bit

	while ((arr_num < array_max) && (strlen(reads_address[arr_num]) != 0))
	{
		/**************************************************************************************************
		reads = string(reads_address[arr_num]);
		for (size_t i = 0,len= reads.length() - (k - 1); i < len; i++)
		{
			kmer = reads.substr(i, k);
			if (!find_N(kmer))
			{
				kmers_bit_tmp = TCGA2int_(kmer);
				root_table[kmers_bit_tmp]++;
				//cout_all++;
			}
		}
		**************************************************************************************************/
		reads = string(reads_address[arr_num]);
		for (size_t i = 0, len = reads.length() - (k - 1); i < len; i++)
		{
			str2bin_return tmp = find_N2(reads.substr(i, k));
			if (!tmp.flag)
			{
				root_table[tmp.bin]++;
				//cout_all++;
			}
		}
		/**************************************************************************************************
		
		k_flag = 0;
		kmer_tmp = 0;
		for (size_t i = 0,len = strlen(reads_address[arr_num]); i < len; i++) //处理一条reads
		{
			switch (reads_address[arr_num][i])
			{
			case 'A':
			case 'a':
				new_bit = 0b00;
				break;
			case 'C':
			case 'c':
				new_bit = 0b01;
				break;
			case 'G':
			case 'g':
				new_bit = 0b10;
				break;
			case 'T':
			case 't':
				new_bit = 0b11;
				break;
			default:
				k_flag = -1;
				break;
			}
			if (k_flag == -1) //遇到奇奇怪怪的字符，重置
			{
				k_flag = 0;
				kmer_tmp = 0;
			}
			else if (k_flag < (k - 1)) //未拼接完整
			{
				kmer_tmp = (kmer_tmp << 2) + new_bit;
				k_flag++;
			}
			else //拼接完整！！！！！！！！！！！！！！！！！
			{
				kmer_tmp = (kmer_tmp << 2) + new_bit;
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				root_table[kmer_tmp]++;
				//cout_all++;
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				kmer_tmp = kmer_tmp & get_end_2k_2;
			}
		}
		**************************************************************************************************/
		//清空该条
		reads_address[arr_num][0] = '\0';
		arr_num++;
	}
	address_array->enqueue(reads_address);
}

inline uint_32 counter_less_than_14::TCGA2int_(const string& str)
{
	uint_32 TCGA2int = 0;
	for (size_t num = 0,len = str.size(); num < len; num++)
	{
		switch (str.at(num))
		{
		case 'A': // A碱基为0b00
		case 'a':
			TCGA2int <<= 2;
			break;
		case 'C': //01
		case 'c':
			TCGA2int = (TCGA2int << 2) + 0b1;
			break;
		case 'G': //10
		case 'g':
			TCGA2int = (TCGA2int << 2) + 0b10;
			break;
		case 'T': //11
		case 't':
			TCGA2int = (TCGA2int << 2) + 0b11;
			break;
		default:
			cout << "reads含有奇怪的字符" << str.at(num) << "." << endl;
			break;
		}
	}
	return TCGA2int;
}

//inline uint_32 counter_less_than_14::TCGA2intV2_(uint_32& prev, char ch)
//{
//	switch (ch)
//	{
//	case'A':
//		prev <<= 2;
//		break;
//	case 'C':
//		prev = (prev << 2) + 0b1;
//		break;
//	case 'G':
//		prev = (prev << 2) + 0b10;
//		break;
//	case 'T':
//		prev = (prev << 2) + 0b11;
//		break;
//	default:
//		cout << "reads含有奇怪的字符" << ch << "." << endl;
//		break;
//	}
//	return prev;
//}

void counter_less_than_14::print()
{
	//fstream print_less_than_14 = fstream("counter_less_than_14.dat", ios::out | ios::binary);
	//int length = (int)pow(4, k);
	//uint_64 sum = 0;
	//for (int i = 0; i < length; i++)
	//{
	//	sum = sum + root_table[i];
	//}
	//cout << "总条数cout " << cout_all << endl;
	//cout << "总条数sum：" << sum << endl;
	//cout << "length" << length << endl;
	//print_less_than_14.write((char*)root_table, (int)pow(4, k) * 4);
	//print_less_than_14.close();

	int print_less_than_14 = open((filename+".data").c_str(), O_RDWR | O_CREAT, 0664);
	const size_t len = (int)pow(4, k) * 4;
	ftruncate(print_less_than_14, len);
	uint_32* mapped = (uint_32*)mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, print_less_than_14, 0);
	memcpy(mapped, root_table, len);
	munmap((void*)mapped, len);
	close(print_less_than_14);

}

