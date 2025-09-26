#include "soudplaythread.h"

SoudPlayThread::SoudPlayThread()
{

}

void SoudPlayThread::AddSound(queue<string> &sounds)
{
    mutex.lock();
    while(sounds.size()){
        soundQue.push(sounds.front());
        sounds.pop();
    }
    mutex.unlock();
}

void SoudPlayThread::run()
{
    for(;;){
        mutex.lock();
        if(soundQue.size()){
            // 清空音效队列但不播放
            while(soundQue.size()){
                soundQue.pop();
            }
        }
        mutex.unlock();
        
        // 短暂休眠，避免CPU占用过高
        msleep(50);
    }
}
