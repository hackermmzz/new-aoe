#ifndef USRAI_H
#define USRAI_H
#include "AI.h"
extern tagGame tagUsrGame;
extern ins UsrIns;
/*##########请勿修改以上部分##########*/

//#include "Resource.h"

class UsrAI:public AI
{
public:
    UsrAI(){this->id=0;}
    ~UsrAI(){}

    int getdis(int x,int y,int xx,int yy);
    bool issignaltree(int i);
    bool isbush(int i);
    bool homevale(int x,int y,int a,int b);
    int getbestx(int x);
    int getbesty(int x);
    void getbush(int i);
    int getbestzb(int l,int x,int y);
    void get_ck_tree(int i);
    bool nobuildfinish(int i);
    void getstone_ck(int i);
    void getstone_far(int i);
    bool nomaxblood(int i);
    void help(int i);

private:
    /***************AI通信函数，请勿修改***************/
    void processData() override;
    tagInfo getInfo(){return tagUsrGame.getInfo();}
    int AddToIns(instruction ins) override
    {
        UsrIns.lock.lock();
        ins.id=UsrIns.g_id;
        UsrIns.g_id++;
        UsrIns.instructions.push(ins);
        UsrIns.lock.unlock();
        return ins.id;
    }
    void clearInsRet() override
    {
        tagUsrGame.clearInsRet();
    }
    /***************AI通信函数，请勿修改***************/
};


#endif // USRAI_H
