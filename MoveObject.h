#ifndef MOVEOBJECT_H
#define MOVEOBJECT_H

#include <Coordinate.h>

class MoveObject:public Coordinate
{
protected:
    int blood;
    int maxBlood;

    int Angle;
    //规定 从下顺时针分别为0 1 2 3 4 5 6 7

    int state;
    //对于Animal
    /*
     * 0 Idle       闲置状态 遍历人准备攻击或逃跑
     * 1 Roaming    随机移动
     * 2 Fleeing    逃跑
     * 3 Chasing    找到人追人
     * 4 Attacking  攻击中
     */
    
public:
    MoveObject();

    //块、细节坐标转换
    double transDetail( int blockNum ){ return blockNum*BLOCKSIDELENGTH;  }
    int transBlock( double detailNum ){ return (int)detailNum/BLOCKSIDELENGTH; }
};

#endif // MOVEOBJECT_H
