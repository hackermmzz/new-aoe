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
    Building *newbuilding = NULL;
    if(Num == BUILDING_FARM) newbuilding = new Building_Resource(Num,BlockDR,BlockUR,getCiv() , playerScience , represent , percent);
    else newbuilding=new Building(Num,BlockDR,BlockUR, getCiv(), playerScience , represent , percent);

    if(newbuilding->getNum() == BUILDING_HOME) playerScience->addHome();
    else if(newbuilding->getNum() != BUILDING_CENTER) playerScience->add_civiBuildNum(newbuilding->getNum());

    build.push_back(newbuilding);
    return newbuilding;
}

int Player::addHuman(int Num, double DR, double UR)
{
    Human *newhuman=new Human(Num,DR,UR , playerScience , represent);

    call_debugText("blue"," 产生了新的"+newhuman->getChineseName()+"(编号:" + QString::number(newhuman->getglobalNum()) + ")");

    human.push_back(newhuman);
    humanNumIncrease(newhuman);
    return 0;
}

Army* Player::addArmy(int Num , double DR , double UR)
{
    Army *newArmy = new Army(DR , UR, Num ,  playerScience , represent);
    call_debugText("blue"," 产生了新的"+newArmy->getChineseName()+"(编号:" + QString::number(newArmy->getglobalNum()) + ")");

    human.push_back(newArmy);
    humanNumIncrease(newArmy);
    return newArmy;
}

int Player::addFarmer(double DR, double UR)
{
    Farmer *newfarmer=new Farmer(DR,UR , playerScience , represent);
    call_debugText("blue"," 产生了新的村民(编号:" + QString::number(newfarmer->getglobalNum()) + ")");

    human.push_back(newfarmer);
    humanNumIncrease(newfarmer);
    return 0;
}

Missile* Player::addMissile( Coordinate* attacker , Coordinate* attackee )
{
    Missile* newMissile = NULL;
    BloodHaver* aterOb = NULL;

    attacker->printer_ToBloodHaver((void**)&aterOb);
    if(aterOb!=NULL)
    {
        newMissile = new Missile(aterOb->get_type_Missile(), attacker , attackee, playerScience , represent);

        missile.push_back(newMissile);
    }

    return newMissile;
}

//删除实例对象
list<Human*>::iterator Player::deleteHuman( list<Human*>::iterator iterDele )
{
    humanNumDecrease(*iterDele);
    delete *iterDele;
    return human.erase(iterDele);
}

list<Building*>::iterator Player::deleteBuilding( list<Building*>::iterator iterDele )
{
    if((*iterDele)->getNum() == BUILDING_HOME) playerScience->subHome();
    else if((*iterDele)->getNum() == BUILDING_CENTER) playerScience->subCenter();
    else playerScience->sub_civiBuildNum((*iterDele)->getNum());

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

void Player::changeResource(int wood,int food,int stone,int gold, bool negative)
{
    if(negative) wood*=-1,food*=-1,stone*=-1,gold*=-1;//negative为true说明进行扣除资源操作

    this->wood +=wood;
    this->food+=food;
    this->stone+=stone;
    this->gold+=gold;
}

void Player::changeResource_byBuild(int buildNum)
{
    int wood,food,stone,gold;

    playerScience->get_Resource_Consume(buildNum,wood,food,stone,gold);
    changeResource(wood,food,stone,gold,true);
}

void Player::changeResource_byBuildAction(Building* actbuilding , int buildact)
{
    int wood = 0,food = 0,stone = 0,gold = 0;

    playerScience->get_Resource_Consume(actbuilding->getNum(),buildact,wood,food,stone,gold);
    changeResource(wood,food,stone,gold,true);
    actbuilding->set_Resource_TS(wood,food,stone,gold);
}

//***************************************************************
//控制建筑行动
void Player::enforcementAction( Building* actBuild, vector<Point>Block_free  )
{
    bool isNeedCreatObject = false; //是否需要创建对象
    int creatObjectSort , creatObjectNum;
    Point block;

    isNeedCreatObject = playerScience->isNeedCreatObjectAfterAction(actBuild->getNum() , actBuild->getActNum() , creatObjectSort , creatObjectNum);
    block.x = actBuild->getBlockDR();
    block.y = actBuild->getBlockUR();
//    qDebug()<<"actend"<<(actBuild->getActNum());
    playerScience->finishAction(actBuild->getNum() ,actBuild->getActNum());
    actBuild->init_Resouce_TS();    //重置行动建筑的返还资源

    if(isNeedCreatObject)
    {
        //需要优化，添加建筑周围随机位置生成，且避开障碍物
        if(Block_free.size()) block = Block_free[ rand()%Block_free.size() ];

        if(creatObjectSort == SORT_FARMER) addFarmer(trans_BlockPointToDetailCenter(block.x) , trans_BlockPointToDetailCenter(block.y));
        else if(creatObjectSort == SORT_ARMY) addArmy(creatObjectNum , trans_BlockPointToDetailCenter(block.x) , trans_BlockPointToDetailCenter(block.y));
    }
}

void Player::back_Resource_TS( Building* actBuild )
{
    //player的资源加上建筑行动手动取消返回的资源
    int wood,food,stone,gold;
    actBuild->get_Resouce_TS(wood,food,stone,gold);
    this->changeResource(wood,food,stone,gold);
    actBuild->init_Resouce_TS();
}
