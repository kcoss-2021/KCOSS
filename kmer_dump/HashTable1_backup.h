#ifndef LINUXPROGRAMMING_HASHTABLE_H
#define LINUXPROGRAMMING_HASHTABLE_H

#include <atomic>
#include <cassert>
#include <string>
#include <memory>

//----------------------------------------------
//  The World's Simplest Lock-Free HashTable
//
//  Maps 32-bit integers to 32-bit integers.
//  Uses open addressing with linear probing.
//  You can call SetItem and GetItem from several threads simultaneously.
//  GetItem is wait-free.
//  SetItem is lock-free.
//  You can't assign any value to key = 0.
//  In the m_cells array, key = 0 is reserved to indicate an unused cell.
//  You can't assign value = 0 to any key.
//  value = 0 means the key is unused.
//  The hash table never grows in size.
//  You can't delete individual items from the hash table.
//  You can Clear the hash table, but only at a time when there are no other calls being made from other threads.
//----------------------------------------------

//size_t __hash_fn_1(K k) {
//    return 0;
//}

class HashTable1 {
public:
	typedef uint64_t K;
	typedef size_t V;
	typedef unsigned long long int uint_64;

	struct Entry {
		std::atomic<K> key;
		std::atomic<V> value;
	};

	struct tablestruct
	{
		Entry* table;
		size_t m_arraySize;
	};

private:
	Entry* m_entries;
	size_t m_arraySize;
	std::atomic_size_t m_arrayCount;
	size_t __hash_fn_1(uint_64 key) {
		return XXH64(&key, sizeof(key), 0);
	}
	std::allocator<Entry> __allocator;

public:
	HashTable1(size_t arraySize) {
		// Initialize cells
		assert((arraySize & (arraySize - 1)) == 0);   // Must be a power of 2

		//auto t1 = std::chrono::steady_clock::now();

		m_arraySize = arraySize;
		m_entries = new Entry[arraySize];
		//m_entries = __allocator.allocate(arraySize);

		//auto t2 = std::chrono::steady_clock::now();
		//cout << (chrono::duration_cast<chrono::microseconds>(t2 - t1).count() / 1000000.0) << "秒" << endl;
		//auto t3 = std::chrono::steady_clock::now();

		//for (size_t i = 0; i < arraySize; ++i) {
		//	__allocator.construct(&m_entries[i]);
		//}

		//auto t4 = std::chrono::steady_clock::now();
		//cout << (chrono::duration_cast<chrono::microseconds>(t4 - t3).count() / 1000000.0) << "秒" << endl;
		//for (size_t i = 0; i < (1u<< 15); i++)
		//{
		//	cout << "est: " << m_entries[i].key << "->" << m_entries[i].value << endl;
		//}

	}

	~HashTable1() {
		// Delete cells
		//delete[] m_entries;

		for (size_t i = 0; i < m_arraySize; ++i) {
			__allocator.destroy(m_entries + i);
		}

		__allocator.deallocate(m_entries, m_arraySize);
	}

	static constexpr size_t MAX_REPROBE = 16;

	static size_t reprobe_fn(size_t hash, size_t i) {
		return hash + (i * (i + 1)) / 2;
	}

	bool upsertIfPresent(K key, V value) {
		for (size_t hash = __hash_fn_1(key), reprobe_count = 0; reprobe_count < MAX_REPROBE; reprobe_count++) {
			size_t idx = reprobe_fn(hash, reprobe_count);
			idx &= m_arraySize - 1;

			K prevKey = m_entries[idx].key.load();
			V prevValue = m_entries[idx].value.load();

			if (prevValue != 0) {
				if (prevKey != key) {
					continue;
				}
				V prev = m_entries[idx].value.fetch_add(value);
				if (prev == 0) {
					m_arrayCount++;
				}

				return true;
			}

			return false;
		}

		return false;
	}

	// increase by value
	bool upsert(K key, V value) {
		for (size_t hash = __hash_fn_1(key), reprobe_count = 0; reprobe_count < MAX_REPROBE; reprobe_count++) {
			size_t idx = reprobe_fn(hash, reprobe_count);
			idx &= m_arraySize - 1;

			K prevKey = m_entries[idx].key.load();
			V prevValue = m_entries[idx].value.load();

			if (prevValue != 0) {
				if (prevKey != key) {
					continue;
				}

				V prev = m_entries[idx].value.fetch_add(value);
				if (prev == 0)
				{
					m_arrayCount++;
				}
				return true;
			}

			bool dominance = m_entries[idx].key.compare_exchange_strong(prevKey, key);
			if (!dominance) {
				if (prevKey == key) {
					V prev = m_entries[idx].value.fetch_add(value);
					if (prev == 0)
					{
						m_arrayCount++;
					}
					return true;
				}
				continue;
			}


			V prev = m_entries[idx].value.fetch_add(value);
			if (prev == 0)
			{
				m_arrayCount++;
			}
			return true;
		}

		return false;
	}

	V get(K key) {
		for (size_t hash = __hash_fn_1(key), reprobe_count = 0; reprobe_count < MAX_REPROBE; reprobe_count++) {
			size_t idx = reprobe_fn(hash, reprobe_count);

			idx &= m_arraySize - 1;

			if (m_entries[idx].key.load() == key) {
				return m_entries[idx].value.load();
			}
		}

		return 0;
	}

	size_t size() {
		return m_arrayCount;
	}

	double load_factor() {
		return m_arrayCount * 1.0 / m_arraySize;
	}

	void memory_dump() {
		for (size_t i = 0; i < m_arraySize; ++i) {
			Entry& it = m_entries[i];
			std::cout << "table[" << i << "]: ";
			std::cout << "key = \'" << it.key << "\' ";
			std::cout << "value = \'" << it.value << "\'" << std::endl;
		}
	}

	// 保存到硬盘
	void save()
	{
		//std::fstream hash_table_1_file = std::fstream("hash_table_1.dat", std::ios::out | std::ios::binary);
		int sum = 0;
		int sum_item = 0;
		for (size_t i = 0; i < m_arraySize; i++)
		{
			if (m_entries[i].value != 0)
			{
				//cout << "哈希表写入：" << m_entries[i].key << endl;
				//hash_table_1_file.write((char*)&m_entries[i].key, 8);
				//hash_table_1_file.write((char*)&m_entries[i].value, 4);
				sum = sum + m_entries[i].value;
				sum_item++;
			}
		}
		//hash_table_1_file.close();

		std::cout << "大哈希表value和：" << sum << std::endl;
		std::cout << "大哈希表t条数：" << sum_item << std::endl;
	}

	tablestruct table_struct()
	{
		tablestruct struc;
		struc.table = m_entries;
		struc.m_arraySize = m_arraySize;
		return struc;
	}

};

#endif //LINUXPROGRAMMING_HASHTABLE_H
