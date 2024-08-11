#include "Army.h"

//[playerrepresent][num][leve][angel]
std::list<ImageResource>* Army::Walk[2][7][2][8];
std::list<ImageResource>* Army::Disappear[2][7][2][8];
std::list<ImageResource>* Army::Stand[2][7][2][8];
std::list<ImageResource>* Army::Attack[2][7][2][8];
std::list<ImageResource>* Army::Die[2][7][2][8];

//[num][level]
std::string Army::ArmyName[7][2]={{"Clubman","Axeman"},
                                  {"Slinger","Slinger"},
                                  {"Archer","Archer"},
                                  {"Scout","Scout"},
                                  {"Sworder","Sworder"},
                                  {"ImprovedArcher","ImprovedArcher"},
                                  {"Cavalry","Cavalry"}
                                 };

std::string Army::ArmyDisplayName[7][2]={{"棍棒兵","刀斧兵"},
                                         {"投石兵","投石兵"},
                                         {"弓箭手","弓箭手"},
                                         {"侦察骑兵","侦察骑兵"},
                                         {"Prof.Yan","Prof.Yan"},
                                         {"Prof.Lou","Prof.Lou"},
                                         {"Prof.Lu","Prof.Lu"}
                                        };

string Army::click_sound = "Click_Army";

Army::Army()
{

}

Army::Army(double DR,double UR,int Num , Development* playerScience, int playerRepresent)
{
    //设置科技树和阵营
    this->playerScience = playerScience;
    this->playerRepresent = playerRepresent;

    this->Num = Num;

    setAttribute();

    setDRUR(DR, UR);
    updateBlockByDetail();

    setSideLenth();
    this->nextBlockDR=BlockDR;
    this->nextBlockUR=BlockUR;
    setPredictedDRUR(DR, UR);
    setPreviousDRUR(DR, UR);
    setDR0UR0(DR, UR);

    this->nowstate=MOVEOBJECT_STATE_STAND;
    isAttackable = true;

    setNowRes();
    updateImageXYByNowRes();
    this->imageH=DR-UR;

    //设置SN信息
    this->globalNum=10000*getSort()+g_globalNum;
    g_Object.insert({this->globalNum,this});
    g_globalNum++;
}


void Army::nextframe()
{
    if(isDie())
    {
        if( !isDying() )
        {
             setPreDie();
             requestSound_Die();
        }
        else if(!get_isActionEnd() && isNowresShift())
        {
            nowres++;
            if( !changeToDisappear && get_isActionEnd())
            {
                changeToDisappear = true;
                nowres_step = 1000;
                setNowRes();
            }
        }
    }
    else
    {
        if(isNowresShift())
        {
            if(nowres == nowlist->begin())
            {
                if(nowstate == MOVEOBJECT_STATE_ATTACK)
                    requestSound_Attack();
                else if(nowobject == this && nowstate == MOVEOBJECT_STATE_WALK && playerRepresent == NOWPLAYERREPRESENT)
                    requestSound_Walk();
            }

            nowres++;
            if(nowres==nowlist->end())
            {
                nowres=nowlist->begin();
                //读到最后回到最初
                initAttack_perCircle();
            }
        }

        updateMove();
        setNowRes();
    }

    if(playerRepresent != 0 && timer_Visible>0) time_BeVisible();

    updateImageXYByNowRes();
}

void Army::setNowRes()
{
    std::list<ImageResource> *templist = NULL;

    switch (this->nowstate) {
    case MOVEOBJECT_STATE_STAND:
        templist =this->Stand[playerRepresent][Num][getLevel()][Angle];
        break;
    case MOVEOBJECT_STATE_WALK:
        templist =this->Walk[playerRepresent][Num][getLevel()][Angle];
        break;
    case MOVEOBJECT_STATE_ATTACK:
        templist =this->Attack[playerRepresent][Num][getLevel()][Angle];
        break;
    case MOVEOBJECT_STATE_DIE:
        if(changeToDisappear) templist = this->Disappear[playerRepresent][Num][getLevel()][Angle];
        else templist =this->Die[playerRepresent][Num][getLevel()][Angle];
        break;
    default:
        break;
    }
    if(templist!= nowlist)
    {
        nowlist = templist;
        nowres = nowlist->begin();
        initAttack_perCircle();
        initNowresTimer();
    }
}


void Army::requestSound_Attack()
{
    if(Num == AT_IMPROVED|| Num == AT_BOWMAN)
        soundQueue.push("Archer_Attack");
}

void Army::requestSound_Die()
{
    if(Num == AT_SCOUT)
        soundQueue.push("Scout_Die");
    else
        soundQueue.push("Army_Die");
}

void Army::requestSound_Walk()
{
    if(Num == AT_SCOUT)
        soundQueue.push("Scout_Walk");
}
/***********************************************************/
//获取军队的各项数据
//移速
double Army::getSpeed()
{
    double moveSpeed;

    if( upgradable ) moveSpeed = speed_change[getLevel()];
    else moveSpeed = speed;

    return moveSpeed*playerScience->get_rate_Move(getSort(),Num);
}

//血量
int Army::getMaxBlood()
{
    int realmBlood;

    if(upgradable) realmBlood = MaxBlood_change[getLevel()];
    else realmBlood = MaxBlood;

    return  realmBlood*playerScience->get_rate_Blood(getSort(),Num)+playerScience->get_addition_Blood(getSort(),Num);
}

//视野
int Army::getVision()
{
    int realVision;
    if(upgradable) realVision = vision_change[getLevel()];
    else realVision = vision;

    return realVision + playerScience->get_addition_DisAttack(getSort(),Num,armyClass,get_AttackType());
}

//攻击力
int Army::getATK()
{
    int atkValue;//用于存储初始攻击力

    //赋值初始攻击力,依据兵种是否能升级,划分两类赋值方式
    if(upgradable) atkValue = atk_change[getLevel()];
    else atkValue = atk;

    //再atkValue基础上,计算player及科技带来的加成,并返回
//    return (int)( atkValue*playerScience->get_rate_Attack(getSort(),Num,armyClass,get_AttackType()), interactSort, interactNum) + \
//             get_add_specialAttack() + playerScience->get_addition_Attack(getSort(),Num,armyClass,get_AttackType());

    return (int)( atkValue*playerScience->get_rate_Attack(getSort(),Num,armyClass,get_AttackType(), interactSort, interactNum)) + \
            get_add_specialAttack() + playerScience->get_addition_Attack(getSort(),Num,armyClass,get_AttackType());
}

//防御力,分为获取肉搏防御力和投射物防御力
int Army::getDEF(int attackType_got)
{
    int defValue = 0;//用于存储初始防御力

    //赋值defValue;根据attackType_got即收到的伤害类型,选择相应的防御类型:肉盾防御或投射防御.若为祭司转化或(投石车?等)无伤害减免
    if(attackType_got == ATTACKTYPE_CLOSE || attackType_got == ATTACKTYPE_ANIMAL)
    {
        if(upgradable) defValue = defence_close_change[getLevel()];
        else defValue = defence_close;
    }
    else if(attackType_got == ATTACKTYPE_SHOOT)
    {
        if(upgradable) defValue = defence_shoot_change[getLevel()];
        else defValue = defence_shoot;
    }

    //在defValue的基础上,计算player及科技带来的 加成,并返回
    return (int)( defValue*playerScience->get_rate_Defence(getSort(),Num,armyClass,attackType_got) ) + \
            playerScience->get_addition_Defence(getSort(),Num,armyClass,attackType_got);
}

int Army::showATK_Basic()
{
    int atkValue;//用于存储初始攻击力

    //赋值初始攻击力,依据兵种是否能升级,划分两类赋值方式
    if(upgradable) atkValue = atk_change[getLevel()];
    else atkValue = atk;

    return atkValue+get_add_specialAttack();
}

int Army::showDEF_Close()
{
    int defValue = 0;

    if(upgradable) defValue = defence_close_change[getLevel()];
    else defValue = defence_close;

    return defValue;
}

int Army::showDEF_Shoot()
{
    int defValue = 0;

    if(upgradable) defValue = defence_shoot_change[getLevel()];
    else defValue = defence_shoot;

    return defValue;
}

//攻击距离
double Army::getDis_attack()
{
    double dis;

    if(upgradable) dis = dis_Attack_change[getLevel()];
    else dis = dis_Attack;

    if(dis == 0) dis = DISTANCE_ATTACK_CLOSE + (attackObject->getSideLength())/2.0;
    else dis = ( dis + playerScience->get_addition_DisAttack(getSort(),Num,armyClass,get_AttackType() ) )*BLOCKSIDELENGTH;

    return dis;
}

int Army::get_add_specialAttack()
{
    int addition = 0;

    if(Num == AT_SLINGER)
    {
       /* if(interactSort == SORT_ARMY)
        {
            if(interactNum == AT_BOWMAN || interactNum == AT_IMPROVED) addition+=2;
        }
        else */if( interactSort == SORT_BUILDING)
        {
            if(interactNum == BUILDING_ARROWTOWER || interactNum == BUILDING_WALL)
                addition += DEFSHOOT_BUILD_ARROWTOWER;
        }
    }
//    else if(Num == AT_CAVALRY)
//    {
//        if(interactSort == SORT_ARMY)
//        {
//            if(interactNum == AT_CLUBMAN || interactNum == AT_SWORDSMAN)
//                addition+=5;
//        }

//    }

    return addition;
}
/***********************************************************/


void Army::setAttribute()
{
    this->Blood=1;
    this->Angle=rand()%8;
    //设置军队属性
    switch (Num) {
    case AT_CLUBMAN:        //棍棒兵,可升级1次
        upgradable = true;
        dependBuildNum = BUILDING_ARMYCAMP;
        dependBuildAct = BUILDING_ARMYCAMP_UPGRADE_CLUBMAN;
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

        crashLength = CRASHBOX_SINGLEOB;
        nowres_step = NOWRES_TIMER_CLUBMAN;

        break;

//    case AT_SWORDSMAN:  //短剑兵,可升级3次
//        upgradable = true;
//        armyClass = ARMY_INFANTRY;
//        attackType = ATTACKTYPE_CLOSE;

//        MaxBlood_change = new int[4]{ BLOOD_SHORTSWORDSMAN1,BLOOD_SHORTSWORDSMAN2,BLOOD_SHORTSWORDSMAN3,BLOOD_SHORTSWORDSMAN4 };
//        speed_change = new double[4]{ SPEED_SHORTSWORDSMAN1,SPEED_SHORTSWORDSMAN2,SPEED_SHORTSWORDSMAN3,SPEED_SHORTSWORDSMAN4 };
//        vision_change = new int[4]{ VISION_SHORTSWORDSMAN1,VISION_SHORTSWORDSMAN2,VISION_SHORTSWORDSMAN3,VISION_SHORTSWORDSMAN4 };
//        atk_change  = new int[4]{ATK_SHORTSWORSMAN1,ATK_SHORTSWORSMAN2,ATK_SHORTSWORSMAN3,ATK_SHORTSWORSMAN4};
//        dis_Attack_change  = new double[4]{DIS_SHORTSWORDSMAN1 , DIS_SHORTSWORDSMAN2,DIS_SHORTSWORDSMAN3,DIS_SHORTSWORDSMAN4};
//        inter_Attack_change = new double[4]{ INTERVAL_SHORTSWORDSMAN1,INTERVAL_SHORTSWORDSMAN2,INTERVAL_SHORTSWORDSMAN3,INTERVAL_SHORTSWORDSMAN4 };
//        defence_close_change  = new int[4]{ DEFCLOSE_SHORTSWORSMAN1,DEFCLOSE_SHORTSWORSMAN2,DEFCLOSE_SHORTSWORSMAN3,DEFCLOSE_SHORTSWORSMAN4 };
//        defence_shoot_change  = new int[4]{ DEFSHOOT_SHORTSWORSMAN1,DEFSHOOT_SHORTSWORSMAN2,DEFSHOOT_SHORTSWORSMAN3,DEFSHOOT_SHORTSWORSMAN4 };
//        break;

    case AT_SWORDSMAN:    //投石者
        upgradable = false;
        dependBuildNum = BUILDING_ARMYCAMP;
        armyClass = ARMY_INFANTRY;
        attackType = ATTACKTYPE_CLOSE;

        MaxBlood = BLOOD_SHORTSWORDSMAN1;
        speed = SPEED_SHORTSWORDSMAN1;
        vision = VISION_SHORTSWORDSMAN1;
        atk = ATK_SHORTSWORSMAN1;
        dis_Attack = DIS_SHORTSWORDSMAN1;
        inter_Attack = INTERVAL_SHORTSWORDSMAN1;
        defence_close = DEFCLOSE_SHORTSWORSMAN1;
        defence_shoot = DEFSHOOT_SHORTSWORSMAN1;

        crashLength = CRASHBOX_SINGLEOB;
        nowres_step = NOWRES_TIMER_SWORSMAN;
        break;

    case AT_SLINGER:    //投石者
        upgradable = false;
        dependBuildNum = BUILDING_ARMYCAMP;
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

        crashLength = CRASHBOX_SINGLEOB;

        type_Missile = Missile_Cobblestone;
        phaseFromEnd_MissionAttack = THROWMISSION_SLINGER;

        nowres_step = NOWRES_TIMER_SLINGER;
        break;

    case AT_BOWMAN:     //弓箭手
        upgradable = false;
        dependBuildNum = BUILDING_RANGE;
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

        crashLength = CRASHBOX_SINGLEOB;

        type_Missile = Missile_Arrow;
        phaseFromEnd_MissionAttack = THROWMISSION_ARCHER;

        nowres_step = NOWRES_TIMER_BOWMAN;
        break;

    case AT_IMPROVED:     //弓箭手
        upgradable = false;
        dependBuildNum = BUILDING_RANGE;
        armyClass = ARMY_ARCHER;
        attackType = ATTACKTYPE_SHOOT;

        MaxBlood = BLOOD_IMPROVEDBOWMAN1;
        speed = SPEED_IMPROVEDBOWMAN1;
        vision = VISION_IMPROVEDBOWMAN1;
        atk = ATK_IMPROVEDBOWMAN1;
        dis_Attack = DIS_IMPROVEDBOWMAN1;
        inter_Attack = INTERVAL_IMPROVEDBOWMAN1;
        defence_close = DEFCLOSE_IMPROVEDBOWMAN1;
        defence_shoot = DEFSHOOT_IMPROVEDBOWMAN1;

        crashLength = CRASHBOX_SINGLEOB;

        type_Missile = Missile_Arrow;
        phaseFromEnd_MissionAttack = THROWMISSION_IMPROVEDBOWMAN1;

        nowres_step = NOWRES_TIMER_IMPROVEDBOWMAN1;
        break;

    case AT_SCOUT:      //侦察骑兵
        upgradable = false;
        dependBuildNum = BUILDING_STABLE;
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

        crashLength = CRASHBOX_SMALLOB;

        nowres_step = NOWRES_TIMER_SCOUT;
        break;

    case AT_CAVALRY:      //侦察骑兵
        upgradable = false;
        dependBuildNum = BUILDING_STABLE;
        armyClass = ARMY_RIDER;
        attackType = ATTACKTYPE_CLOSE;

        MaxBlood = BLOOD_CAVALRY;
        speed = SPEED_CAVALRY;
        vision = VISION_CAVALRY;
        atk = ATK_CAVALRY;
        dis_Attack = DIS_CAVALRY;
        inter_Attack = INTERVAL_CAVALRY;
        defence_close = DEFCLOSE_CAVALRY;
        defence_shoot = DEFSHOOT_CAVALRY;

        crashLength = CRASHBOX_SMALLOB;

        nowres_step = NOWRES_TIMER_CAVALRY;
        break;
    default:
        incorrectNum = true;
        break;
    }

}


int Army::getLevel()
{
    /**
    *   传出：士兵等级
    *   通过查询player科技树表，得到当前player管控的该种类士兵的等级
    *   如果该种类士兵无法升级，则默认为0级
    */
    if(upgradable) return playerScience->getActLevel(dependBuildNum , dependBuildAct);
    else return 0;
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
