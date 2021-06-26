#ifndef KMER_COUNTER_CONCURRENT_BLOOM_FILTER_H
#define KMER_COUNTER_CONCURRENT_BLOOM_FILTER_H

#include <algorithm>
#include <atomic>
#include <cstring>
#include <xxh3.h>


namespace kcoss {
    template<typename T>
    struct std_XXHash_for_bf : std::hash<T> {
        std::uint64_t operator()(const T &t) {
            return XXH64(&t, sizeof(t), 0);
        }
    };

    /**
     * A Segmented Bloom Filter that can insert and search elements in a parallel manner.
     * @tparam T element type
     * @tparam Hash hash function type
     */
//    template<typename T, typename Hash = std_XXHash_for_bf<T>>
    class MultipleConcurrentBloomFilter {
    private:
        typedef uint64_t segment_t;
        typedef std::atomic_uint64_t atomic_segment_t;

        atomic_segment_t *segment_arr;
        std_XXHash_for_bf<uint64_t> root_hash_fn_64;
        std_XXHash_for_bf<__uint128_t> root_hash_fn_128;

        static constexpr size_t SEGMENT_BITS = sizeof(segment_t) * 8;
        const size_t GROUPS;
        const size_t SEGMENTS;
        const size_t GROUP_SEGMENTS;
        const size_t HASH_COUNT;
        atomic_size_t group_index{};
//        size_t group_index = 0;
        // (((n_bits + bits - 1) / (bits) + n - 1)) / n * n

    public:
        explicit MultipleConcurrentBloomFilter(size_t n_bits, size_t n_bloom_filter = 1, size_t hash_count = 3) :
                GROUPS(n_bloom_filter),
                SEGMENTS(((n_bits + SEGMENT_BITS - 1) / SEGMENT_BITS + n_bloom_filter - 1) / n_bloom_filter *
                         n_bloom_filter),
                GROUP_SEGMENTS(((n_bits + SEGMENT_BITS - 1) / SEGMENT_BITS + n_bloom_filter - 1) / n_bloom_filter),
                HASH_COUNT(hash_count) {
            assert(n_bloom_filter >= 1);
            segment_arr = new atomic_segment_t[SEGMENTS];
            group_index = 0;
        }

        virtual ~MultipleConcurrentBloomFilter() {
            delete[] segment_arr;
        }

        /**
         * test and insert an element.
         * @param elem the element to insert
         * @return if the element is first-occurrence.
         */
        inline bool test_set_64(const uint64_t &elem) {
            size_t seg_hash = root_hash_fn_64(elem);
            size_t seg_offset = seg_hash % GROUP_SEGMENTS;

            segment_t to_insert = _calc_to_insert64(elem);
            segment_t prev = segment_arr[group_index * GROUP_SEGMENTS + seg_offset].fetch_or(to_insert);
            return (to_insert & prev) != to_insert;
        }

        inline bool test_set_128(const __uint128_t &elem) {
            size_t seg_hash = root_hash_fn_128(elem);
            size_t seg_offset = seg_hash % GROUP_SEGMENTS;

            segment_t to_insert = _calc_to_insert128(elem);
            segment_t prev = segment_arr[group_index * GROUP_SEGMENTS + seg_offset].fetch_or(to_insert);
            return (to_insert & prev) != to_insert;
        }

        void clear() {
            memset(segment_arr, 0, SEGMENTS * sizeof(segment_t));
        }

        void next() {
            size_t prev = group_index++;
            assert(prev + 1 < GROUPS);
        }

        void dump(const string &file_name) {
            // 将装载有所有 k-mer 的布隆过滤器存至文件，以便 dump 程序复用.
            int fd = open((file_name + ".bf").c_str(), O_RDWR | O_CREAT, 0664);

            // 文件内容 = 分段数(64bit) + 分趟数(64bit), 布隆过滤器全部数据(分段数 * 64bit).
            const size_t len = (2 + SEGMENTS) * sizeof(segment_t);
            ftruncate(fd, len); // NOLINT(cppcoreguidelines-narrowing-conversions)

            auto *mapped = (uint64_t *) mmap(nullptr, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            mapped[0] = SEGMENTS;
            mapped[1] = GROUPS;
            memcpy(mapped + 2, segment_arr, SEGMENTS * sizeof(segment_t));

            munmap((void *) mapped, len);
            close(fd);
        }

    private:
        segment_t _calc_to_insert64(const uint64_t &elem) {
            segment_t to_insert = 0;

            for (size_t i = 0, hash_val = root_hash_fn_64(elem), idx = 0; i < HASH_COUNT; ++i) {
                hash_val = root_hash_fn_64(hash_val);
                idx = hash_val % SEGMENT_BITS;
                to_insert |= (((segment_t) 1) << (idx));
            }

            return to_insert;
        }

        segment_t _calc_to_insert128(const __uint128_t &elem) {
            segment_t to_insert = 0;

            for (size_t i = 0, hash_val = root_hash_fn_128(elem), idx = 0; i < HASH_COUNT; ++i) {
                hash_val = root_hash_fn_128(hash_val);
                idx = hash_val % SEGMENT_BITS;
                to_insert |= (((segment_t) 1) << (idx));
            }

            return to_insert;
        }

    };

}

#endif //KMER_COUNTER_CONCURRENT_BLOOM_FILTER_H