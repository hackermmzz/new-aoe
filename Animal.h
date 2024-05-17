#ifndef ANIMAL_H
#define ANIMAL_H

#include <MoveObject.h>
#include <Resource.h>
#include <GlobalVariate.h>
#include <Bloodhaver.h>

class Animal:public MoveObject,public Resource,public BloodHaver
{
private:
    int Friendly=FRIENDLY_NULL;
    //友好度 1为友好 2为敌对 3为中立 0为无

    int state;
    //对于Animal
    /*
     * 0 Idle       闲置状态 遍历人准备攻击或逃跑
     * 1 Roaming    随机移动
     * 2 Fleeing    逃跑
     * 3 Chasing    找到人追人
     * 4 Attacking  攻击中
     */
    
    bool moveAble = true;
    int treeState = 0;

    //以下为图片资源
    static std::list<ImageResource> *Walk[5][8];

    static std::list<ImageResource> *Stand[5][8];

    static std::list<ImageResource> *Attack[5][8];

    static std::list<ImageResource> *Die[5][8];

    static std::list<ImageResource> *Run[5][8];

    static std::string Animalname[5];
    static std::string Animalcarcassname[5];
    static std::string AnimalDisplayName[5];
    
public:
    Animal();
    Animal(int Num,double DR,double UR);
  /**********************虚函数**************************/
    void setPreAttack( ){ this->prestate = MOVEOBJECT_STATE_ATTACK; }
    bool isAttacking(){ return nowstate == MOVEOBJECT_STATE_ATTACK;}
    bool isMonitorObject(Coordinate* judOb){
        int judNum = judOb->getNum(),judSort = judOb->getSort();
        if(Num == ANIMAL_LION) return judSort == SORT_ARMY||judSort == SORT_FARMER || (judSort == SORT_ANIMAL && judNum == ANIMAL_GAZELLE);
        if(Num == ANIMAL_GAZELLE) return judSort == SORT_ARMY||judSort == SORT_FARMER || (judSort == SORT_ANIMAL && judNum == ANIMAL_LION);
        return false;
    }

    void nextframe();
    int getSort();
    void setNowRes();
    double getDis_attack(){ return dis_Attack + (attackObject->getSideLength())/2.0; }

    bool is_attackHit(){ return get_isActionEnd() && attack_OneCircle; }
    /***************指针强制转化****************/
    //若要将Animal类指针转化为父类指针,务必用以下函数!

    void printer_ToResource(void** ptr){ *ptr = dynamic_cast<Resource*>(this); }    //传入ptr为Resource类指针的地址
    void printer_ToBloodHaver(void** ptr){ *ptr = dynamic_cast<BloodHaver*>(this); }    //传入ptr为BloodHaver类指针的地址
    void printer_ToAnimal(void** ptr){ *ptr = this; }
    /*************以上指针强制转化****************/
  /********************以上虚函数**************************/

    static std::string getAnimalName(int index)
    {
        return Animalname[index];
    }
    static std::string getAnimalcarcassname(int index)
    {
        return Animalcarcassname[index];
    }
    static std::string getAnimalDisplayName(int index)
    {
        return AnimalDisplayName[index];
    }
    static std::list<ImageResource>* getRun(int i, int j) {
        return Run[i][j];
    }
    static std::list<ImageResource>* getWalk(int i, int j) {
        return Walk[i][j];
    }
    static std::list<ImageResource>* getStand(int i, int j) {
        return Stand[i][j];
    }
    static std::list<ImageResource>* getAttack(int i, int j) {
        return Attack[i][j];
    }
    static std::list<ImageResource>* getDie(int i, int j) {
        return Die[i][j];
    }
    static void setRun(int i, int j, std::list<ImageResource>* newValue) {
        Run[i][j] = newValue;
    }
    static void setWalk(int i, int j, std::list<ImageResource>* newValue) {
        Walk[i][j] = newValue;
    }
    static void setStand(int i, int j, std::list<ImageResource>* newValue) {
        Stand[i][j] = newValue;
    }
    static void setAttack(int i, int j, std::list<ImageResource>* newValue) {
        Attack[i][j] = newValue;
    }
    static void setDie(int i, int j, std::list<ImageResource>* newValue) {
        Die[i][j] = newValue;
    }
    static void allocateWalk(int i, int j) {
        Walk[i][j] = new std::list<ImageResource>;
    }
    static void allocateStand(int i, int j) {
        Stand[i][j] = new std::list<ImageResource>;
    }
    static void allocateAttack(int i, int j) {
        Attack[i][j] = new std::list<ImageResource>;
    }
    static void allocateDie(int i, int j) {
        Die[i][j] = new std::list<ImageResource>;
    }
    static void allocateRun(int i, int j) {
        Run[i][j] = new std::list<ImageResource>;
    }
    static void deallocateWalk(int i, int j) {
        delete Walk[i][j];
        Walk[i][j] = nullptr;
    }
    static void deallocateStand(int i, int j) {
        delete Stand[i][j];
        Stand[i][j] = nullptr;
    }
    static void deallocateAttack(int i, int j) {
        delete Attack[i][j];
        Attack[i][j] = nullptr;
    }
    static void deallocateDie(int i, int j) {
        delete Die[i][j];
        Die[i][j] = nullptr;
    }
    static void deallocateRun(int i, int j) {
        delete Run[i][j];
        Run[i][j] = nullptr;
    }

    bool isTree(){ return !moveAble;}
    int get_Friendly(){ return Friendly; }
};

#endif // ANIMAL_H
