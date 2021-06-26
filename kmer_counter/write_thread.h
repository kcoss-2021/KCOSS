#pragma once

#include <concurrentqueue.h>
#include "definition.h"

using namespace std;

class write_thread {
public:
    write_thread(moodycamel::ConcurrentQueue<c_reads> *creads_list_def,
                 moodycamel::ConcurrentQueue<c_reads> *creads_list_addr_def);

    ~write_thread();

private:
    moodycamel::ConcurrentQueue<c_reads> *creads_list;       //存放重叠群队列
    moodycamel::ConcurrentQueue<c_reads> *creads_list_addr;  //回收重叠群队列
    bool run;
    thread t_write;
    int k;
    int array_m;
    string filename;
    //uint_64 sum;
};

write_thread::write_thread(moodycamel::ConcurrentQueue<c_reads> *creads_list_def,
                           moodycamel::ConcurrentQueue<c_reads> *creads_list_addr_def) {
    run = true;
    creads_list = creads_list_def;
    creads_list_addr = creads_list_addr_def;
    k = exe_arg.k;
    filename = exe_arg.filename;

    t_write = thread([this] {
                         c_reads item; //包含creads的块 处理完后回收
                         int Not_empty;
//                         int file_fd = open((filename + ".creads").c_str(), O_WRONLY | O_CREAT | O_TRUNC, 00666);
                         fstream c_reads_file = std::fstream((filename + ".creads").c_str(), ios::out | ios::binary);//option_1
                         /**************************************************************************/
                         while ((Not_empty = this->creads_list->try_dequeue(item)) || run) {
                             if (Not_empty) //Not_empty非0时将其内容保存到硬盘
                             {
//                                 write(file_fd, item + 1, 4 * item[0]); // 将存储块除块头且有内容部分写盘
//                                 c_reads_file.write((char *) &item[1], 4 * item[0]); // 将存储块除块头且有内容部分写盘 option_1
                                 c_reads_file.write((char *) (item + 1), 4 * item[0]); // 将存储块除块头且有内容部分写盘 option_1

                                 creads_list_addr->enqueue(item);
//                                 cout << " write! " << item[0] << endl;
                             }
                         }
                         /**************************************************************************/
//                         close(file_fd);
                         c_reads_file.close(); //option_1

                     }
    );
}

write_thread::~write_thread() {
    run = false;
    t_write.join();
}
