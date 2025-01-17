#include "UsrAI.h"
tagGame tagUsrGame;
ins UsrIns;
/*##########请勿修改以上部分##########*/
#include<fstream>
tagInfo Info;
double mid=36*BLOCKSIDELENGTH;  // 中间位置的坐标
int Blockmid=36;
bool once=true;
bool isCamp=false;  // 是否有军营
bool isCampFinish=false;  // 军营是否建造完成
int tmpFrame=(int)1e6;  // 临时帧计数
int tmpFrame_arrow=(int)1e6;
int tmpFrame_army=(int)1e6;
int tmpFrame_fix=(int)1e6;
int id_Wrong_lastFrame = -1;
bool is_update=false;
void UsrAI::processData()
{
    qDebug()<<"success";
    int nowState_Farmer;
    int farmer_max=4;
    int farmer_num=0;
    int SN_res;
    double dis,temp_dis;
    int order_Record;
    static int timers = 1;
    int map[72][72]={0};
    int tagmap[72][72]={0};
    int yymap[72][72]={0};//阴影地图
    int house = 0;
    bool is_blood = false;
    int barn = 0;//谷仓
    int stock = 0;//仓库
    int farm = 0;//农场
    int stable = 0;//马厩
    int mark = 0;
    int arrowtower = 0;//箭塔
    int is_enemy = false;
    int collection = 0; //采集
    int farmer_1 =0;//农场主
    int hunter = 0;
    int mining = 0;
    int construct = 0;
    int woodcutter = 0;
    Info=getInfo(); //每帧获取新的tagInfo
    int have_bush =false;
    int have_gazelle=false;
    if(id_Wrong_lastFrame>=0&&timers>0) //记录了上一帧下达命令的编号
       {
          order_Record = Info.ins_ret[id_Wrong_lastFrame];  //获取上一帧下达命令的结果
         DebugText( ("命令"+ QString::number(id_Wrong_lastFrame) +"的返回为"+ QString::number(order_Record)).toStdString());
            id_Wrong_lastFrame = -1;
            timers--;  //只获取并打印一次
       }
    //农民数量
    for(int i=0 ; i<Info.farmers.size() ;i++){
        farmer_num++;
        if(Info.farmers[i].NowState!=HUMAN_STATE_IDLE)
        {
            for(int j=0 ;j<Info.buildings.size() ;j++){
                if(Info.farmers[i].WorkObjectSN == Info.buildings[j].SN && Info.buildings[j].Type !=BUILDING_FARM)
                {
                    construct++;
                    break;
                }
                if(Info.farmers[i].WorkObjectSN== Info.buildings[j].SN && Info.buildings[j].Type==BUILDING_FARM )
                {
                    farmer_1++;
                    break;
                }
            }
            for(int j=0 ;j<Info.resources.size() ;j++){
                if(Info.farmers[i].WorkObjectSN == Info.resources[j].SN)
                {
                    if(Info.resources[j].Type==RESOURCE_STONE)
                    {
                        mining++;
                        break;
                    }
                    if(Info.resources[j].Type==RESOURCE_TREE)
                    {
                        woodcutter++;
                        break;
                    }
                    if(Info.resources[j].Type== RESOURCE_BUSH)
                    {
                        collection++;
                        break;
                    }
                    if(Info.resources[j].Type== RESOURCE_GAZELLE || Info.resources[j].Type==RESOURCE_ELEPHANT)
                    {
                        hunter++;
                        break;
                    }
                }
            }
            /*qDebug()<<"工人有"<<construct<<"个";
            qDebug()<<"挖矿的有"<<mining<<"个";
            qDebug()<<"伐木工有"<<woodcutter<<"个";
            qDebug()<<"采果子的有"<<collection<<"个";*/
            //qDebug()<<"农场主有"<<farmer_1<<"个";
        }
    }
    //复制地形地图
    for(int i=0; i<72 ;i++){
        for(int j=0; j<72 ;j++){
            map[i][j] = Info.theMap[i][j];
            //qDebug()<<map[i][j]<<" ";
        }
        //qDebug()<<"\n";
    }
    //物资地图
    for(int i=0; i<72 ;i++)
    {
        for(int j=0; j<72 ;j++){
            tagmap[i][j] = map[i][j];
        }
    }
    for(Resource : getInfo().resources)
    {
        tagmap[Resource.BlockDR][Resource.BlockUR]=Resource.Type;
        if(Resource.Type==RESOURCE_STONE)
        {
            for(int i=0 ; i<2 ;i++){
                for(int j=0 ; j<2 ;j++){
                    if(i==0 && j==0)
                    {
                        tagmap[Resource.BlockDR + i][Resource.BlockUR + j]=Resource.Type;
                    }
                    else
                    {
                        tagmap[Resource.BlockDR + i][Resource.BlockUR + j]=Resource.Type + 1;
                    }
                }
            }
        }
        if(Resource.Type==RESOURCE_GAZELLE ||Resource.Type==RESOURCE_ELEPHANT)
        {
            /*DebugText("读到动物了");
            if(Resource.Type==RESOURCE_ELEPHANT){
                DebugText("大象");
            }*/
            if(Resource.Type==RESOURCE_GAZELLE && Resource.Blood > 0)
            {
                is_blood = true;
            }
            tagmap[Resource.BlockDR][Resource.BlockUR]=Resource.Type;
        }
    }
    for(int i=0; i<72 ;i++){
        for(int j=0; j<72 ;j++){
            if(tagmap[i][j]== RESOURCE_BUSH)
            {
                have_bush =true;
            }
            if(tagmap[i][j]== RESOURCE_GAZELLE)
            {
                have_gazelle =true;
            }
        }
    }
    for(building : getInfo().buildings)
    {
        if(building.Type!=BUILDING_RANGE || building.Type!=BUILDING_HOME)//不是箭塔或者房子
            {
                for(int i=0; i<3 ;i++)
                {
                    for(int j=0; j<3 ;j++)
                    {
                        tagmap[building.BlockDR + i][building.BlockUR + j]='*';
                    }
                }
            }
        else
        {
            for(int i=0; i<2 ;i++)
            {
                for(int j=0; j<2 ;j++)
                {
                    tagmap[building.BlockDR + i][building.BlockUR + j]='*';
                }
            }
        }
    }
    /*for (int i = 0; i < 72; i++) {
            for (int j = 0; j < 72; j++) {
                qDebug() << "tagmap[" << i << "][" << j << "] =" << tagmap[i][j];
            }
        }*/

    //建筑
    for(building : getInfo().buildings)
        {
          if(building.Type==BUILDING_HOME)//如果建筑类型是房子且建造完成，增加房子数量计数
          {
              house++;
              farmer_max+=4;
          }
          else if(building.Type==BUILDING_STOCK)//仓库
          {
              stock++;
              if(isCamp==true && Info.Meat>=125)
              {
                  BuildingAction(building.SN, BUILDING_STOCK_UPGRADE_DEFENSE_RIDER);
                  is_update = true ;
              }
          }
          else if( building.Type==BUILDING_ARMYCAMP )// 如果建筑类型是军营
            {
                isCamp=true;    //标记有军营
                if(getInfo().GameFrame>tmpFrame+10) // 军营建好后过一会，执行创建棍棒兵的指令
                {
                 //下达命令并获取命令号
                    //id_Wrong_lastFrame = BuildingAction(building.SN, BUILDING_ARMYCAMP_CREATE_CLUBMAN);
                }
                if(building.Percent==100&&!isCampFinish)// 如果军营建造完成且尚未标记完成
                {
                    isCampFinish=true;  // 标记军营建造完成
                    tmpFrame=tagUsrGame.getInfo().GameFrame;  // 记录下当前帧数
                    DebugText("我有兵营啦"); //打印信息到debugText窗口
                }
            }
         else if( building.Type==BUILDING_CENTER && building.Project==ACT_NULL && getInfo().farmers.size()<12 && is_enemy == false)
            {       //创建农民
                BuildingAction(building.SN,BUILDING_CENTER_CREATEFARMER);
            }
            else if( building.Type==BUILDING_CENTER && building.Project==ACT_NULL && getInfo().farmers.size()<8)
            {
              BuildingAction(building.SN,BUILDING_CENTER_CREATEFARMER);
            }
         if(building.Type==BUILDING_GRANARY)//谷仓
         {
             barn++;
             if(Info.Meat>=50){
                 if(building.Project==ACT_NULL)
                 {
                     BuildingAction(building.SN,BUILDING_GRANARY_ARROWTOWER);
                 }
             }
         }
         if(building.Type==BUILDING_MARKET)//市场
         {
             mark++;
             if(Info.Meat>=120 && Info.Wood >=75){
                 if(building.Project==ACT_NULL)
                 {
                     BuildingAction(building.SN,BUILDING_MARKET_WOOD_UPGRADE);
                 }
             }
         }
         if((building.Type==BUILDING_STABLE))//马厩
         {
             stable++;
             if(Info.Meat>=60 && is_update==true ){
                 if(building.Project==ACT_NULL)
                 {
                     BuildingAction(building.SN,BUILDING_STABLE_CREATE_SCOUT);
                 }
             }
         }
         if((building.Type==BUILDING_FARM))//农场
         {
            farm++;
         }
        }
    int is_attack =false ;//箭塔是否攻击
    for(int i=0 ;i<Info.buildings.size();i++){
        if(Info.buildings[i].Type==BUILDING_ARROWTOWER){
            is_attack =false;
            arrowtower++;//记录箭塔数量
            //箭塔攻击函数
            for(int j=0; j<Info.enemy_armies.size() ;j++){
                if((abs(Info.enemy_armies[j].BlockDR - Info.buildings[i].BlockDR) <= 10 && abs(Info.enemy_armies[j].BlockUR - Info.buildings[i].BlockUR) <= 10
                        && abs(Info.enemy_armies[j].BlockDR - Info.buildings[i].BlockDR)+abs(Info.enemy_armies[j].BlockUR - Info.buildings[i].BlockUR)<15
                        && is_attack == false && Info.buildings[i].Project == 0 )||(abs(Info.enemy_armies[j].DR - Info.buildings[i].BlockDR) <= 10 && abs(Info.enemy_armies[j].UR - Info.buildings[i].BlockUR) <= 10
                        && abs(Info.enemy_armies[j].DR - Info.buildings[i].BlockDR)+abs(Info.enemy_armies[j].UR - Info.buildings[i].BlockUR)<15
                        && is_attack == false && Info.buildings[i].Project == 0 ))
                {
                    if(getInfo().GameFrame>tmpFrame_arrow+100 || tmpFrame_arrow==(int)1e6)
                    {
                        HumanAction(Info.buildings[i].SN,Info.enemy_armies[j].SN);
                        tmpFrame_arrow = tagUsrGame.getInfo().GameFrame;
                        is_attack=true;
                    }
                }
            }
        }
    }
    //遍历村民
    for(int i=0 ;i<Info.farmers.size();i++)
        {
            int is_work = false;
            /*if(Info.farmers[i].NowState==HUMAN_STATE_STOP)
            {
                double x =36.0;
                double y=36.0;
                for(int m=x-10 ;m<x+10 ;m++){
                    for(int n=y-10;n<y+10 ;n++){
                        if(tagmap[m][n]<=4 && tagmap[m][n]>=0)
                        {
                            HumanMove(Info.farmers[i].SN,m,n);
                        }
                    }
                }
            }*/
            /*for(int j=0 ;j<Info.buildings.size() ;j++){//修中心
                if(Info.buildings[j].Type==BUILDING_CENTER && Info.buildings[j].Blood<Info.buildings[j].MaxBlood && is_work == false)
                {
                    Info.farmers[i].NowState =HUMAN_STATE_IDLE;
                    is_work = false;
                    if(Info.buildings[j].Blood<Info.buildings[j].MaxBlood && is_work ==false && Info.buildings[j].Percent>=0)
                    {
                        HumanAction(Info.farmers[i].SN,Info.buildings[j].SN);
                        is_work = true;
                        break;
                    }
                }
            }*/
            if(Info.farmers[i].NowState!=HUMAN_STATE_IDLE)
            {
                is_work =true;
            }
            if((Info.farmers[i].NowState==HUMAN_STATE_IDLE &&is_work == false)||(i<=6))
            {

                for(int j=0 ;j<Info.buildings.size() ;j++){//修建筑
                    if(Info.buildings[j].Type==BUILDING_CENTER)
                    {
                        if(Info.buildings[j].Type==BUILDING_CENTER && Info.buildings[j].Blood<Info.buildings[j].MaxBlood)
                        {
                            is_enemy = true;
                            if( i<=6 && Info.farmers[i].WorkObjectSN != Info.buildings[j].SN &&(tmpFrame_fix==(int)1e6 || tagUsrGame.getInfo().GameFrame>tmpFrame_fix+50 ))
                            {
                                HumanAction(Info.farmers[i].SN,Info.buildings[j].SN);
                                is_work = true;
                                tmpFrame_fix=tagUsrGame.getInfo().GameFrame;
                                break;
                            }
                        }
                    }
                    /*else {
                       if(Info.buildings[j].Type==BUILDING_STABLE && Info.buildings[j].Blood<Info.buildings[j].MaxBlood && is_work==false)
                       {
                           if(i<=5 && Info.farmers[i].WorkObjectSN != Info.buildings[j].SN)
                           {
                               HumanAction(Info.farmers[i].SN,Info.buildings[j].SN);
                               is_work = true;
                               break;
                           }
                       }
                    }*/
                    }
                for(int j=0 ;j<Info.buildings.size() ;j++){//修建筑
                    if(Info.buildings[j].Blood<Info.buildings[j].MaxBlood && is_work ==false && Info.buildings[j].Percent>=0)
                    {
                        if(Info.buildings[j].Type==BUILDING_STABLE && Info.buildings[j].Blood<Info.buildings[j].MaxBlood && is_work==false)
                        {
                            if(Info.farmers[i].WorkObjectSN != Info.buildings[j].SN)
                            {
                                HumanAction(Info.farmers[i].SN,Info.buildings[j].SN);
                                is_work = true;
                                break;
                            }
                        }
                    }
                }
                for(int j=0 ;j<Info.buildings.size() ;j++){//修建筑
                    if(Info.buildings[j].Blood<Info.buildings[j].MaxBlood && is_work ==false && Info.buildings[j].Percent>=0 && construct< 3)
                    {
                        if(Info.farmers[i].WorkObjectSN != Info.buildings[j].SN)
                        {
                            HumanAction(Info.farmers[i].SN,Info.buildings[j].SN);
                            is_work = true;
                            break;
                        }
                    }
                }
                if(stable < 1 && Info.Wood>=150 && construct< 5 && is_work==false && isCamp==true)//建马厩
                {
                    int BlockDR=Blockmid-6,BlockUR=Blockmid-6;
                    find_armycamp_xy(BlockDR,BlockUR,tagmap);
                    HumanBuild(Info.farmers[i].SN, BUILDING_STABLE, BlockDR, BlockUR);
                    is_work = true;
                }
                if((house < 2 && Info.Wood>=30 && construct< 3 && is_work == false )||(house < 2 && Info.Wood>=30 && construct< 9 && is_work == false && is_update == true))
                {
                    int BlockDR=Blockmid-10,BlockUR=Blockmid-10;
                    find_home_xy(BlockDR,BlockUR,tagmap);
                    //qDebug()<<BlockDR<<" "<<BlockUR;
                    if (find_home_xy(BlockDR, BlockUR, tagmap) == 0) {
                        //qDebug() << BlockDR << " " << BlockUR<<" ";
                        HumanBuild(Info.farmers[i].SN, BUILDING_HOME, BlockDR, BlockUR);
                        is_work =true;
                    } else {
                        qDebug() << "No suitable position found for building home.";
                    }
                }
                if((house < 4 && Info.Wood>=30 && construct< 3 && is_work == false && isCamp==true && Info.farmers.size()>=12)||(house < 4 && Info.Wood>=30 && construct< 9 && is_work == false && isCamp==true && Info.farmers.size()>=12 && is_update == true))
                {
                    int BlockDR=Blockmid-10,BlockUR=Blockmid-10;
                    find_home_xy(BlockDR,BlockUR,tagmap);
                    //qDebug()<<BlockDR<<" "<<BlockUR;
                    if (find_home_xy(BlockDR, BlockUR, tagmap) == 0) {
                        //qDebug() << BlockDR << " " << BlockUR<<" ";
                        HumanBuild(Info.farmers[i].SN, BUILDING_HOME, BlockDR, BlockUR);
                        is_work =true;
                    } else {
                        qDebug() << "No suitable position found for building home.";
                    }
                }
                if(house < 5 && Info.Wood>=30 && construct< 3 && is_work == false && isCamp==true && stable>0 && Info.farmers.size()>=20)
                {
                    int BlockDR=Blockmid-10,BlockUR=Blockmid-10;
                    find_home_xy(BlockDR,BlockUR,tagmap);
                    //qDebug()<<BlockDR<<" "<<BlockUR;
                    if (find_home_xy(BlockDR, BlockUR, tagmap) == 0) {
                        //qDebug() << BlockDR << " " << BlockUR<<" ";
                        HumanBuild(Info.farmers[i].SN, BUILDING_HOME, BlockDR, BlockUR);
                        is_work =true;
                    } else {
                        qDebug() << "No suitable position found for building home.";
                    }
                }
                if(barn < 1 && Info.Wood>=120 && construct< 3 && is_work == false && have_bush==true)//谷仓
                {
                    int BlockDR=0,BlockUR=0;
                    find_barn_xy(BlockDR,BlockUR,tagmap,RESOURCE_BUSH);
                    //qDebug()<<BlockDR<<" "<<BlockUR;
                    if (find_barn_xy(BlockDR, BlockUR, tagmap,RESOURCE_BUSH) == 0) {
                        //qDebug() << BlockDR << " " << BlockUR<<" ";
                        HumanBuild(Info.farmers[i].SN, BUILDING_GRANARY, BlockDR, BlockUR);
                        is_work = true;
                        //construct++;
                    } else {
                        qDebug() << "No suitable position found for building barn.";
                    }
                }

                if(Info.Stone>=150 && construct<3 && is_work == false)//箭塔
                {
                    for(int j=0;j<Info.buildings.size();j++){
                        if(Info.buildings[j].Type== BUILDING_GRANARY)
                        {
                            if(Info.buildings[j].Percent==100 &&Info.buildings[j].ProjectPercent==100)
                            {
                                int BlockDR=36,BlockUR=36;
                                find_arrowtower_xy(BlockDR, BlockUR,tagmap,arrowtower);
                                HumanBuild(Info.farmers[i].SN,BUILDING_ARROWTOWER,BlockDR,BlockUR);
                                is_work =true;
                                break;
                            }
                        }
                    }
                }
                if(Info.Wood>=125 && construct< 5 && is_work==false && isCamp==false && arrowtower>=2)//建军营
                {
                    int BlockDR=Blockmid-10,BlockUR=Blockmid-10;
                    find_armycamp_xy(BlockDR,BlockUR,tagmap);
                    HumanBuild(Info.farmers[i].SN, BUILDING_ARMYCAMP, BlockDR, BlockUR);
                    is_work = true;
                }
                /*if(mark < 1 && barn > 0 && Info.Wood>=150 && construct< 5 && is_work == false && arrowtower>=2 && stable>0)//建市场
                {
                    int BlockDR=Blockmid-6,BlockUR=Blockmid-6;
                    find_armycamp_xy(BlockDR,BlockUR,tagmap);
                    HumanBuild(Info.farmers[i].SN, BUILDING_MARKET, BlockDR, BlockUR);
                    is_work = true;
                }*/
                if(farm < 1 && Info.Wood>=75 && construct< 5 && is_work==false &&  isCamp==true && stable>0 && mark>0 && have_gazelle==false)//建农场
                {
                    int BlockDR=Blockmid-6,BlockUR=Blockmid-6;
                    find_farm_xy(BlockDR,BlockUR,tagmap);
                    HumanBuild(Info.farmers[i].SN, BUILDING_FARM, BlockDR, BlockUR);
                    is_work = true;
                }
                if(collection<4 && farmer_max<=30 && is_work == false && have_bush == true)
                {
                    for(Resource : getInfo().resources){
                        if(Resource.Type==RESOURCE_BUSH)
                        {
                            HumanAction(Info.farmers[i].SN,Resource.SN);
                            is_work =true;
                            break;
                        }
                    }
                    break;
                }
                if(hunter<6 && farmer_max<=30 && is_work == false && have_bush == false && have_gazelle == true && is_blood == true)
                {
                    for(Resource : getInfo().resources){
                        if(Resource.Type==RESOURCE_GAZELLE)
                        {
                            HumanAction(Info.farmers[i].SN,Resource.SN);
                            is_work =true;
                            break;
                        }
                    }
                    break;
                }
                if((woodcutter<5 && farmer_max<=30 && is_work == false && stable<1 )||(house < 4 && woodcutter<4 && farmer_max<=30 && is_work == false))
                {
                    for(Resource : getInfo().resources){
                        if(Resource.Type==RESOURCE_TREE)
                        {
                            int BlockDR=0,BlockUR=0;
                            tree_min(BlockDR,BlockUR,tagmap);
                            if(Resource.BlockDR==BlockDR && Resource.BlockUR==BlockUR)
                            {
                                HumanAction(Info.farmers[i].SN , Resource.SN);
                                //woodcutter++;
                                is_work =true;
                            }
                        }
                    }
                    break;
                }
                if(mining < 4 && farmer_max<=30 && is_work == false)
                {
                    int BlockDR=36,BlockUR=36;
                    stone_min(BlockDR,BlockUR,tagmap);
                    if(stock!=2 && Info.Wood>=120)
                    {
                        find_stock_xy(BlockDR,BlockUR,tagmap);
                        HumanBuild(Info.farmers[i].SN, BUILDING_STOCK , BlockDR, BlockUR);
                        is_work =true;
                    }
                    if(Info.Wood<120 && stock!=2)
                        {
                        for(Resource : getInfo().resources){
                            if(Resource.Type==RESOURCE_TREE)
                            {
                                int x=0,y=0;
                                tree_min(x,y,tagmap);
                                if(Resource.BlockDR==x && Resource.BlockUR==y)
                                {
                                    HumanAction(Info.farmers[i].SN , Resource.SN);
                                    //woodcutter++;
                                    is_work =true;
                                }
                            }
                        }
                        }
                    if(stock==2)
                    {
                        BlockDR=36;
                        BlockUR=36;
                        stone_min(BlockDR,BlockUR,tagmap);
                        for(Resource : getInfo().resources){
                            if(Resource.Type==RESOURCE_STONE){
                                if(Resource.BlockDR==BlockDR && Resource.BlockUR==BlockUR)
                                {
                                    HumanAction(Info.farmers[i].SN , Resource.SN);
                                    is_work =true;
                                }
                            }
                    }
                }
                }
                if(hunter<4 && farmer_max<=30 && is_work == false && have_bush == false && have_gazelle == true && is_enemy== true)
                {
                    for(Resource : getInfo().resources){
                        if(Resource.Type==RESOURCE_GAZELLE)
                        {
                            HumanAction(Info.farmers[i].SN,Resource.SN);
                            is_work =true;
                            break;
                        }
                    }
                    break;
                }
                if(farmer_1 < 1 && farmer_max<=20 && is_work == false && farm>0 && have_bush == false && stable>0)
                {
                    for(int j=0 ;j<Info.buildings.size();j++){
                        int t=false;//是否被耕种
                        if(Info.buildings[j].Type==BUILDING_FARM && Info.buildings[j].Cnt>0)
                        {
                            for(int m=0 ;m<Info.farmers.size();m++){
                                if(Info.farmers[m].WorkObjectSN==Info.buildings[j].SN)
                                {
                                    t = true;
                                    break;
                                }
                            }
                            if(t==false && farmer_1 < 2)
                            {
                                HumanAction(Info.farmers[i].SN,Info.buildings[j].SN);
                                is_work =true;
                            }
                        }
                    }
                }
                if(farmer_max<=30 && is_work == false && have_bush == true)
                {
                    for(Resource : getInfo().resources){
                        if(Resource.Type==RESOURCE_BUSH)
                        {
                            HumanAction(Info.farmers[i].SN,Resource.SN);
                            is_work =true;
                            break;
                        }
                    }
                    break;
                }
                if( farmer_max<=30 && is_work == false && have_bush == false && have_gazelle == true && is_blood == true)
                {
                    for(Resource : getInfo().resources){
                        if(Resource.Type==RESOURCE_GAZELLE)
                        {
                            HumanAction(Info.farmers[i].SN,Resource.SN);
                            is_work =true;
                            break;
                        }
                    }
                    break;
                }
        }
    }
    //我的军队
    for(int i=0 ; i<Info.armies.size() ;i++){
        int is_work=false;
        if(Info.armies[i].NowState==HUMAN_STATE_IDLE)
        {
            for(int j=0; j<Info.enemy_armies.size() ;j++){
                        if(Info.enemy_armies[j].BlockDR>0 &&Info.enemy_armies[j].BlockDR<72 &&Info.enemy_armies[j].BlockUR<72
                           && Info.enemy_armies[j].BlockUR>0)
                        {
                            if(Info.enemy_armies[j].Sort==AT_SCOUT){
                                HumanAction(Info.armies[i].SN,Info.enemy_armies[j].SN);
                                is_work=true;
                            }
                        }
                    }
            for(int j=0; j<Info.enemy_armies.size() ;j++){
                        if(Info.enemy_armies[j].BlockDR>0 &&Info.enemy_armies[j].BlockDR<72 &&Info.enemy_armies[j].BlockUR<72
                           && Info.enemy_armies[j].BlockUR>0 && is_work == false)
                        {
                            if(Info.enemy_armies[j].Sort==AT_BOWMAN){
                                HumanAction(Info.armies[i].SN,Info.enemy_armies[j].SN);
                                is_work=true;
                            }
                        }
                    }
            for(int j=0; j<Info.enemy_armies.size() ;j++){
                        if(Info.enemy_armies[j].BlockDR>0 &&Info.enemy_armies[j].BlockDR<72 &&Info.enemy_armies[j].BlockUR<72
                           && Info.enemy_armies[j].BlockUR>0 && is_work == false)
                        {
                                HumanAction(Info.armies[i].SN,Info.enemy_armies[j].SN);
                                is_work=true;
                        }
                    }
                /*if((is_work == false &&tagUsrGame.getInfo().GameFrame>tmpFrame_army+100)||(is_work == false && tmpFrame_army==(int)1e6 ))
                {
                    //到处走
                    int t= Info.armies[i].SN % 4;
                    switch (t) {
                    case 0:
                        for(double m=6.0 ;m<66 ;m++){
                            for(double n=6.0 ;n<66;n++){
                                HumanMove(Info.armies[i].SN,m,n);
                                tmpFrame_army = tagUsrGame.getInfo().GameFrame;
                            }
                        }
                        break;
                    case 1:
                        for(double m=66.0 ;m>6 ;m--){
                            for(double n=6.0 ;n<66;n++){
                                HumanMove(Info.armies[i].SN,m,n);
                                tmpFrame_army = tagUsrGame.getInfo().GameFrame;
                            }
                        }
                        break;
                    case 2:
                        for(double m=66.0 ;m>6 ;m--){
                            for(double n=6.0 ;n<66;n++){
                                HumanMove(Info.armies[i].SN,n,m);
                                tmpFrame_army = tagUsrGame.getInfo().GameFrame;
                            }
                        }
                        break;
                    case 3:
                        for(double m=6.0 ;m<66 ;m++){
                            for(double n=6.0 ;n<66;n++){
                                HumanMove(Info.armies[i].SN,n,m);
                                tmpFrame_army = tagUsrGame.getInfo().GameFrame;
                            }
                        }
                        break;
                    default:
                        break;
                    }


                }*/
            }
        }
    }
    bool isBuildingturn3 (int x,int y,int tagmap[72][72])
    {
        for(int i=x; i<x+3 ;i++)
        {
            for(int j=y; j<y+3 ;j++)
            {
                for(int m=0 ;m<Info.farmers.size() ;m++){
                    if(Info.farmers[m].BlockDR == i && Info.farmers[m].BlockUR==j)
                    {
                        return false;
                    }
                }
                for(int m=0 ;m<Info.armies.size() ;m++){
                    if(Info.armies[m].BlockDR == i && Info.armies[m].BlockUR==j)
                    {
                        return false;
                    }
                }
            if(tagmap[i][j]!=tagmap[x][y] || tagmap[i][j]>4 || tagmap[i][j]<0)
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool isBuildingturn2(int x,int y,int tagmap[72][72])
    {
        for(int i=x; i<x+2 ;i++)
        {
            for(int j=y; j<y+2 ;j++)
            {
                for(int m=0 ;m<Info.farmers.size() ;m++){
                    if(Info.farmers[m].BlockDR == i && Info.farmers[m].BlockUR==j)
                    {
                        return false;
                    }
                }
                for(int m=0 ;m<Info.armies.size() ;m++){
                    if(Info.armies[m].BlockDR == i && Info.armies[m].BlockUR==j)
                    {
                        return false;
                    }
                }
                if(tagmap[i][j]!=tagmap[x][y] || tagmap[i][j]>4 || tagmap[i][j]<0)
                {
                    return false;
                }
            }
        }
        return true;
    }

    int UsrAI::find_home_xy(int &x, int &y ,int tagmap[72][72])
    {
        for(int i=Blockmid-7 ; i<Blockmid+7 ;i++){
            for(int j=Blockmid-7 ; j<Blockmid+7 ;j++){
                if(isBuildingturn2(i,j,tagmap)==true){
                    x=i;
                    y=j;
                    return 0;
                }
            }
        }
        return -1;
    }

    int UsrAI::find_armycamp_xy(int &x, int &y ,int tagmap[72][72])
    {
        for(int i=Blockmid+6 ; i>Blockmid-10 ;i--){
            for(int j=Blockmid-6 ; j<Blockmid+10 ;j++){
                if(isBuildingturn3(i,j,tagmap)==true){
                    x=i;
                    y=j;
                    return 0;
                }
            }
        }
        return -1;
    }

    int UsrAI::find_stock_xy(int &x, int &y,int tagmap[72][72]){
        int map_1[72][72]={0};
        for(int i=x-10; i<x+10 ;i++){
            for(int j=y-10; j<y+10 ;j++){
                if(tagmap[i][j]==RESOURCE_STONE)
                {
                    map_1[i][j]=tagmap[i][j];
                }
                 //qDebug() << "map[" << i << "][" << j << "] =" << map_1[i][j];
            }
        }
        int t=0;//判断
        int ts=0;//兜底
        for(int i=0; i<72 ;i++){
            for(int j=0; j<72 ;j++){
                for(int m=0; m<72 ;m++){
                    t=0;
                    for(int n=0; n<72 ;n++){
                        if(map_1[m][n]==RESOURCE_STONE && ts==0)
                        {
                            if((abs(m-i)+abs(n-j))>7 || isBuildingturn3(i,j,tagmap)==false)//可以适当调高
                            {
                                t=1;
                                m=72;//重新遍历
                                break;
                            }
                        }
                        if(map_1[m][n]==RESOURCE_STONE && ts==1)
                        {
                            if((abs(m-i)+abs(n-j))>11 || isBuildingturn3(i,j,tagmap)==false)//可以适当调高
                            {
                                t=1;
                                m=72;//重新遍历
                                break;
                            }
                        }
                    }
                }
                if(t==0)
                {
                    x=i;
                    y=j;
                    return 0;
                }
            }
            if(i==71 && ts==0)
            {
                i=0;
                ts=1;
            }
        }
        return -1;
    }

    int UsrAI::stone_min(int &x, int &y,int tagmap[72][72]){
        int max_1 = 1000;
        for(int i=0 ; i<72 ;i++){
            for(int j=0; j<72 ;j++){
                if(tagmap[i][j]==RESOURCE_STONE){
                    if(abs(36-i)+abs(36-j)<= max_1)
                    {
                        x=i;
                        y=j;
                        max_1=abs(36-i)+abs(36-j);
                    }
                }
            }
        }
    }

    int UsrAI::find_barn_xy(int &x, int &y,int tagmap[72][72],int type)
    {
        int map_1[72][72]={0};
        for(int i=0; i<72 ;i++){
            for(int j=0; j<72 ;j++){
                if(type==RESOURCE_BUSH)
                {
                    map_1[i][j]=tagmap[i][j];
                }
                if(type==RESOURCE_GAZELLE)
                {
                    map_1[i][j]=tagmap[i][j];
                }
            }
        }
        int t=0;//判断
        int ts=0;//兜底
        for(int i=0; i<72 ;i++){
            for(int j=0; j<72 ;j++){
                for(int m=0; m<72 ;m++){
                    t=0;
                    for(int n=0; n<72 ;n++){
                        if(map_1[m][n]==type && ts==0)
                        {
                            if((abs(m-i)+abs(n-j))>5 || isBuildingturn3(i,j,tagmap)==false)//可以适当调高
                            {
                                t=1;
                                m=72;//重新遍历
                                break;
                            }
                        }
                        if(map_1[m][n]==type && ts==1)
                        {
                            if((abs(m-i)+abs(n-j))>8 || isBuildingturn3(i,j,tagmap)==false)//可以适当调高
                            {
                                t=1;
                                m=72;//重新遍历
                                break;
                            }
                        }
                    }
                }
                if(t==0)
                {
                    x=i;
                    y=j;
                    return 0;
                }
            }
            if(i==71 && ts==0)
            {
                i=0;
                ts=1;
            }
        }
        return -1;
    }

    void UsrAI::tree_min(int &x, int &y,int tagmap[72][72]){
        int max_1 = 1000;
        for(int i=0 ; i<72 ;i++){
            for(int j=0; j<72 ;j++){
                if(tagmap[i][j]==RESOURCE_TREE){
                    if(abs(36-i)+abs(36-j)<= max_1)
                    {
                        x=i;
                        y=j;
                        max_1=abs(36-i)+abs(36-j);
                    }
                }
            }
        }
    }

    int UsrAI::find_arrowtower_xy(int &x, int &y,int tagmap[72][72], int arrowtower){
        if(isBuildingturn2(x,y+2,tagmap)==true)
        {
            y=y+2;
            return 0;
        }
        if(isBuildingturn2(x,y-3,tagmap)==true)
        {
            y=y-3;
            return 0;
        }
        if(isBuildingturn2(x+9,y,tagmap)==true)
        {
            x=x+9;
            return 0;
        }
        if(isBuildingturn2(x-9,y,tagmap)==true)
        {
            x=x-9;
            return 0;
        }
        int t=arrowtower%4;
        switch (t) {
        case 0:
            for(int i=x-9; i<x+9 ; i++){
                for(int j=y+8 ;j>y-8 ;j--){
                    if(isBuildingturn2(i,j,tagmap)==true){
                        x=i;
                        y=j;
                        return 0;
                    }
                }
            }
            break;
        case 1:
            for(int i=x+9; i>x-9 ; i--){
                for(int j=y+8 ;j>y-8 ;j--){
                    if(isBuildingturn2(i,j,tagmap)==true){
                        x=i;
                        y=j;
                        return 0;
                    }
                }
            }
            break;
        case 2:
            for(int i=x-9; i<x+9 ; i++){
                for(int j=y-8 ;j<y+8 ;j++){
                    if(isBuildingturn2(i,j,tagmap)==true){
                        x=i;
                        y=j;
                        return 0;
                    }
                }
            }
            break;
        case 3:
            for(int i=x+9; i>x-9 ; i--){
                for(int j=y-8 ;j<y+8 ;j++){
                    if(isBuildingturn2(i,j,tagmap)==true){
                        x=i;
                        y=j;
                        return 0;
                    }
                }
            }
            break;
        default:
            break;
        }
    }

    int UsrAI::find_farm_xy(int &x, int &y,int tagmap[72][72])
    {
        for(int i=0 ;i<Info.buildings.size();i++){
            if(Info.buildings[i].Type==BUILDING_CENTER)
            {
                if(isBuildingturn3(Info.buildings[i].BlockDR+4,Info.buildings[i].BlockUR,tagmap)==true)
                {
                    x=Info.buildings[i].BlockDR+4;
                    y=Info.buildings[i].BlockUR;
                    return 0;
                }
                if(isBuildingturn3(Info.buildings[i].BlockDR-4,Info.buildings[i].BlockUR,tagmap)==true)
                {
                    x=Info.buildings[i].BlockDR-4;
                    y=Info.buildings[i].BlockUR;
                    return 0;
                }
            }
            if(Info.buildings[i].Type==BUILDING_GRANARY)
            {
                for(int j=-3; j<=3 ;j++){
                    if(isBuildingturn3(Info.buildings[i].BlockDR+4,Info.buildings[i].BlockUR+j,tagmap)==true)
                    {
                        x=Info.buildings[i].BlockDR+4;
                        y=Info.buildings[i].BlockUR+j;
                        return 0;
                    }
                }
                for(int j=-3; j<=3 ;j++){
                    if(isBuildingturn3(Info.buildings[i].BlockDR-4,Info.buildings[i].BlockUR+j,tagmap)==true)
                    {
                        x=Info.buildings[i].BlockDR-4;
                        y=Info.buildings[i].BlockUR+j;
                        return 0;
                    }
                }
                for(int j=-3; j<=3 ;j++){
                    if(isBuildingturn3(Info.buildings[i].BlockDR+j,Info.buildings[i].BlockUR+4,tagmap)==true)
                    {
                        x=Info.buildings[i].BlockDR+j;
                        y=Info.buildings[i].BlockUR+4;
                        return 0;
                    }
                }
                for(int j=-3; j<=3 ;j++){
                    if(isBuildingturn3(Info.buildings[i].BlockDR+j,Info.buildings[i].BlockUR-4,tagmap)==true)
                    {
                        x=Info.buildings[i].BlockDR+j;
                        y=Info.buildings[i].BlockUR-4;
                        return 0;
                    }
                }
            }
        }
    }
=======

#include <QDebug>

static int gamemap[72][72]={0};

static int bestzb[72*72][2]={0};

static int bushsn[6]={0};//灌木丛信息
static bool getbushsn=0;

static int gucang=0;//谷仓信息

static bool jianta=0;//箭塔是否被触发

//仓库信息
static bool ck_t=0;//树
static int x_ck=0;
static int y_ck=0;

static int ck=0;//石头
static double ck_x=0;
static double ck_y=0;

static int tempsn=0;

//建筑坐标
static int kk=1;
static int firsthome[4][2]={{32,40},{40,39},{39,31},{31,32}};
static int firsthomefinish=0;
static int jtzb[8][2]={{33,35},{33,37},{35,38},{37,38},{38,36},{38,34},{36,33},{34,33}};
static int sechome[12][2]={{34,42},{36,42+kk},{38,41},{42,37},{42+kk,35},{41,33},{37,29},{35,28+kk},{33,30},{29,34},{28+kk,36},{30,38}};

//阶段信息
static bool woodenough=0;
static bool meatenough=0;
static bool stoneenough=0;

//石头信息
static bool getstonesn=0;
static int stonesn[10]={0};
static int stonei=0;
long long frame=0;
tagInfo myInFo;

void UsrAI::processData()
{
    /*
    //写入你的代码
    myInFo=getInfo();

    frame++;
    //遍历地图
    for(int i=0;i<72;i++)
    {
        for(int j=0;j<72;j++)
        {
            gamemap[i][j]=myInFo.theMap[i][j];
        }
    }

    //遍历资源，石头9，其他7
    for(tagResource resource:getInfo().resources)
    {
        if(resource.Type==RESOURCE_STONE)
        {
            for(int i=0;i<2;i++)
            {
                for(int j=0;j<2;j++)
                {
                    gamemap[resource.BlockDR+i][resource.BlockUR+j]=9;
                }
            }
        }
        else
        {
            gamemap[resource.BlockDR][resource.BlockUR]=7;
        }
    }

    //遍历建筑，箭塔17，仓库18，其他19
    for(tagBuilding building:getInfo().buildings)
    {
        //给建筑加标签
        if(building.Type==BUILDING_HOME || building.Type==BUILDING_ARROWTOWER)
        {
            for(int i=0;i<2;i++)
            {
                for(int j=0;j<2;j++)
                {
                    gamemap[building.BlockDR+i][building.BlockUR+j]=17;
                }
            }
        }
        else if(building.Type==BUILDING_STOCK)
        {
            for(int i=0;i<3;i++)
            {
                for(int j=0;j<3;j++)
                {
                    gamemap[building.BlockDR+i][building.BlockUR+j]=18;
                }
            }
        }
        else
        {
            for(int i=0;i<3;i++)
            {
                for(int j=0;j<3;j++)
                {
                    gamemap[building.BlockDR+i][building.BlockUR+j]=19;
                }
            }
        }
        //谷仓
        if(building.Type==BUILDING_GRANARY && building.Percent==100)
        {
            //研究箭塔
            if(!jianta)
            {
                BuildingAction(building.SN,BUILDING_GRANARY_ARROWTOWER);
                jianta=1;
            }
        }
        //市镇中心
        else if(building.Type==BUILDING_CENTER)
        {
            //造小人
            if(building.Project==ACT_NULL && myInFo.farmers.size()<16)
            {
                BuildingAction(building.SN,BUILDING_CENTER_CREATEFARMER);
            }
        }
        //仓库
        else if(building.Type==3)
        {
            if(!ck_t)
            {
                x_ck=building.BlockDR+1;
                y_ck=building.BlockUR+1;
                ck_t=1;
            }
        }
        //箭塔
        else if(building.Type==BUILDING_ARROWTOWER && building.Project==ACT_NULL && !(getInfo().GameFrame%10))
        {
            for(tagArmy enemyarmy:getInfo().enemy_armies)
            {
                int tempdis=getdis(building.BlockDR,building.BlockUR,enemyarmy.BlockDR,enemyarmy.BlockUR);
                if(tempdis<80)
                {
                    tempsn=enemyarmy.SN;
                    HumanAction(building.SN,tempsn);
                    break;
                }
            }
        }
    }

    //遍历单位
    //农民，5
    for(tagFarmer farmer:getInfo().farmers)
    {
        gamemap[farmer.BlockDR][farmer.BlockUR]=5;
    }

    //得到bush的sn
    if(!getbushsn)
    {
        int bushi=0;
        for(tagResource resource :getInfo().resources)
        {
            if(resource.Type==RESOURCE_BUSH)
            {
                bushsn[bushi]=resource.SN;
                bushi++;
            }
        }
        getbushsn=1;
    }

    //得到石头sn
    if(!getstonesn && ck==1)
    {

        for(tagResource resource :getInfo().resources)
        {
            int x=resource.BlockDR;
            int y=resource.BlockUR;
            bool is=0;
            for(int ii=-7;ii<=7;ii++)
            {
                for(int jj=-7;jj<=7;jj++)
                {
                    if(gamemap[x+ii][y+jj]==18)
                    {
                        is=1;
                    }
                }
            }
            if(is && resource.Type==RESOURCE_STONE && ((gamemap[x+2][y]>=0&&gamemap[x+2][y]<=4) || (gamemap[x-1][y]>=0&&gamemap[x-1][y]<=4) || (gamemap[x][y+2]>=0&&gamemap[x][y+2]<=4) || (gamemap[x][y-1]>=0&&gamemap[x][y-1]<=4)))
            {
                stonesn[stonei]=resource.SN;
                stonei++;
                qDebug()<<stonei;
            }
        }
        getstonesn=1;
    }
    for(unsigned int i=0;i<myInFo.farmers.size();i++)
    {
        //判断木头
        if(myInFo.Wood>=360 && !woodenough)
        {
            woodenough=1;
            for(unsigned int i=0;i<myInFo.farmers.size();i++)
            {
                if(i==0||i==2||(i>=8&&i<=11))
                {
                    myInFo.farmers[i].NowState=0;
                }
            }
        }
        //判断石头
        if(myInFo.Stone>=1200 && !stoneenough)
        {
            stoneenough=1;
            qDebug()<<stoneenough;
            for(unsigned int i=0;i<myInFo.farmers.size();i++)
            {
                myInFo.farmers[i].NowState=0;
            }
        }
        //判断食物
        if(myInFo.farmers.size()>=16 && !meatenough && myInFo.Meat>=100)
        {
            meatenough=1;
            for(unsigned int i=0;i<myInFo.farmers.size();i++)
            {
                if(i==1||(i>=3&&i<=7))
                {
                    myInFo.farmers[i].NowState=0;
                }
            }
        }

        //木头不够，6人砍树，四人仓库
        if(!woodenough)
        {
            // 1 firsthome,砍树
            if(i==0 && (myInFo.farmers[i].NowState==0||myInFo.farmers[i].NowState==11))
            {
                bool isst=0;
                for(tagResource resource:getInfo().resources)
                {
                    if(resource.Type==RESOURCE_TREE && resource.Blood==BLOOD_TREE && ((resource.BlockDR-36)*(resource.BlockDR-36)+(resource.BlockUR-36)*(resource.BlockUR-36))<=72)
                    {
                        isst=1;
                    }
                }
                if(firsthomefinish<3 && myInFo.Wood>BUILD_HOUSE_WOOD)
                {
                    for(int ii=0;ii<4;ii++)
                    {
                        int x=firsthome[ii][0];
                        int y=firsthome[ii][1];
                        if(homevale(x,y,2,2))
                        {
                            HumanBuild(myInFo.farmers[i].SN,BUILDING_HOME,x,y);
                            firsthomefinish++;
                            break;
                        }
                    }
                }
                else if(isst)
                {
                    if(myInFo.farmers[i].NowState==0 || myInFo.farmers[i].NowState==11)
                    {
                        for(tagResource resource:getInfo().resources)
                        {
                            if(resource.Type==RESOURCE_TREE && ((resource.BlockDR-36)*(resource.BlockDR-36)+(resource.BlockUR-36)*(resource.BlockUR-36))<=72)
                            {
                                HumanAction(myInFo.farmers[i].SN,resource.SN);
                            }
                        }
                    }
                }
                else if(firsthomefinish<4)
                {
                    for(int ii=0;ii<4;ii++)
                    {
                        int x=firsthome[ii][0];
                        int y=firsthome[ii][1];
                        if(homevale(x,y,2,2))
                        {
                            HumanBuild(myInFo.farmers[i].SN,BUILDING_HOME,x,y);
                            firsthomefinish++;
                            break;
                        }
                    }
                }
                else
                {
                    get_ck_tree(i);
                }

            }

            // 3 砍附近树，砍树
            else if(i==2&& (myInFo.farmers[i].NowState==0||myInFo.farmers[i].NowState==11))
            {
                if(issignaltree(i))
                {

                }
                else
                {
                    get_ck_tree(i);
                }
            }

            // 9,10,11,12砍树
            else if(i>=8&&i<=11&& (myInFo.farmers[i].NowState==0||myInFo.farmers[i].NowState==11))
            {
                if(issignaltree(i))
                {

                }
                else
                {
                    get_ck_tree(i);
                }
            }

            // 13,14,15,16盖第一个仓库,石头
            else if((i>=12) && (myInFo.farmers[i].NowState==0||myInFo.farmers[i].NowState==11))
            {
                bool can=0;
                for(tagResource resource:getInfo().resources)
                {
                    if(resource.SN==stonesn[i-12] && resource.Cnt>0)
                    {
                        can=1;
                    }
                }
                if(ck==0)
                {
                    int d=72*72;
                    for(tagResource resource:getInfo().resources)
                    {
                        if(resource.Type==RESOURCE_STONE)
                        {
                            int tempd=getdis(resource.BlockDR,resource.BlockUR,36,36);
                            if(tempd<d)
                            {
                                d=tempd;
                                tempsn=resource.SN;
                            }
                        }
                    }
                    int ck_zb=0;
                    for(tagResource resource:getInfo().resources)
                    {
                        if(resource.SN==tempsn)
                        {
                            ck_zb=getbestzb(3,resource.BlockDR,resource.BlockUR);
                        }
                    }
                    HumanBuild(myInFo.farmers[i].SN,BUILDING_STOCK,bestzb[ck_zb][0],bestzb[ck_zb][1]);
                    ck_x=bestzb[ck_zb][0];
                    ck_y=bestzb[ck_zb][1];
                    ck++;
                }

                else if(nobuildfinish(BUILDING_STOCK))
                {
                    HumanAction(myInFo.farmers[i].SN,tempsn);
                }
                else if(can)
                {
                    HumanAction(myInFo.farmers[i].SN,stonesn[i-12]);
                }
                else
                {
                    getstone_far(i);
                }
            }
        }

        //木头够了石头不够
        if(woodenough && !stoneenough)
        {
            // 1,3,9,10,11,12收集石头
            if(((i>=8&&i<=11)) && (myInFo.farmers[i].NowState==0||myInFo.farmers[i].NowState==11))
            {
                bool can=0;
                for(tagResource resource:getInfo().resources)
                {
                    if(resource.SN==stonesn[i-9] && resource.Cnt>0)
                    {
                        can=1;
                    }
                }
                if(can)
                {
                    HumanAction(myInFo.farmers[i].SN,stonesn[i-9]);
                }
                else
                {
                    getstone_ck(i);
                }
            }
            // 1,3帮13
            else if((i==0||i==2) && (myInFo.farmers[i].NowState==0||myInFo.farmers[i].NowState==11))
            {
                help(13);
                HumanAction(myInFo.farmers[i].SN,tempsn);
            }
            else if((i>=12) && (myInFo.farmers[i].NowState==0||myInFo.farmers[i].NowState==11))
            {
                getstone_far(i);
            }
        }

        //食物不够，六人浆果
        if(!meatenough)
        {
            // 2 盖谷仓,采集浆果
            if(i==1 && (myInFo.farmers[i].NowState==0||myInFo.farmers[i].NowState==11))
            {
                if(gucang!=1)
                {
                    int gc_zb=getbestzb(3,getbestx(20),getbesty(20));
                    int x=bestzb[gc_zb][0];
                    int y=bestzb[gc_zb][1];
                    HumanBuild(myInFo.farmers[i].SN,BUILDING_GRANARY,x,y);
                    gucang++;
                }
                else if(isbush(0))
                {
                    HumanAction(myInFo.farmers[i].SN,bushsn[0]);
                }
            }

            // 4,5,6,7,8,浆果
            else if(i>=3&&i<=7 && (myInFo.farmers[i].NowState==0||myInFo.farmers[i].NowState==11))
            {
                if(isbush(i-2))
                {
                    HumanAction(myInFo.farmers[i].SN,bushsn[i-2]);
                }
            }
        }

        //食物够了
        if(meatenough && !stoneenough)
        {
            // 2，4，5，6，7，8石头
            if((i==1||(i>=3&&i<=7)) && (myInFo.farmers[i].NowState==0||myInFo.farmers[i].NowState==11))
            {
                bool can=0;
                for(tagResource resource:getInfo().resources)
                {
                    if(resource.SN==stonesn[i+stonei-7] && resource.Cnt>0)
                    {
                        can=1;
                    }
                }
                if(can)
                {
                    HumanAction(myInFo.farmers[i].SN,stonesn[stonei+i-7]);
                }
                else
                {
                    getstone_far(i);
                }
            }
        }

        //石头够了，8盖箭塔，4帮忙，4盖墙
        if(stoneenough)
        {
            // 1，2，3，4，5，6，7，8盖箭塔
            if(i<=7 && (myInFo.farmers[i].NowState==0||myInFo.farmers[i].NowState==11))
            {
                if(homevale(jtzb[i][0],jtzb[i][1],2,2) && myInFo.Stone>=BUILD_ARROWTOWER_STONE)
                {
                    HumanBuild(myInFo.farmers[i].SN,BUILDING_ARROWTOWER,jtzb[i][0],jtzb[i][1]);
                }
                else if(nobuildfinish(BUILDING_ARROWTOWER))
                {
                    HumanAction(myInFo.farmers[i].SN,tempsn);
                }
                else if(nomaxblood(i))
                {
                    HumanAction(myInFo.farmers[i].SN,tempsn);
                }
            }

            // 13,14,15,16帮着盖箭塔
            else if((i>=12 && i<=15) && (myInFo.farmers[i].NowState==0||myInFo.farmers[i].NowState==11||myInFo.farmers[i].NowState==3))
            {
                if(nomaxblood(i))
                {
                    HumanAction(myInFo.farmers[i].SN,tempsn);
                }
                else
                {
                    help((i-12)*2);
                    HumanAction(myInFo.farmers[i].SN,tempsn);
                }
            }
            // 8,9,10,11筑墙
            else if((i>=8 && i<=11) && (myInFo.farmers[i].NowState==0||myInFo.farmers[i].NowState==11))
            {
                if(homevale(sechome[(i-8)*3][0],sechome[(i-8)*3][1],2,2))
                {
                    HumanBuild(myInFo.farmers[i].SN,BUILDING_HOME,sechome[(i-8)*3][0],sechome[(i-8)*3][1]);
                }
                else if(homevale(sechome[(i-8)*3+1][0],sechome[(i-8)*3+1][1],2,2))
                {
                    HumanBuild(myInFo.farmers[i].SN,BUILDING_HOME,sechome[(i-8)*3+1][0],sechome[(i-8)*3+1][1]);
                }
                else if(homevale(sechome[(i-8)*3+2][0],sechome[(i-8)*3+2][1],2,2))
                {
                    HumanBuild(myInFo.farmers[i].SN,BUILDING_HOME,sechome[(i-8)*3+2][0],sechome[(i-8)*3+2][1]);
                }
                else if(nomaxblood(i))
                {
                    HumanAction(myInFo.farmers[i].SN,tempsn);
                }
                else
                {
                    help((i-8)*2+1);
                    HumanAction(myInFo.farmers[i].SN,tempsn);
                }
            }
        }
    }

    //打印地图
    std::ofstream outFile("PrintMap.txt");
    if (outFile.is_open())
    {
        for (int i = 0; i<72; i++)
        {
            for (int j = 0; j < 72; j++)
            {
                outFile << gamemap[i][j]<<' ';
            }
            outFile << std::endl;
        }
        outFile.close();
    }
    */
}

int UsrAI::getdis(int x, int y, int xx, int yy)
{
    int d=(x-xx)*(x-xx)+(y-yy)*(y-yy);
    return d;
}

bool UsrAI::isbush(int i)
{
    for(tagResource resource :getInfo().resources)
    {
        if(resource.SN==bushsn[i]&&resource.Blood==0)
        {
            return 0;
        }
    }
    return 1;
}

bool UsrAI::issignaltree(int i)
{
    for(tagResource resource:getInfo().resources)
    {
        if(resource.Type==RESOURCE_TREE && resource.Blood==BLOOD_TREE && ((resource.BlockDR-36)*(resource.BlockDR-36)+(resource.BlockUR-36)*(resource.BlockUR-36))<=72)
        {
            HumanAction(myInFo.farmers[i].SN,resource.SN);
            return 1;
        }
    }
    return 0;
}

bool UsrAI::homevale(int x, int y,int a,int b)
{
    int zb=gamemap[x][y];
    if(zb<0 || zb>4)
    {
        return 0;
    }
    for(int i=0;i<a;i++)
    {
        for(int j=0;j<b;j++)
        {
            if(gamemap[x+i][y+j]!=zb)
            {
                return 0;
            }
        }
    }
    return 1;
}

int UsrAI::getbestzb(int l,int x,int y)
{
    for(int i=0;i<72*72;i++)
    {
        for(int j=0;j<2;j++)
        {
            bestzb[i][j]=72;
        }
    }
    int n=0;
    for(int i=0;i<72;i++)
    {
        for(int j=0;j<72;j++)
        {
            if(homevale(i,j,l,l) && gamemap[i-1][y-1]!=9 && gamemap[i-1][y+1]!=9 && gamemap[i+1][y-1]!=9 && gamemap[i+1][y+1]!=9 && gamemap[i-1][y]!=9 && gamemap[i+1][y]!=9 && gamemap[i][y+1]!=9 && gamemap[i][y-1]!=9)
            {
                bestzb[n][0]=i;
                bestzb[n][1]=j;
                n++;
            }
        }
    }
    int count=0;
    int dis=72*72;
    for(int i=0;i<72*72;i++)
    {
        int temp=(bestzb[i][0]-x)*(bestzb[i][0]-x)+(bestzb[i][1]-y)*(bestzb[i][1]-y);
        if(temp<dis)
        {
            dis=temp;
            count=i;
        }
    }
    return count;
}

int UsrAI::getbestx(int x)
{
    int xx=0;
    int num=0;
    int firstx=0;
    for(tagResource resource:getInfo().resources)
    {
        if(resource.Type==x)
        {
            firstx=resource.BlockDR;
            break;
        }
    }
    for(tagResource resource:getInfo().resources)
    {
        if(resource.Type==x && ((resource.BlockDR-firstx)>-4 && (resource.BlockDR-firstx)<4))
        {
            xx+=resource.BlockDR;
            num++;
        }
    }
    if(num==0){
        qWarning()<<"num==0";
        return 0;
    }
    return xx/num;
}

int UsrAI::getbesty(int x)
{
    int xx=0;
    int num=0;
    int firstx=0;
    for(tagResource resource:getInfo().resources)
    {
        if(resource.Type==x)
        {
            firstx=resource.BlockUR;
            break;
        }
    }
    for(tagResource resource:getInfo().resources)
    {
        if(resource.Type==x && ((resource.BlockUR-firstx)>-4 && (resource.BlockUR-firstx)<4))
        {
            xx+=resource.BlockUR;
            num++;
        }
    }
    if(num==0){
        qWarning()<<"num==0";
        return 0;
    }

    return xx/num;
}

void UsrAI::get_ck_tree(int i)
{
    int t_dis=81;
    int i_sn=0;
    for(tagResource resource:getInfo().resources)
    {
        if(resource.Type==RESOURCE_TREE && resource.Blood==BLOOD_TREE)
        {
            int x=resource.BlockDR;
            int y=resource.BlockUR;
            int temp=(resource.BlockDR-x_ck)*(resource.BlockDR-x_ck)+(resource.BlockUR-y_ck)*(resource.BlockUR-y_ck);
            if(temp<t_dis && (gamemap[x-1][y]<=4||gamemap[x-1][y]>=0) && (gamemap[x-1][y-1]<=4||gamemap[x-1][y-1]>=0) &&
                (gamemap[x][y-1]<=4||gamemap[x][y-1]>=0) && (gamemap[x][y+2]<=4||gamemap[x][y+2]>=0) &&
                (gamemap[x+1][y-1]<=4||gamemap[x+1][y-1]>=0) && (gamemap[x+1][y+2]<=4||gamemap[x+1][y+2]>=0) &&
                (gamemap[x+2][y]<=4||gamemap[x+2][y]>=0) && (gamemap[x+2][y+1]<=4||gamemap[x=2][y+1]>=0))
            {
                t_dis=temp;
                i_sn=resource.SN;
            }
        }
    }
    HumanAction(myInFo.farmers[i].SN,i_sn);
}

bool UsrAI::nobuildfinish(int i)
{
    for(tagBuilding building:getInfo().buildings)
    {
        if(building.Type==i && building.Percent<100)
        {
            tempsn=building.SN;
            return 1;
        }
    }
    return 0;
}

void UsrAI::getstone_ck(int i)
{
    int t_dis=81;
    int i_sn=0;
    for(tagResource resource:getInfo().resources)
    {
        if(resource.Type==RESOURCE_STONE)
        {
            int temp=(resource.BlockDR-ck_x)*(resource.BlockDR-ck_x)+(resource.BlockUR-ck_y)*(resource.BlockUR-ck_y);
            if(temp<t_dis)
            {
                t_dis=temp;
                i_sn=resource.SN;
            }
        }
    }
    HumanAction(myInFo.farmers[i].SN,i_sn);
}

void UsrAI::getstone_far(int i)
{
    double t_dis=1e6;
    int i_sn=0;
    for(tagResource resource:getInfo().resources)
    {
        if(resource.Type==RESOURCE_STONE)
        {
            double temp=calDistance(resource.DR,resource.UR,myInFo.farmers[i].DR,myInFo.farmers[i].UR);
            if(temp<t_dis)
            {
                t_dis=temp;
                i_sn=resource.SN;
            }
        }
    }
    HumanAction(myInFo.farmers[i].SN,i_sn);
}

bool UsrAI::nomaxblood(int i)
{
    for(tagBuilding building:getInfo().buildings)
    {
        if(building.Type==BUILDING_ARROWTOWER && getdis(building.BlockDR,building.BlockUR,myInFo.farmers[i].BlockDR,myInFo.farmers[i].BlockUR)<4 && building.Blood!=BLOOD_BUILD_ARROWTOWER)
        {
            tempsn=building.SN;
            return 1;
        }
    }
    return 0;
}

void UsrAI::help(int i)
{
    if(myInFo.farmers[i].NowState!=0 && myInFo.farmers[i].NowState!=11)
    {
        tempsn=myInFo.farmers[i].WorkObjectSN;
    }
}
