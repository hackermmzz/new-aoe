#include "BroadCast.h"

BroadCast::BroadCast()
{

}

void BroadCast::subscribe(BroadCast::TaskType task)
{
    tasks.push_back(std::move(task));;
}

void BroadCast::broadcast()
{
    for(auto t:tasks){
        if(t)t();
    }
}
