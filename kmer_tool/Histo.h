//
// Created by Administrator on 2021/7/20.
//

#ifndef HISTO_HISTO_H
#define HISTO_HISTO_H
#include"Tool.h"
class Histo:public Tool{
public:
    Histo(moodycamel::ConcurrentQueue<char*>* emptyAddress,int blockSize, uint32_t low,uint32_t high){
        this->emptyAddress=emptyAddress;
        this->blockSize=blockSize;
        this->low=low;
        this->high=high;
        array=new atomic<uint64_t>[high+1];
    }
    void count(char* a){
        int length = blockSize - 1;
        int number = length / 12;//每个k v大小是12个字符
        uint64_t cursor = 0;
        uint64_t key;
        uint32_t value;
        for (int i = 0; i < number; i++) {
            memcpy(&key, a + cursor, 8);
            cursor += 8;
            memcpy(&value, a + cursor, 4);
            cursor += 4;
            if (key != 0 || value != 0) {//有效的数据
                //cout << "key : " << key << "   value : " << value << endl;
                if(value<=low){
                    array[low]++;
                }else if(value>=high){
                    array[high]++;
                }else{
                    array[value]++;
                }
            }

        }
        emptyAddress->enqueue(a);
    }
    ~Histo(){
        cout<<"Histo End"<<endl;
        delete []array;
    }
    void print(){
        for(uint32_t i=low;i<=high;i++){
            cout<<i<<"    "<<array[i]<<endl;
        }
    }

private:
    moodycamel::ConcurrentQueue<char*>* emptyAddress;
    int blockSize;
    uint32_t low;
    uint32_t high;
    atomic<uint64_t> *array;
};


#endif //HISTO_HISTO_H
