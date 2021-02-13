#ifndef LINUXPROGRAMMING_HASHTABLE_H2
#define LINUXPROGRAMMING_HASHTABLE_H2

#include <atomic>
#include <cassert>
#include <string>
#include <memory>

class HashTable2 {
public:
	typedef __uint128_t K;
	typedef uint_32 V;

	struct Entry {
		K key = NULL_KEY;
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
	//std::atomic_size_t m_arrayCount;

	// 零键被视为空键，故另外安排一个零键的键值对.
	// 即 entries[m_arraySize] 固定放零键值对.
	static constexpr K NULL_KEY = ((K)0);
	static constexpr K NULL_KEY_FOR_ZERO_ENTRY = ~((K)0);


	size_t __hash_fn_1(K key) {
		return XXH64(&key, sizeof(key), 0);
	}
	std::allocator<Entry> __allocator;

public:
	HashTable2(size_t arraySize) {
		//assert(arraySize >= 1 && (arraySize & (arraySize - 1)) == 0);   // Must be a power of 2
		m_arraySize = arraySize;
		m_entries = new Entry[arraySize + 1];
		m_entries[m_arraySize].key = NULL_KEY_FOR_ZERO_ENTRY;
	}

	~HashTable2() {
		// Delete cells
		//delete[] m_entries;

		for (size_t i = 0; i < m_arraySize + 1; ++i) {
			__allocator.destroy(m_entries + i);
		}

		__allocator.deallocate(m_entries, m_arraySize);
	}

	static constexpr size_t MAX_REPROBE = 16;

	static size_t reprobe_fn(size_t hash, size_t i) {
		return hash + (i * (i + 1)) / 2;
	}

	bool upsertIfPresent(K key, V value) {
		if (key == 0) {
			Entry& zero_entry = m_entries[m_arraySize];
			K prevKey = NULL_KEY_FOR_ZERO_ENTRY;

			// To determine if the entry is occupied and leave the entry unchanged.

			// bool dominance = zero_entry.key.compare_exchange_strong(prevKey, NULL_KEY_FOR_ZERO_ENTRY);
			bool dominance = __sync_bool_compare_and_swap(&zero_entry.key, prevKey, NULL_KEY_FOR_ZERO_ENTRY);
			if (!dominance) {
				if (zero_entry.key == key) {
					zero_entry.value.fetch_add(value);

					//V prev = zero_entry.value.fetch_add(value);
					//if (prev == 0)
					//{
					//	m_arrayCount++;
					//}
					return true;
				}

				// for somehow the prevKey != key
				// it should not happen in zero_entry
				return false;
			}

			// not present (CAS succeed)
			return false;
		}

		for (size_t hash = __hash_fn_1(key), reprobe_count = 0; reprobe_count < MAX_REPROBE; reprobe_count++) {
			size_t idx = reprobe_fn(hash, reprobe_count);
			idx &= m_arraySize - 1;

			// idx < m_arraySize, 绝不会跟零键值对冲突

			K prevKey = NULL_KEY;

			// To determine if the entry is occupied and leave the entry unchanged.
			// bool dominance = m_entries[idx].key.compare_exchange_strong(prevKey, NULL_KEY);
			bool dominance = __sync_bool_compare_and_swap(&m_entries[idx].key, prevKey, NULL_KEY);
			if (!dominance) {
				if (m_entries[idx].key == key) {
					m_entries[idx].value.fetch_add(value);
					//V prev = m_entries[idx].value.fetch_add(value);
					//if (prev == 0)
					//{
					//	m_arrayCount++;
					//}
					return true;
				}
				continue;
			}

			return false;
		}

		return false;
	}

	// increase by value
	bool upsert(K key, V value) {
		if (key == NULL_KEY) {
			Entry& zero_entry = m_entries[m_arraySize];
			K prevKey = NULL_KEY_FOR_ZERO_ENTRY;

			// bool dominance = zero_entry.key.compare_exchange_strong(prevKey, key);
			bool dominance = __sync_bool_compare_and_swap(&zero_entry.key, prevKey, key);
			if (!dominance) {
				if (zero_entry.key == key) {
					zero_entry.value.fetch_add(value);
					//V prev = zero_entry.value.fetch_add(value);
					//if (prev == 0)
					//{
					//	m_arrayCount++;
					//}
					return true;
				}

				// For somehow the prevKey != key after a CAS operation.
				// It should not happen in zero_entry.
				return false;
			}

			//printf("inserted %s -> %s.\n", num2tcga_128(key, 33).c_str(), num2tcga_128(zero_entry.key, 33).c_str());
			// not present (CAS succeed)]
			zero_entry.value.fetch_add(value);
			//V prev = zero_entry.value.fetch_add(value);
			//if (prev == 0)
			//{
			//	m_arrayCount++;
			//}
			return true;
		}

		for (size_t hash = __hash_fn_1(key), reprobe_count = 0; reprobe_count < MAX_REPROBE; reprobe_count++) {
			size_t idx = reprobe_fn(hash, reprobe_count);
			idx &= m_arraySize - 1;

			// idx < m_arraySize, 绝不会跟零键值对冲突

			K prevKey = NULL_KEY;

			// bool dominance = m_entries[idx].key.compare_exchange_strong(prevKey, key);
			bool dominance = __sync_bool_compare_and_swap(&m_entries[idx].key, prevKey, key);
			if (!dominance) {
				if (m_entries[idx].key == key) {
					m_entries[idx].value.fetch_add(value);
					//V prev = m_entries[idx].value.fetch_add(value);
					//if (prev == 0)
					//{
					//	m_arrayCount++;
					//}
					return true;
				}
				continue;
			}

			//printf("inserted %s -> %s.\n", num2tcga_128(key, 33).c_str(), num2tcga_128(m_entries[idx].key, 33).c_str());

			m_entries[idx].value.fetch_add(value);
			//V prev = m_entries[idx].value.fetch_add(value);
			//if (prev == 0)
			//{
			//	m_arrayCount++;
			//}
			return true;
		}

		return false;
	}

	V get(K key) {
		if (key == NULL_KEY) {
			return m_entries[0].value.load();
		}

		for (size_t hash = __hash_fn_1(key), reprobe_count = 0; reprobe_count < MAX_REPROBE; reprobe_count++) {
			size_t idx = reprobe_fn(hash, reprobe_count);

			idx &= m_arraySize - 1;

			if (m_entries[idx].key == key) {
				return m_entries[idx].value.load();
			}
		}

		return 0;
	}

	size_t size() {

		size_t count = 0;
		for (size_t i = 0; i < m_arraySize + 1; i++)
		{
			if (m_entries[i].value != 0)
			{
				count++;
			}
		}

		return count;
	}

	double load_factor() {
		return static_cast<double>(size()) / (m_arraySize + 1);
	}


	// 保存到硬盘
	//void save()
	//{
	//	std::fstream hash_table_1_file = std::fstream("HashTable2.dat", std::ios::out | std::ios::binary);
	//	int sum = 0;
	//	int sum_item = 0;
	//	for (size_t i = 0; i < m_arraySize; i++)
	//	{
	//		if (m_entries[i].value != 0)
	//		{
	//			//cout << "哈希表写入：" << m_entries[i].key << endl;
	//			hash_table_1_file.write((char*)&m_entries[i].key, 8);
	//			hash_table_1_file.write((char*)&m_entries[i].value, 4);
	//			//sum = sum + m_entries[i].value;
	//			//sum_item++;
	//		}
	//	}
	//	hash_table_1_file.close();

	//	//std::cout << "大哈希表value和：" << sum << std::endl;
	//	//std::cout << "大哈希表t条数：" << sum_item << std::endl;
	//}

	void save(string filename, int k)
	{
		int hash_table_1_file = open((filename + ".HT1").c_str(), O_RDWR | O_CREAT, 0664);
		uint_64 byte_len = 0;
		if (k > 32 && k <= 48)
		{
			byte_len = 12ull;
			//cout << "k=33-48" << endl;
		}
		else
		{
			byte_len = 16ull;
			//cout << "!!!!!!!!!!" << endl;
		}
		const uint_64 slot = byte_len / 4ull +1ull;
		const uint_64 len = size() * sizeof(uint_32) * slot;
		ftruncate(hash_table_1_file, len);
		//cout << "mapped~" << endl;
		uint_32*  mapped = (uint_32*)mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, hash_table_1_file, 0);
		//cout << "mapped~" << endl;
		uint_64 num = 0;
		for (size_t i = 0; i < m_arraySize + 1; i++)
		{
			if (m_entries[i].value != 0)
			{
				//cout << num2tcga_128(m_entries[i].key, k) << "	" << m_entries[i].value << endl;
				memcpy(mapped + num * slot, &m_entries[i].key, byte_len);
				//__uint128_t tmp = 0;
				//memcpy(&tmp, mapped + num * 5, byte_len);

				memcpy(mapped + num * slot + (slot-1ull), &m_entries[i].value, 4);
				//uint_32 tmp1 = 0;
				//memcpy(&tmp1, mapped + num * 5+4, 4);
				//cout << num2tcga_128(tmp, k) << "	" << tmp1 << endl;

				num++;
			}
		}
		munmap((void*)mapped, len);
		close(hash_table_1_file);
		//cout << "大表写完" << endl;
	}

	tablestruct table_struct()
	{
		tablestruct struc;
		struc.table = m_entries;
		struc.m_arraySize = m_arraySize + 1;
		return struc;
	}

	string num2tcga_128(__uint128_t kmer, int num)
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
			kmer >>= 2;
		}
		return k_str;
	}

};

#endif //LINUXPROGRAMMING_HASHTABLE_H
