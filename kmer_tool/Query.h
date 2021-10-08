//
// Created by Administrator on 2021/7/15.
//

#ifndef HISTO_QUERY_H
#define HISTO_QUERY_H
#include <iostream>
#include "Tool.h"
class Query:public Tool{
public:
    Query(moodycamel::ConcurrentQueue<char*>* emptyAddress,int blockSize,string target){
        this->emptyAddress=emptyAddress;
        this->blockSize=blockSize;
        dealTarget(target);
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
                if(key==this->target){
                    cout<<"result frequence : "<<value+1<<endl;
                }
            }

        }
        emptyAddress->enqueue(a);
    }
    void dealTarget(string target){
        this->target=0;
        int length=target.length();
        for(int i=0;i<length;i++){
            this->target = (this->target << 2) +  change(target[i]);
        }
        this->target=0b1100010010101100000011100100100000100100100011001001111101110111;
    }
    uint32_t change(char a){
        switch (a)
        {
            case 'A':
            case 'a':
                return 0b00;
                //new_char = "A";
                break;
            case 'C':
            case 'c':
                return 0b01;
                //new_char = "C";
                break;
            case 'G':
            case 'g':
                return 0b10;
                //new_char = "G";
                break;
            case 'T':
            case 't':
                return 0b11;
                //new_char = "T";
                break;
            default:
                break;
        }
    }
    ~Query(){
        cout<<"Query End"<<endl;
    }
    void print(){
        //cout<<"Query print"<<endl;
    }
private:
    moodycamel::ConcurrentQueue<char*>* emptyAddress;
    int blockSize;
    uint64_t target;
    int flag=0;
};


#endif //HISTO_QUERY_H
