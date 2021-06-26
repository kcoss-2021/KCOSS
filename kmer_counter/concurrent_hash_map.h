#ifndef KMER_COUNTER_CONCURRENT_HASH_MAP_H
#define KMER_COUNTER_CONCURRENT_HASH_MAP_H

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <atomic>
#include <cassert>
#include <string>
#include <memory>

#include <xxh3.h>
#include <libcuckoo/cuckoohash_map.hh>

template<typename T>
struct Hash {
    inline virtual size_t operator()(const T &t) = 0;
};

template<typename T>
struct XXHash : Hash<T> {
    inline size_t operator()(const T &t) override {
        return XXH64(&t, sizeof(t), 0);
    };
};

template<typename T>
struct std_XXHash_for_ht : std::hash<T> {
    std::size_t operator()(const T &t) {
        return XXH64(&t, sizeof(t), 0);
    }
};

struct Probing {
    inline virtual size_t operator()(size_t hash_val, size_t reprobe_time) = 0;
};

struct LinearProbing : Probing {
    inline size_t operator()(size_t hash_val, size_t reprobe_time) override {
        return hash_val + reprobe_time;
    }
};

struct QuadraticProbing : Probing {
    inline size_t operator()(size_t hash_val, size_t reprobe_time) override {
        return hash_val + (reprobe_time * (reprobe_time + 1)) / 2;
    }
};

template<typename K, typename V>
struct LogicalEntry {
    K &k;
    V &v;

    LogicalEntry(K &k, V &v) : k(k), v(v) {}
};


/**
 * Fixed size, open addressing, lock-free parallel hash map.
 * Note: __sync builtins are used instead of newer __atomic builtins
 * because __atomic builtins for 16-byte values are missing for somehow.
 * @tparam K key type
 * @tparam V value type
 * @tparam Hash hash function
 * @tparam Probing probing function
 */
template<typename K, typename V, typename Hash = XXHash<K>, typename Probing = QuadraticProbing>
class FixedSizeHashMap {
public:
    typedef K key_t;
    typedef V value_t;

    struct Entry {
        key_t key;
        value_t value;
    };

private:

    Entry *entries;
    Entry *zero_key_entry;
    Hash hash_fn;
    Probing probing_fn;

    const size_t CAPACITY;
    const size_t MAX_REPROBE;

    static constexpr key_t NULL_KEY = ((key_t) 0);
public:
    explicit FixedSizeHashMap(const size_t capacity, const size_t max_reprobe = 16) :
            CAPACITY(capacity),
            MAX_REPROBE(max_reprobe) {
        assert(std::is_integral<key_t>());
        assert(std::is_integral<value_t>());

        entries = new Entry[capacity + 1];
        zero_key_entry = &entries[capacity];
    }

    virtual ~FixedSizeHashMap() {
        zero_key_entry = nullptr;
        delete[] entries;
    }

    // add v to the counter of k.
    bool upsert(const key_t &k, const value_t &v) {
        if (k == NULL_KEY) {
            // the zero key entry is exclusive.
            // no need to update the key slot at all.
            __sync_fetch_and_add(&(zero_key_entry->value), v);
            return true;
        }

        // upsert non-zero key in an open addressing manner.
        for (size_t hash_val = hash_fn(k), i = 0; i < MAX_REPROBE; ++i) {
            size_t idx = probing_fn(hash_val, i) % CAPACITY;
            Entry &entry = entries[idx];
            if (!__sync_bool_compare_and_swap(&(entry.key), NULL_KEY, k) && entry.key != k) {
                continue;
            }
            __sync_fetch_and_add(&(entry.value), v);
            return true;
        }

        return false;
    }

    // add v to the counter of k when k exists.
    bool upsertIfPresent(const key_t &k, const value_t &v) {
        if (k == NULL_KEY) {
            // the zero key entry is exclusive.
            // only need to verify the value field.
            // check if v == 0 with a CAS.
            if (__sync_val_compare_and_swap(&(zero_key_entry->value), 0, 0) == 0) {
                return false;
            }

            __sync_fetch_and_add(&(zero_key_entry->value), v);
            return true;
        }

        key_t prevKey;

        // upsert non-zero key in an open addressing manner.
        for (size_t hash_val = hash_fn(k), i = 0; i < MAX_REPROBE; ++i) {
            size_t idx = probing_fn(hash_val, i) % CAPACITY;
            Entry &entry = entries[idx];
            prevKey = __sync_val_compare_and_swap(&(entry.key), NULL_KEY, NULL_KEY);

            if (prevKey == NULL_KEY) {
                // a null entry means the element is not inserted in the entries afterwards
                return false;
            }

            if (prevKey != k) {
                continue;
            }

            __sync_fetch_and_add(&(entry.value), v);
            return true;
        }

        return false;
    }

    size_t size() {
        size_t count = 0;
        for (size_t i = 0; i < CAPACITY + 1; i++) {
            if (entries[i].value != 0) {
                count++;
            }
        }
        return count;
    }

    double load_factor() {
        return static_cast<double>(size() / (CAPACITY + 1));
    }

    void dump(const string &file_name) {
        int fd = open((file_name + ".HT1").c_str(), O_RDWR | O_CREAT, 0664);
        size_t len = size() * 12;
        int ret = ftruncate(fd, len); // NOLINT

        auto *mapped = (uint32_t *) mmap(nullptr, len, PROT_WRITE, MAP_SHARED, fd, 0);
        size_t offset = 0;
        for (size_t i = 0; i < CAPACITY + 1; ++i) {
            if (entries[i].value > 0) {
                memcpy(mapped + offset * 3, &entries[i].key, 8);
                memcpy(mapped + offset * 3 + 2, &entries[i].value, 4);
                offset++;
            }
        }
        munmap((void *) mapped, len);
        close(fd);
    }

};


template<typename K, typename V>
struct CompactEntry {
    K key[(sizeof(K) / sizeof(V)) / 2];
    V value[sizeof(K) / sizeof(V)];
};

template<>
struct CompactEntry<uint64_t, uint16_t> {
    uint64_t key[2];
    uint16_t value[4];
};

template<>
struct CompactEntry<__uint128_t, uint16_t> {
    __uint128_t key[4];
    uint16_t value[8];
};

/**
 * Fixed size, open addressing, lock-free parallel hash map with the compact entry structure.
 * Note that for every logical entry of idx % 2 == 1, the key must be inverted.
 * @tparam K key type
 * @tparam V value type
 * @tparam Hash hash function
 * @tparam Probing probing function
 */
template<typename K, typename V, typename Hash = XXHash<K>, typename Probing = QuadraticProbing>
class CompactFixedSizeHashMap {
public:
    typedef K key_t;
    typedef V value_t;
    typedef CompactEntry<key_t, value_t> entry_t;
private:
    Hash hash_fn;
    Probing probing_fn;
    entry_t *entries;
    entry_t *zero_key_entry;

    const size_t VALUE_BASE = sizeof(K) / sizeof(V);
    const size_t KEY_BASE = VALUE_BASE / 2;
    const size_t CAPACITY;
    const size_t MAX_REPROBE;
    const key_t KEY_MASK;

    static constexpr key_t NULL_KEY = ((key_t) 0);

    // value_base = 8, key_base = value_base / 2;
    // idx = 0 -> <k0_0, v0_0>
    // idx = 1 -> <k0_0, v0_1>
    // idx = 2 -> <k0_1, v0_2>
    // idx = 3 -> <k0_1, v0_3>
    // idx = 4 -> <k0_2, v0_4>
    // idx = 5 -> <k0_2, v0_5>
    // idx = 6 -> <k0_3, v0_6>
    // idx = 7 -> <k0_3, v0_7>
    // ...
    // idx = i -> <k{i/4}_{(i%4)/2}, v{i/4}_{i%4}>
    //
    inline key_t &_get_key(size_t idx) const {
        return entries[idx / VALUE_BASE].key[(idx % VALUE_BASE) / 2];
    }

    inline value_t &_get_value(size_t idx) const {
        return entries[idx / VALUE_BASE].value[idx % VALUE_BASE];
    }

public:
    explicit CompactFixedSizeHashMap(const size_t capacity, const size_t key_bits, const size_t max_reprobe = 16) :
            CAPACITY(capacity),
            KEY_MASK(key_bits == (sizeof(key_t) * 8) ? (~((key_t) 0)) : ((((key_t) 1) << key_bits) - 1)),
            MAX_REPROBE(max_reprobe) {
        assert(sizeof(key_t) % sizeof(value_t) == 0 && VALUE_BASE >= 2);
        assert((CAPACITY & (CAPACITY - 1)) == 0);
        entries = new CompactEntry<key_t, value_t>[(CAPACITY + VALUE_BASE) / VALUE_BASE];
        zero_key_entry = &entries[(CAPACITY + VALUE_BASE) / VALUE_BASE - 1];
    }

    virtual ~CompactFixedSizeHashMap() {
        zero_key_entry = nullptr;
        delete[] entries;
    }

    // add v to the counter of k.
    bool upsert(const key_t &k, const value_t &v) {
        key_t transformed = std::min(k, ~k & KEY_MASK);
        bool inverted = (k != transformed);

        if (transformed == NULL_KEY) {
            key_t &key = zero_key_entry->key[0];
            value_t &value = zero_key_entry->value[(inverted ? 1 : 0)];

            // the zero-key entry is exclusive.
            // no need to update the key slot at all.
            __sync_fetch_and_add(&value, v);
            return true;
        }

        // upsert non-zero key in an open addressing manner.
        for (size_t hash_val = hash_fn(transformed), i = 0; i < MAX_REPROBE; ++i) {
            size_t idx = probing_fn(hash_val, i) % CAPACITY;
            idx = (idx & (~((size_t) 1))) + (inverted ? 1 : 0);

            key_t &key = _get_key(idx);
            if (!__sync_bool_compare_and_swap(&key, NULL_KEY, transformed) && key != transformed) {
                continue;
            }
            value_t &value = _get_value(idx);
            __sync_fetch_and_add(&value, v);
            return true;
        }

        return false;
    }

    // add v to the counter of k when k exists.
    bool upsertIfPresent(const key_t &k, const value_t &v) {
        if (k == NULL_KEY) {
            // the zero key entry is exclusive.
            // only need to verify the value field.
            // check if v == 0 with a CAS.
            if (__sync_val_compare_and_swap(&(zero_key_entry->value), 0, 0) == 0) {
                return false;
            }

            __sync_fetch_and_add(&(zero_key_entry->value), v);
            return true;
        }

        key_t prevKey;

        // upsert non-zero key in an open addressing manner.
        for (size_t hash_val = hash_fn(k), i = 0; i < MAX_REPROBE; ++i) {
            size_t idx = probing_fn(hash_val, i) % CAPACITY;
            key_t &key = _get_key(idx);
            prevKey = __sync_val_compare_and_swap(&key, NULL_KEY, NULL_KEY);

            if (prevKey == NULL_KEY) {
                // a null entry means the element is not inserted in the entries afterwards
                return false;
            }

            if (prevKey != k) {
                continue;
            }

            value_t &value = _get_value(idx);
            __sync_fetch_and_add(&value, v);
            return true;
        }

        return false;
    }

    size_t size() {
        size_t count = 0;
        for (size_t i = 0; i < CAPACITY + VALUE_BASE; i++) {
            if (_get_value(i) > 0) {
                count++;
            }
        }
        return count;
    }

    double load_factor() {
        return static_cast<double>(size() / (CAPACITY + VALUE_BASE));
    }

    void dump(const string &file_name) {
        int fd = open((file_name + ".HT1").c_str(), O_RDWR | O_CREAT, 0664);
        size_t len = size() * 12;
        int ret = ftruncate(fd, len); // NOLINT

        void *mapped =  mmap(nullptr, len, PROT_WRITE, MAP_SHARED, fd, 0);
        auto *ptr = (uint32_t *) mapped;
        for (size_t i = 0; i < CAPACITY + VALUE_BASE; ++i) {
            // TODO uint32_t 肯定不能适配所有值类型
            uint32_t v = _get_value(i);
            if (v > 0) {
                key_t k = _get_key(i);
                if (i % 2 == 1) {
                    // inverted
                    k = ~k & KEY_MASK;
                }

                memcpy(ptr, &k, 8);
                memcpy(ptr + 2, &v, 4);
                ptr += 3;
            }
        }
        munmap(mapped, len);
        close(fd);
    }

};


#endif //KMER_COUNTER_CONCURRENT_HASH_MAP_H
