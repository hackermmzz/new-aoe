#include "Player.h"

//***************************************************************
//构造与析构
Player::Player()
{
    playerScience = new Development();
}

Player::Player(int represent)
{
    this->represent = represent;
    playerScience = new Development();
}

Player::~Player()
{
    delete playerScience;
    //清空池子
    list<Human*>::iterator iter_deleHuman = human.begin();
    list<Building*>::iterator iter_deleBuild = build.begin();
    list<Missile*>::iterator iter_deleMissile = missile.begin();

    while(iter_deleHuman!=human.end())iter_deleHuman = deleteHuman(iter_deleHuman);
    while(iter_deleBuild!=build.end())iter_deleBuild = deleteBuilding(iter_deleBuild);
    while(iter_deleMissile!=missile.end())iter_deleMissile = deleteMissile(iter_deleMissile);
}

//***************************************************************
//添加实例对象
Building* Player::addBuilding(int Num, int BlockDR, int BlockUR , double percent)
{
    Building *newbuilding=new Building(Num,BlockDR,BlockUR,this->civilization , percent);
    newbuilding->setPlayerScience(playerScience);
    newbuilding->setPlayerRepresent(represent);
    build.push_back(newbuilding);
    return newbuilding;
}

int Player::addHuman(int Num, double DR, double UR)
{
    Human *newhuman=new Human(Num,DR,UR);
    newhuman->setPlayerScience(playerScience);
    newhuman->setPlayerRepresent(represent);
    human.push_back(newhuman);
    return 0;
}

Army* Player::addArmy(int Num , double DR , double UR)
{
    Army *newArmy = new Army(DR , UR, Num);
    newArmy->setPlayerScience(playerScience);
    newArmy->setPlayerRepresent(represent);
    human.push_back(newArmy);

    return newArmy;
}

int Player::addFarmer(double DR, double UR)
{
    Farmer *newfarmer=new Farmer(DR,UR);
    newfarmer->setPlayerScience(playerScience);
    newfarmer->setPlayerRepresent(represent);
    human.push_back(newfarmer);
    return 0;
}

Missile* Player::addMissile( Coordinate* attacker , Coordinate* attackee )
{
    Missile* newMissile = NULL;
    BloodHaver* aterOb = NULL;

    attacker->printer_ToBloodHaver((void**)&aterOb);
    if(aterOb!=NULL)
    {
        newMissile = new Missile(aterOb->get_type_Missile(), attacker , attackee);
        newMissile->setPlayerScience(playerScience);
        newMissile->setPlayerRepresent(represent);
        missile.push_back(newMissile);
    }

    return newMissile;
}

//删除实例对象
list<Human*>::iterator Player::deleteHuman( list<Human*>::iterator iterDele )
{
    delete *iterDele;
    return human.erase(iterDele);
}

list<Building*>::iterator Player::deleteBuilding( list<Building*>::iterator iterDele )
{
    delete *iterDele;
    return build.erase(iterDele);
}

list<Missile*>::iterator Player::deleteMissile( list<Missile*>::iterator iterDele )
{
    delete *iterDele;
    return missile.erase(iterDele);
}

//删除missile投掷者（其已死亡，原指针被delete），改为使用投资者记录
void Player::deleteMissile_Attacker( Coordinate* attacker )
{
    for(list<Missile*>::iterator iter = missile.begin();iter!=missile.end();iter++)
        (*iter)->deleteAttackerSponsor(attacker);
}

//***************************************************************
//控制资源
void Player::changeResource( int resourceSort , int num  , bool negative)
{
    if(negative) num *= -1;     //negative为true说明进行扣除资源操作

    switch (resourceSort)
    {
        case HUMAN_GOLD:
            gold+=num;
            break;
        case HUMAN_WOOD:
            wood+=num;
            break;
        case HUMAN_STONE:
            stone+=num;
            break;
        case HUMAN_GRANARYFOOD:
        case HUMAN_STOCKFOOD:
            food+=num;
            break;
        default:
            break;
    }
}

//***************************************************************
//控制建筑行动
void Player::enforcementAction( Building* actBuild )
{
    bool isNeedCreatObject = false;
    int creatObjectSort , creatObjectNum;

    isNeedCreatObject = playerScience->isNeedCreatObjectAfterAction(actBuild->getNum() , actBuild->getActNum() , creatObjectSort , creatObjectNum);
    playerScience->finishAction(actBuild->getNum() ,actBuild->getActNum());

    if(isNeedCreatObject)
    {
        //需要优化，添加建筑周围随机位置生成，且避开障碍物
        if(creatObjectSort == SORT_FARMER) addFarmer(actBuild->getDR() , actBuild->getUR());
        else if(creatObjectSort == SORT_ARMY) addArmy(creatObjectNum , actBuild->getDR() , actBuild->getUR());
    }
}
