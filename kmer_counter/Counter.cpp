#include "Counter.h"

string Counter::num2tcga(__uint128_t kmer,int num)
{
	string k_str = "";
	for (int i = 0; i < num; i++)
	{
		switch (kmer & 0b11)
		{
		case 0b00:
			k_str = "A" + k_str;
			break;
		case 0b01:
			k_str = "C" + k_str;
			break;
		case 0b10:
			k_str = "G" + k_str;
			break;
		case 0b11:
			k_str = "T" + k_str;
			break;
		}
		kmer >>=  2;
	}
	return k_str;
}