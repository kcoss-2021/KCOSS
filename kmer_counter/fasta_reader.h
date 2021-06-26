#ifndef KMER_COUNTER_FASTA_READER_H
#define KMER_COUNTER_FASTA_READER_H

#include <cstdio>
#include <cstddef>
#include <cerrno>
#include <cassert>
#include <cstring>

#include "function_def.h"

namespace tdq {
    using std::array;

    /**
     * 以迭代器模式读取文件然后填入 block, 主要提供 has_next() 和 next() 两个函数.
     * 本类既不负责产生 block, 也不负责提交 block, 只是 reads 的搬运工.
     */
    class FASTAReader {
    public:
        static const size_t BLOCK_LINE_LEN = 192;
        typedef char *read_type;
        typedef read_type *read_block_type;

    private:
//        typedef char *read_type;
//        typedef read_type *read_block_type;

        const size_t K;
        const char *FILENAME;

        // to be the next line in block.
        read_type _next;
        // _len = strlen(_next)
        size_t _len = 0;

        FILE *fp = nullptr;
        char buf[BLOCK_LINE_LEN]{};

    public:
        FASTAReader(
                const size_t k,
                const char *filename) :
                K(k), FILENAME(filename) {
            assert(k <= 64);
            if ((fp = fopen(FILENAME, "r+")) == nullptr) {
                perror(FILENAME);
                exit(errno);
            }
            _next = new char[BLOCK_LINE_LEN];
        }

        virtual ~FASTAReader() {
            fclose(fp);
            fp = nullptr;
            delete[] _next;
        }

        bool has_next() {
            return !feof(fp) || _len > 0;
        }

        /**
         * 往文件读取若干行 reads.
         * @param _block 预分配的内存空间
         * @param _block_lines 欲读取多少行
         * @return 实际读取多少行
         */
        size_t next(read_block_type &_block, const size_t _block_lines) {
            if (!has_next()) {
                return false;
            }

            size_t _line_cnt = 0;

            while (_line_cnt < _block_lines) {
                if (!fgets(buf, sizeof(buf), fp)) {
                    // end of file

                    // commit next line iff _len >= K.
                    if (_len >= K) {
                        strcpy(_block[_line_cnt++], _next);
                    }

                    memset(_next, 0, BLOCK_LINE_LEN);
                    _len = 0;
//                    print_block_lines(_block, _line_cnt);
                    return _line_cnt;
                }

                if (buf[0] == '>') {
                    // not a valid line

                    // commit next line iff _len >= K.
                    if (_len >= K) {
                        strcpy(_block[_line_cnt++], _next);
                    }

                    memset(_next, 0, BLOCK_LINE_LEN);
                    _len = 0;
                    continue;
                }

                // encountered a valid line.
                rtrim(buf);

                // next line is too short to commit.
                if (_len < K) {
                    strcat(_next, buf);
                    _len += strlen(buf);
                    continue;
                }

                // commit next line iff _len >= K,
                // and give the last k - 1 bytes to next line.
                strcpy(_block[_line_cnt++], _next);
                memset(_next, 0, BLOCK_LINE_LEN);
                strncpy(_next, _block[_line_cnt - 1] + _len - (K - 1), K - 1);
                strcat(_next, buf);
                _len = K - 1 + strlen(buf);
            }

//            print_block_lines(_block, _line_cnt);
            return _line_cnt;
        }
    };

}

#endif //KMER_COUNTER_FASTA_READER_H
