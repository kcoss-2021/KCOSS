#include <cstring>
#include "Cmdline.h"
#include "ThreadPool_stable.h"
#include"Query.h"
#include"Histo.h"
#include"File.h"
int main(int argc,char* argv[]) {
    std::cout << "Hello, Avalon!" << std::endl;

    cmdline::parser parameter;
    parameter.add<string>("choice", 'c', "for example:query、histo", true, "");
    parameter.add<string>("target", 't', "for example : ATCG", false, "");
    parameter.add<int>("low", 'l', "low of histo", false, 1);
    parameter.add<int>("high", 'h', "high of histo", false, 10);
    parameter.parse_check(argc, argv);
    string choice=parameter.get<string>("choice");
    string target=parameter.get<string>("target");
    uint32_t low=parameter.get<int>("low");
    uint32_t high=parameter.get<int>("high");

    moodycamel::ConcurrentQueue<char*> emptyAddress; //用于存放空块
    int entry = 12;
    int blocks = 64;
    int blockSize = entry * 1024 + 1;//+1留给'\0'
    char* bigBlock = (char*)malloc(sizeof(char) * blockSize * blocks);
    for (int i = 0; i < blocks; i++) {//把空块放进空队列
        bigBlock[(i + 1) * blockSize - 1] = '\0';
        emptyAddress.enqueue(&bigBlock[i * blockSize]);
    }

    Tool* tool= nullptr;
    if(choice=="query"){
        tool= new Query(&emptyAddress,blockSize,target);
    }
    else if(choice=="histo"){
        tool= new Histo(&emptyAddress,blockSize,low,high);
    }
    else{
        cout<<"please input -c query or -c histo"<<endl;
        exit(1);
    }
    auto* thread_pool = new ThreadPool_stable<char*>([&](char* reads_address) { tool->count(reads_address); }, 1);

    /*读取文件*/
    string fileName1="/home/b8402/21_xiaoweihao/Clion/histo/tmp/tmp.obbjlGTWa7/cmake-build-release/out_file.HT1";
    string fileName2="/home/b8402/21_xiaoweihao/Clion/histo/tmp/tmp.obbjlGTWa7/cmake-build-release/out_file.HT2";
    File* file=new File();
    int file1=file->openFile(fileName1);
    int file2=file->openFile(fileName2);
    uint64_t fileSize1 = lseek(file1, 0, SEEK_END);
    uint64_t fileSize2 = lseek(file2, 0, SEEK_END);
    char* mapped1 = file->mapFile(file1,fileSize1);
    char* mapped2 = file->mapFile(file2,fileSize2);

    /*放入线程池*/
    file->input(emptyAddress,thread_pool,blockSize,mapped1,fileSize1);
    file->input(emptyAddress,thread_pool,blockSize,mapped2,fileSize2);

    delete thread_pool;
    free(bigBlock);
    munmap(mapped1, fileSize1);
    munmap(mapped2, fileSize2);

    tool->print();


    return 0;
}
