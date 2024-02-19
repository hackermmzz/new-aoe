#include "Army.h"

Army::Army()
{

}

Army::Army(double DR,double UR,int type)
{
    //type可视为army的state
    this->type = type;
    this->Blood=1;
    setAttribute();
    this->Angle=rand()%8;
    this->DR=DR;
    this->UR=UR;
    this->BlockDR=DR/BLOCKSIDELENGTH;
    this->BlockUR=UR/BLOCKSIDELENGTH;
    this->nextBlockDR=BlockDR;
    this->nextBlockUR=BlockUR;
    this->PredictedDR=DR;
    this->PredictedUR=UR;
    this->PreviousDR=DR;
    this->PreviousUR=UR;
    this->DR0=DR;
    this->UR0=UR;
    this->nowstate=MOVEOBJECT_STATE_STAND;

    setNowRes();
    this->imageX=this->nowres->pix.width()/2.0;
    this->imageY=this->nowres->pix.width()/4.0;
    this->imageH=DR-UR;
    this->globalNum=g_globalNum;
    g_Object.insert({this->globalNum,this});
    g_globalNum++;

}


void Army::nextframe()
{
    qDebug()<<"Army缺少nextframe";


}

void Army::setNowRes()
{
    qDebug()<<"Army缺少setNowRes";

}

/***********************************************************/
//获取军队的各项数据
//移速
double Army::getSpeed()
{
    double moveSpeed;

    if( upgradable ) moveSpeed = speed_change[ playerScience->get_level(getSort(),type) ];
    else moveSpeed = speed;

    return moveSpeed*playerScience->get_rate_Move(getSort(),type);
}

//血量
int Army::getMaxBlood()
{
    int realmBlood;

    if(upgradable) realmBlood = MaxBlood_change[playerScience->get_level(getSort(),type)];
    else realmBlood = MaxBlood;

    return  realmBlood*playerScience->get_rate_Blood(getSort(),type)+playerScience->get_addition_Blood(getSort(),type);
}

//视野
int Army::getVision()
{
    int realVision;
    if(upgradable) realVision = vision_change[playerScience->get_level(getSort(),type)];
    else realVision = vision;

    return realVision;
}

//攻击力
int Army::getATK()
{
    int atkValue;//用于存储初始攻击力

    //赋值初始攻击力,依据兵种是否能升级,划分两类赋值方式
    if(upgradable) atkValue = atk_change[playerScience->get_level(getSort(),type)];
    else atkValue = atk;

    //再atkValue基础上,计算player及科技带来的加成,并返回
    return (int)( atkValue*playerScience->get_rate_Attack(getSort(),type,armyClass,get_AttackType())) + \
            playerScience->get_rate_Attack(getSort(),type,armyClass,get_AttackType()) + get_add_specialAttack();
}

//防御力,分为获取肉搏防御力和投射物防御力
int Army::getDEF(int attackType_got)
{
    int defValue = 0;//用于存储初始防御力

    //赋值defValue;根据attackType_got即收到的伤害类型,选择相应的防御类型:肉盾防御或投射防御.若为祭司转化或(投石车?等)无伤害减免
    if(attackType_got == ATTACKTYPE_CLOSE || attackType_got == ATTACKTYPE_ANIMAL)
    {
        if(upgradable) defValue = defence_close_change[playerScience->get_level(getSort(),type)];
        else defValue = defence_close;
    }
    else if(attackType_got == ATTACKTYPE_SHOOT)
    {
        if(upgradable) defValue = defence_shoot_change[playerScience->get_level(getSort(),type)];
        else defValue = defence_shoot;
    }

    //在defValue的基础上,计算player及科技带来的 加成,并返回
    return (int)( defValue*playerScience->get_rate_Defence(getSort(),type,armyClass,attackType_got) ) + \
            playerScience->get_addition_Defence(getSort(),type,armyClass,attackType_got);
}

//攻击距离
double Army::getDis_attack()
{
    double dis;

    if(upgradable) dis = dis_Attack_change[playerScience->get_level(getSort(),type)];
    else dis = dis_Attack;

    if(dis == 0) dis = DISTANCE_ATTACK_CLOSE;
    else dis = ( dis + playerScience->get_addition_DisAttack(getSort(),type,armyClass,get_AttackType() ) )*BLOCKSIDELENGTH;

    return dis;
}

/***********************************************************/
void Army::setAttribute()
{
    //设置军队属性
    switch (type) {
    case AT_CLUBMAN:        //棍棒兵,可升级1次
        upgradable = true;
        armyClass = ARMY_INFANTRY;
        attackType = ATTACKTYPE_CLOSE;

        MaxBlood_change = new int[2]{ BLOOD_CLUBMAN1,BLOOD_CLUBMAN2 };
        speed_change = new double[2]{ SPEED_CLUBMAN1,SPEED_CLUBMAN2 };
        vision_change = new int[2]{ VISION_CLUBMAN1,VISION_CLUBMAN2 };
        atk_change  = new int[2]{ATK_CLUBMAN1,ATK_CLUBMAN2};
        dis_Attack_change  = new double[2]{DIS_CLUBMAN1 , DIS_CLUBMAN2};
        inter_Attack_change = new double[2]{ INTERVAL_CLUBMAN1,INTERVAL_CLUBMAN2 };
        defence_close_change  = new int[2]{ DEFCLOSE_CLUBMAN1,DEFCLOSE_CLUBMAN2 };
        defence_shoot_change  = new int[2]{ DEFSHOOT_CLUBMAN1,DEFSHOOT_CLUBMAN2 };

        break;

    case AT_SWORDSMAN:  //短剑兵,可升级3次
        upgradable = true;
        armyClass = ARMY_INFANTRY;
        attackType = ATTACKTYPE_CLOSE;

        MaxBlood_change = new int[4]{ BLOOD_SHORTSWORDSMAN1,BLOOD_SHORTSWORDSMAN2,BLOOD_SHORTSWORDSMAN3,BLOOD_SHORTSWORDSMAN4 };
        speed_change = new double[4]{ SPEED_SHORTSWORDSMAN1,SPEED_SHORTSWORDSMAN2,SPEED_SHORTSWORDSMAN3,SPEED_SHORTSWORDSMAN4 };
        vision_change = new int[4]{ VISION_SHORTSWORDSMAN1,VISION_SHORTSWORDSMAN2,VISION_SHORTSWORDSMAN3,VISION_SHORTSWORDSMAN4 };
        atk_change  = new int[4]{ATK_SHORTSWORSMAN1,ATK_SHORTSWORSMAN2,ATK_SHORTSWORSMAN3,ATK_SHORTSWORSMAN4};
        dis_Attack_change  = new double[4]{DIS_SHORTSWORDSMAN1 , DIS_SHORTSWORDSMAN2,DIS_SHORTSWORDSMAN3,DIS_SHORTSWORDSMAN4};
        inter_Attack_change = new double[4]{ INTERVAL_SHORTSWORDSMAN1,INTERVAL_SHORTSWORDSMAN2,INTERVAL_SHORTSWORDSMAN3,INTERVAL_SHORTSWORDSMAN4 };
        defence_close_change  = new int[4]{ DEFCLOSE_SHORTSWORSMAN1,DEFCLOSE_SHORTSWORSMAN2,DEFCLOSE_SHORTSWORSMAN3,DEFCLOSE_SHORTSWORSMAN4 };
        defence_shoot_change  = new int[4]{ DEFSHOOT_SHORTSWORSMAN1,DEFSHOOT_SHORTSWORSMAN2,DEFSHOOT_SHORTSWORSMAN3,DEFSHOOT_SHORTSWORSMAN4 };
        break;

    case AT_SLINGER:    //投石者
        upgradable = false;
        armyClass = ARMY_INFANTRY;
        attackType = ATTACKTYPE_SHOOT;

        MaxBlood = BLOOD_SLINGER;
        speed = SPEED_SLINGER;
        vision = VISION_SLINGER;
        atk = ATK_SLINGER;
        dis_Attack = DIS_SLINGER;
        inter_Attack = INTERVAL_SLINGER;
        defence_close = DEFCLOSE_SLINGER;
        defence_shoot = DEFSHOOT_SLINGER;
        break;

    case AT_BOWMAN:     //弓箭手
        upgradable = false;
        armyClass = ARMY_ARCHER;
        attackType = ATTACKTYPE_SHOOT;

        MaxBlood = BLOOD_BOWMAN;
        speed = SPEED_BOWMAN;
        vision = VISION_BOWMAN;
        atk = ATK_BOWMAN;
        dis_Attack = DIS_BOWMAN;
        inter_Attack = INTERVAL_BOWMAN;
        defence_close = DEFCLOSE_BOWMAN;
        defence_shoot = DEFSHOOT_BOWMAN;
        break;

    case AT_SCOUT:      //侦察骑兵
        upgradable = false;
        armyClass = ARMY_RIDER;
        attackType = ATTACKTYPE_CLOSE;

        MaxBlood = BLOOD_SCOUT;
        speed = SPEED_SCOUT;
        vision = VISION_SCOUT;
        atk = ATK_SCOUT;
        dis_Attack = DIS_SCOUT;
        inter_Attack = INTERVAL_SCOUT;
        defence_close = DEFCLOSE_SCOUT;
        defence_shoot = DEFSHOOT_SCOUT;
        break;

    default:
        break;
    }

}


/*************************析构**********************************/
Army::~Army()
{
    if(MaxBlood_change!=NULL)
    {
        delete MaxBlood_change;
        MaxBlood_change = NULL;
    }

    if(speed_change!=NULL)
    {
        delete speed_change;
        speed_change = NULL;
    }

    if(vision_change!=NULL)
    {
        delete vision_change;
        vision_change = NULL;
    }

    if(atk_change!=NULL)
    {
        delete atk_change;
        atk_change = NULL;
    }

    if(dis_Attack_change!=NULL)
    {
        delete dis_Attack_change;
        dis_Attack_change = NULL;
    }

    if(inter_Attack_change!=NULL)
    {
        delete inter_Attack_change;
        inter_Attack_change = NULL;
    }

    if(defence_close_change!=NULL)
    {
        delete defence_close_change;
        defence_close_change = NULL;
    }

    if(defence_shoot_change!=NULL)
    {
        delete defence_shoot_change;
        defence_shoot_change = NULL;
    }

}
