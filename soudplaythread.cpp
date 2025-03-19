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
        if(soundQue.size()){
            mutex.lock();
            while(soundQue.size()){
                auto&s=soundQue.front();
                SoundMap[s]->play();
                soundQue.pop();
            }
            mutex.unlock();
        }
    }
}
