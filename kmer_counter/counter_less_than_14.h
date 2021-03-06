#ifndef counter_less_than_14_H
#define counter_less_than_14_H
#include "Counter.h"

typedef moodycamel::ConcurrentQueue<char*> Concurrent_Queue_char14;

class counter_less_than_14 :public Counter
{
public:
	counter_less_than_14(atomic_uint* def_root_table, Concurrent_Queue_char14* address_array)
	{
		root_table = def_root_table;
		k = exe_arg.k;
		filename = exe_arg.filename;
		get_end_2k_2 = ~(~0u << (2 * k - 2));
		this->address_array = address_array;
		//cout_all = 0;
	}
	void count(char* reads_address);
	void print();

private:
	atomic_uint* root_table;
	int array_max;
	uint_64 get_end_2k_2;
    Concurrent_Queue_char14* address_array;
};
#endif