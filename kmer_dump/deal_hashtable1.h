#pragma once
#include <stdlib.h>

typedef moodycamel::ConcurrentQueue<key_value*> Concurrent_Queue_key_value;

struct std_XXHash : std::hash<uint_64>
{

	std_XXHash(const int k) {
		this->k = k;
	}

	size_t operator()(const uint_64& t)
	{
		return XXH64(&t, sizeof(t), 0);
		//return t;
	}

private:
	int k = 0;
	//std::hash<string> _hash;
};

typedef libcuckoo::cuckoohash_map<uint_64, size_t, std_XXHash> cuckoo_hash_map;

class deal_hashtable1
{
public:
	deal_hashtable1(char** def_argv, cuckoo_hash_map* hash_table, Concurrent_Queue_key_value* address_array)
	{
		array_max = atoi(def_argv[2]);
		this->hash_table = hash_table;
		this->address_array = address_array;
		cout_item = 0;
		cout_all2 = 0;
	}

	void hashtable_insert(key_value* block_address)
	{
		int num = 0;
		while ((num< array_max) && (block_address[num].value != 0))
		{
			//if (!hash_table->insert(block_address[num].key, block_address[num].value))
			//{
			//	cout << "插入失败" << endl;
			//}
			//while (!hash_table->insert(block_address[num].key, block_address[num].value))
			//{
			//}

			hash_table->insert(block_address[num].key, block_address[num].value);

			cout_all2.fetch_add(block_address[num].value);
			cout_item++;
			// 清空
			block_address[num].value = 0;
			num++;
		}
		address_array->enqueue(block_address);
	}


	// void print_cout_all()
	// {
		// cout << "哈希表接收存入value和:" << cout_all2 << endl;
		// cout << "哈希表接收存入条数:" << cout_item << endl;
	// }

private:
	cuckoo_hash_map* hash_table;
	int array_max;
	Concurrent_Queue_key_value* address_array;
	atomic<uint_64> cout_item;
	atomic<uint_64> cout_all2;
};
