#pragma once
#include <stdlib.h>

typedef moodycamel::ConcurrentQueue<key_value2*> Concurrent_Queue_key_value2;

struct std_XXHash2 : std::hash<__uint128_t>
{
    size_t operator()(const __uint128_t& t)
    {
        return XXH64(&t, sizeof(t), 0);
    }
};

typedef libcuckoo::cuckoohash_map<__uint128_t, size_t> cuckoo_hash_map2;

class g_hashtable2
{
public:
	g_hashtable2(char** def_argv, HashTable2* hash_table1, cuckoo_hash_map2* hash_table2, Concurrent_Queue_key_value2* address_array)
	{
		array_max = atoi(def_argv[2]);
		hash_table_1 = hash_table1;
		hash_table_2 = hash_table2;
		this->address_array = address_array;
		cout_item = 0;
		cout_all2 = 0;
	}

	void hashtable_insert(key_value2* block_address)
	{
		int num = 0;
		bool succeed;
		while ((num < array_max) && (block_address[num].value != 0))
		{
			succeed = hash_table_1->upsert(block_address[num].key, block_address[num].value);
			if (!succeed)
			{
				//cout << "溢出！！！！！！！！！" << endl;
				hash_table_2->insert(block_address[num].key, block_address[num].value);
			}
			
			//hash_table_2->insert(block_address[num].key, block_address[num].value);
			
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
	HashTable2* hash_table_1;
	cuckoo_hash_map2* hash_table_2; //小表可扩容
	int array_max;
	Concurrent_Queue_key_value2* address_array;
	atomic<uint_64> cout_item;
	atomic<uint_64> cout_all2;
};
