#ifndef MISSILE_H
#define MISSILE_H

#include <MoveObject.h>

class Missile : public MoveObject
{
public:
    Missile();
    Missile(int type, double beginDR , double beginUR , double goalDR , double goalUR , double damage);

  /**********************虚函数**************************/
    void nextframe();
    void setNowRes(){ }
    void setAttribute();


    int getSort(){ return SORT_MISSILE; }
  /********************以上虚函数**************************/

  /**********************图像资源设置函数**************************/

    static void allocatemissile(int index){ missile[index]=new std::list<ImageResource>; }

    static std::string getMissilename(int index){ return missilename[index];}
    static std::list<ImageResource>* getmissile(int index) {return missile[index];  }

    static void deallocatemissile(int index)
    {
        delete missile[index];
        missile[index] = nullptr;
    }

  /********************以上图像资源设置函数**************************/


private:
    Coordinate* AttackTarget;
    int Sort_attacker;

    bool isAOE;
    double damage;

    std::map<int ,int > lab_SpecialAttack;


    //图像资源相关
    static std::string missilename[1];
    static std::list<ImageResource> *missile[1];



};

#endif // MISSILE_H
