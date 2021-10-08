//
// Created by Administrator on 2021/7/15.
//
#include <sys/mman.h>
#include<fcntl.h>
#include<unistd.h>
#ifndef HISTO_FILE_H
#define HISTO_FILE_H
typedef unsigned long long uint_64;
class File{
public:
    int openFile(string fileName){
        int file = open(fileName.c_str(), O_RDONLY);
        if (file < 0) {
            printf("open file fail");
            exit(1);
        }
        return file;
    }

    char* mapFile(int file,uint_64 fileSize){
        char* mapped = (char*)mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, file, 0);
        if (mapped == MAP_FAILED) {
            printf("file mmap fail");
            exit(1);
        }
        close(file);
        return mapped;
    }

    void input(moodycamel::ConcurrentQueue<char*>& emptyAddress,ThreadPool_stable<char*>* thread_pool,int blockSize,char* mapped,uint64_t fileSize){
        char* tmp = nullptr;
        uint64_t cursor = 0;
        while (true){
            if (emptyAddress.try_dequeue(tmp)){
                if (cursor + blockSize - 1 > fileSize) {//最后一块
                    memset(tmp, '\0', blockSize);
                    memcpy(tmp, mapped + cursor, fileSize - cursor);
                    thread_pool->executor(tmp);
                    break;
                }
                else {
                    memcpy(tmp, mapped + cursor, blockSize - 1);
                    cursor += blockSize - 1;
                    thread_pool->executor(tmp);
                }
            }
        }
    }
};


#endif //HISTO_FILE_H
