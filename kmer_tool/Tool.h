//
// Created by Administrator on 2021/7/20.
//

#ifndef HISTO_TOOL_H
#define HISTO_TOOL_H
#include "ThreadPool_stable.h"
#include <atomic>
class Tool{
public:
    virtual void count(char* a)=0;
    virtual void print()=0;
    virtual ~Tool(){
        cout<<"Tool End"<<endl;
    }
};

#endif //HISTO_TOOL_H
