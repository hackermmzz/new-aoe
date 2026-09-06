#ifndef ENEMYAI_H
#define ENEMYAI_H

#include "ai.h"
#include <string>

using std::string;

extern tagGame tagEnemyGame;
extern ins EnemyIns;

// 前向声明
class MainWidget;
class Human;

class EnemyAI:public AI
{

public:
    EnemyAI(){this->id=1;}
    ~EnemyAI(){
        ;
    }
private:
    void processData() override;

    tagInfo getInfo(){
        return tagEnemyGame.getInfo();
    }
    void clearInsRet() override{
        tagEnemyGame.clearInsRet();
    }
    virtual ins& GetInsStruct(){
        return EnemyIns;
    }
     void Around();
     void Attack();
     void assignTargetsBasedOnVision();
     int getVisionRange(int unitSort);
     int findBestTargetInVision(int centerX, int centerY, int range, int attackerSort);
     
     // 敌人状态相关方法
     string getEnemyStatus(int unitSN);
     bool isLandUnit(int unitSort);
     bool shouldCooperateAttack(int unitSN);
     
     // 距离计算和目标优化
     double calculateDistance(int x1, int y1, int x2, int y2);
     int findNearestTarget(int attackerX, int attackerY, const vector<int>& targets);
     void Initialize_Enemycenter();
     void Initialize_Enemymap();
     void AssignDefense();
     void AssignFieldSelfDefense();

    public slots:
    void onWaveAttack(int wave);
    void FirstAttack();
    void SecondAttack();
    void ThirdAttack();
    void OrderWaveUnitsToAttackTarget(vector<int>& units, int targetSN);
    tagArmy Threated(tagArmy* army);
};
/*##########DO NOT EDIT ABOVE##########*/



#endif // ENEMYAI_H
