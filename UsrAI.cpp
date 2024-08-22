#include "UsrAI.h"
tagGame tagUsrGame;
ins UsrIns;
/*##########请勿修改以上部分##########*/
//#include<QFile>
tagInfo myinfo;
//阶段
static int stage=0;
//军队的变量
int clubman=0;
int slinger=0;
int bowman=0;
int scout=0;
static int chasinglock[20]={0};
static int Blood[20]={0};
static int bagenerate=1;
static double chasestart_L[50] = {0};
static double chasestart_U[50] = {0};
//村民的变量
static int bloodfarmer[20];
static int bfgenerate=1;
//打印返回的命令
static int timers = 1;
int id_Wrong_lastFrame = -1;
//临时量
int ausechange;
//记录建筑情况
int amountofgranary=0;
int amountofcamp=0;
int amountofstock=0;
int amountofmarket=0;
int amountofstable=0;
int amountofrange=0;
static int yesupgradearrowtower=0;
static int yesupgradearrowtowerstart=0;
static int yesbupgrade=0;
int house=0;
int tower=0;
int farm=0;
//记录资源位置
double average_bush_DR;
double average_bush_UR;
double average_tree_DR;
double average_tree_UR;
double average_stone_DR;
double average_stone_UR;
int resourcestonecount;
int resourcebushcount;
int resourcetreecount;
//记录资源是否还存在
static int thisresisnone=1;
//记录各类建筑的建造位置
static int x_tofind33x=35;
static int y_tofind33x=32;
static int x_arrowtower=32;
static int y_arrowtower=30;
static int x_arrowtower_build=32;
static int y_arrowtower_build=30;
int x_wall_arrowtower[16]={34,32,30,29,29,30,32,34,37,39,41,42,42,41,39,37};
int y_wall_arrowtower[16]={29,30,32,34,37,39,41,42,42,41,39,37,34,32,30,29};
int wall_arrowtower[16]={0};
static int x_granary;
static int y_granary;
static int x_stock;
static int y_stock;
static int x_stock1position;
static int y_stock1position;
static int x_stock2position;
static int y_stock2position;
static int x_home=33;
static int y_home=33;
//记录将要操作的对象的SN
static int SN_todo;
//count延时时钟
static int count_clock=0;
static int count_clock_arrowtower=250;
double mid=36*BLOCKSIDELENGTH;
tagHuman mintomax_blood[1000];//存储敌人血量高低的数组

//cheattocheck
static int cheatresyes = 0;
static int cheatactionyes = 0;
static int cheatattackwave = 0;
//调试
static int map33cout = 1;
static int map22cout = 1;
static int mapcout = 1;





void UsrAI::getdistance(int the_thing_to_get,int Type_of_thing,double x_start,double y_start){//1为收集的资源，2为指定的单位
    double dis=1e6;
    double temp_dis;
    int max_temp_blood=0;
    int Type2out=0;
    SN_todo=-1;
    switch (Type_of_thing) {
    case 1://收集资源
        thisresisnone=1;
        for(int j = 0 ; j<myinfo.resources.size() ; j++){
            if(myinfo.resources[j].Type==the_thing_to_get){
                thisresisnone=0;
                temp_dis = calDistance(x_start,y_start,myinfo.resources[j].DR, myinfo.resources[j].UR);
                if(temp_dis<dis)
                {
                    SN_todo = myinfo.resources[j].SN;
                    dis = temp_dis;
                }
            }
        }
        break;
    case 2://检索敌人
        for(int i = 0;i < 1000;i++){
            mintomax_blood[i].SN=0;
            mintomax_blood[i].Blood=0;
            mintomax_blood[i].DR=0;
            mintomax_blood[i].UR=0;
        }
    /*****计算最小血量的敌人（存储至mintomax_blood数组中）*****/

        if(myinfo.enemy_armies.size()){
            for(int i = 0;i<myinfo.enemy_armies.size();i++){
                mintomax_blood[i].Blood=myinfo.enemy_armies[i].Blood;
                mintomax_blood[i].SN=myinfo.enemy_armies[i].SN;
                mintomax_blood[i].DR=(myinfo.enemy_armies[i].DR)/BLOCKSIDELENGTH;
                mintomax_blood[i].UR=(myinfo.enemy_armies[i].UR)/BLOCKSIDELENGTH;
                max_temp_blood=i+1;
            }
            for(int i = 0;i < max_temp_blood-1;i++){
                for(int j = i+1;j < max_temp_blood;j++){
                    if(mintomax_blood[i].Blood>mintomax_blood[j].Blood){
                        tagHuman cun;
                        cun=mintomax_blood[i];
                        mintomax_blood[i]=mintomax_blood[j];
                        mintomax_blood[j]=cun;
                    }
                }

            }
        }
        for(int j = 0 ; j<myinfo.enemy_armies.size() ; j++){
            temp_dis = calDistance(x_start,y_start,mintomax_blood[j].DR, mintomax_blood[j].UR);
            if(the_thing_to_get==BUILDING_ARROWTOWER){
                if(temp_dis<8){
                    SN_todo=mintomax_blood[j].SN;
                    Type2out=1;
                }
            }
            else{
                if(temp_dis<dis){
                    SN_todo=mintomax_blood[j].SN;
                    Type2out=1;
                }
            }
            if(Type2out){
                break;
            }
        }
        break;
    default:
        break;
    }
}

void UsrAI::generatebacktobuild()
{
    bfgenerate=1;
    bagenerate=1;
    x_tofind33x=35;
    y_tofind33x=34;
    x_home=33;
    y_home=33;

}

void UsrAI::positionofresource(int typeofresource)
{


    if(typeofresource==RESOURCE_BUSH){
        average_bush_DR=0;
        average_bush_UR=0;
        resourcebushcount=0;
        for(int i = 0;i < myinfo.resources.size();i++){
            if(myinfo.resources[i].Type==RESOURCE_BUSH){
                average_bush_DR+=myinfo.resources[i].DR;
                average_bush_UR+=myinfo.resources[i].UR;
                resourcebushcount++;
            }
        }
        average_bush_DR/=BLOCKSIDELENGTH;
        average_bush_UR/=BLOCKSIDELENGTH;
        average_bush_DR/=resourcebushcount;
        average_bush_UR/=resourcebushcount;
        average_bush_DR--;
        average_bush_UR--;
    }
    if(typeofresource==RESOURCE_TREE){
        average_tree_DR=0;
        average_tree_UR=0;
        resourcetreecount=0;
        for(int i = 0;i < myinfo.resources.size();i++){
            if(myinfo.resources[i].Type==RESOURCE_TREE){
                average_tree_DR+=myinfo.resources[i].DR;
                average_tree_UR+=myinfo.resources[i].UR;
                resourcetreecount++;
            }
        }
        average_tree_DR/=BLOCKSIDELENGTH;
        average_tree_UR/=BLOCKSIDELENGTH;
        average_tree_DR/=resourcetreecount;
        average_tree_UR/=resourcetreecount;
        average_tree_DR--;
        average_tree_UR--;
    }
    if(typeofresource==RESOURCE_STONE){
        average_stone_DR=0;
        average_stone_UR=0;
        resourcestonecount=0;
        for(int i = 0;i < myinfo.resources.size();i++){
            if(myinfo.resources[i].Type==RESOURCE_STONE){
                average_stone_DR+=myinfo.resources[i].DR;
                average_stone_UR+=myinfo.resources[i].UR;
                resourcestonecount++;
            }
        }
        average_stone_DR/=BLOCKSIDELENGTH;
        average_stone_UR/=BLOCKSIDELENGTH;
        average_stone_DR/=resourcestonecount;
        average_stone_UR/=resourcestonecount;
        average_stone_DR--;
        average_stone_UR--;
    }
}

void UsrAI::countbuilding()
{
    for(tagBuilding building : myinfo.buildings){
        if(building.Type==BUILDING_ARROWTOWER&&building.Percent==100){
            tower++;
        }
        if(building.Type==BUILDING_MARKET&&building.Percent==100){
            amountofmarket++;
        }
        if(building.Type==BUILDING_STOCK&&building.Percent==100){
            amountofstock++;
        }
        if(building.Type==BUILDING_STABLE&&building.Percent==100){
            amountofstable++;
        }
        if(building.Type==BUILDING_RANGE&&building.Percent==100){
            amountofrange++;
        }
        if(building.Type==BUILDING_ARMYCAMP&&building.Percent==100){
            amountofcamp++;
        }
        if(building.Type==BUILDING_GRANARY&&building.Percent==100){
            amountofgranary++;
        }
        if(building.Type==BUILDING_FARM&&building.Percent==100){
            farm++;
        }
        if(building.Type==BUILDING_HOME&&building.Percent==100){
            house++;
        }
    }
}

void UsrAI::countarmies()
{
    for(int i = 0;i < myinfo.armies.size();i++){
        if(myinfo.armies[i].Sort==AT_CLUBMAN){//近战棍棒
            clubman++;
        }
        if(myinfo.armies[i].Sort==AT_SLINGER){//投石
            slinger++;
        }
        if(myinfo.armies[i].Sort==AT_BOWMAN){//弓箭手
            bowman++;
        }
        if(myinfo.armies[i].Sort==AT_SCOUT){//骑兵
            scout++;
        }
    }
}

void UsrAI::processData()
{
/*****调试内容*****/{
    if(cheatactionyes){
        cheatAction();
        cheatactionyes--;
    }
    if(cheatresyes){
        cheatRes();
        cheatresyes=0;
    }
    if(cheatattackwave){
        cheatAttack(cheatattackwave);
        cheatattackwave=0;
    }





    }
/*****各种量的读入和初始化*****/
    int order_Record;
    myinfo = getInfo();
    amountofgranary=0;
    amountofcamp=0;
    amountofstock=0;
    house = 0;//计算房屋数量
    tower=0;
    farm=0;
    clubman=0;
    slinger=0;
    bowman=0;
    scout=0;
    count_clock++;//每帧时钟加一
    if(count_clock==10000){
        count_clock=0;
    }
    //DebugText(count_clock);
    if(yesupgradearrowtowerstart){
        count_clock_arrowtower--;
        if(count_clock_arrowtower==0){
            yesupgradearrowtower=1;
        }
    }





/*****遍历地图并存储合适的建筑位置*****/
    //动态地图
    int dynamic_map[72][72]={0};
    int leftposition=0;
    int leftpositionflag;
    int map33[70][70]={0};
    int map22[71][71]={0};
    for(int i=0;i<72;i++)
    {
        for(int j=0;j<72;j++)
        {
            dynamic_map[i][j]=myinfo.theMap[i][j];
            dynamic_map[i][j]+='0';
        }
    }
    for(tagBuilding building : myinfo.buildings){
        if(building.Type==BUILDING_HOME||building.Type==BUILDING_ARROWTOWER){
            dynamic_map[building.BlockDR][building.BlockUR]=('A'+building.Type);
            dynamic_map[building.BlockDR+1][building.BlockUR]=('A'+building.Type);
            dynamic_map[building.BlockDR][building.BlockUR+1]=('A'+building.Type);
            dynamic_map[building.BlockDR+1][building.BlockUR+1]=('A'+building.Type);
        }
        else{
            for(int i = 0;i < 3;i++){
                for(int j = 0;j < 3;j++){
                    dynamic_map[building.BlockDR+i][building.BlockUR+j]=('A'+building.Type);
                }
            }
        }
    }
    for(tagHuman farmers : myinfo.farmers){
        dynamic_map[farmers.BlockDR][farmers.BlockUR]=('*');
    }
    for(tagResource resource : myinfo.resources){
        if(resource.Type==RESOURCE_STONE){
            dynamic_map[resource.BlockDR][resource.BlockUR]=('A'+resource.Type);
            dynamic_map[resource.BlockDR+1][resource.BlockUR]=('A'+resource.Type);
            dynamic_map[resource.BlockDR][resource.BlockUR+1]=('A'+resource.Type);
            dynamic_map[resource.BlockDR+1][resource.BlockUR+1]=('A'+resource.Type);
        }
        else{
            dynamic_map[resource.BlockDR][resource.BlockUR]=('A'+resource.Type);
        }
    }
    for(int i = 0;i < 71;i++){
        for(int j = 0;j < 71;j++){
            leftposition=dynamic_map[i][j];
            if(leftposition>='A'){
                continue;
            }
            leftpositionflag=0;
            for(int k = 0;k < 2;k++){
                for(int l = 0;l < 2;l++){
                    if(leftposition!=dynamic_map[i+k][j+l]){
                        leftpositionflag=1;
                    }
                    if(leftpositionflag){
                        break;
                    }
                }
            }
            if(!leftpositionflag){
                map22[i][j]=1;
            }
        }
    }
    for(int i = 0;i < 70;i++){
        for(int j = 0;j < 70;j++){
            leftposition=dynamic_map[i][j];
            if(leftposition>='A'){
                continue;
            }
            leftpositionflag=0;
            for(int k = 0;k < 3;k++){
                for(int l = 0;l < 3;l++){
                    if(leftposition!=dynamic_map[i+k][j+l]){
                        leftpositionflag=1;
                    }
                    if(leftpositionflag){
                        break;
                    }
                }
            }
            if(!leftpositionflag){
                map33[i][j]=1;
            }
        }
    }

/*****地图调试*****/{
    if(map33cout){//存入33地图文件
        ofstream outfile;
        outfile.open("E:\\qt_code\\map33txt\\map33.txt");
        for(int i = 0;i < 70;i++){
            for(int j = 0;j < 70;j++){
                outfile<<map33[i][j]<<" ";
            }
         outfile<<endl;
        }

    }
    if(map22cout){//存入33地图文件
        ofstream outfile;
        outfile.open("E:\\qt_code\\map33txt\\map22.txt");
        for(int i = 0;i < 71;i++){
            for(int j = 0;j < 71;j++){
                outfile<<map22[i][j]<<" ";
            }
         outfile<<endl;
        }

    }
    if(mapcout){//存入33地图文件
        ofstream outfile;
        outfile.open("E:\\qt_code\\map33txt\\dynamic_map.txt");
        for(int i = 0;i < 72;i++){
            for(int j = 0;j < 72;j++){
                char c=dynamic_map[i][j];
                outfile<<c<<" ";
            }
         outfile<<endl;
        }
    }
    }
/*****记录命令*****/{
    if(id_Wrong_lastFrame>=0&&timers>0) //记录了上一帧下达命令的编号
       {
          order_Record = myinfo.ins_ret[id_Wrong_lastFrame];  //获取上一帧下达命令的结果
         DebugText( ("命令"+ QString::number(id_Wrong_lastFrame) +"的返回为"+ QString::number(order_Record)).toStdString());
            id_Wrong_lastFrame = -1;
            timers--;  //只获取并打印一次
    }
    }


/*****核心阶段控制*****/
    countbuilding();
    countarmies();
    switch (stage) {
    //--------------------------------------------------------------------------------------------------------------------------------------
    case 0:{
        /*****遍历建筑*****/
            for(tagBuilding building : myinfo.buildings){

                if(building.Type==BUILDING_CENTER&&building.Project==ACT_NULL&&myinfo.farmers.size()<8){
                    BuildingAction(building.SN,BUILDING_CENTER_CREATEFARMER);
                }
                if(building.Type==BUILDING_ARROWTOWER&&building.Percent==100&&!(count_clock%10)){
                    if(!building.Project){
                        getdistance(BUILDING_ARROWTOWER,2,building.BlockDR,building.BlockUR);
                        HumanAction(building.SN,SN_todo);
                    }
                }
                if(building.Type==BUILDING_GRANARY&&building.Percent==100){
                    if(building.Project==ACT_NULL&&myinfo.Meat>BUILDING_GRANARY_ARROWTOWER_FOOD){

                        BuildingAction(building.SN,BUILDING_GRANARY_ARROWTOWER);

                        yesupgradearrowtowerstart=1;

                    }

                }
            }




        /*****遍历农民*****/
            for(int i = 0;i < myinfo.farmers.size();i++){

                int nowstate_farmer=myinfo.farmers[i].NowState;

                if(i==0&&(nowstate_farmer==HUMAN_STATE_IDLE || nowstate_farmer==HUMAN_STATE_STOP)&&house<1&&myinfo.Wood>=30){
                    if(y_home==38){
                        if(map22[x_home][y_home]){
                            HumanBuild(myinfo.farmers[i].SN,BUILDING_HOME,x_home,y_home);
                        }
                    }
                    if(x_home==38){
                        x_home=28;
                        y_home+=5;
                    }
                        x_home+=5;
                }

                else if(i==1&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)&&!amountofgranary){
                    //positionofresource(RESOURCE_BUSH);
                    double dis=0;
                    double temp_dis;
                    for(int j = 0;j < myinfo.resources.size();j++){
                        if(myinfo.resources[j].Type==RESOURCE_BUSH){
                            temp_dis=calDistance(myinfo.resources[j].DR,myinfo.resources[j].UR,mid,mid);
                            if(temp_dis>dis){
                                dis=temp_dis;
                                average_bush_DR=myinfo.resources[j].BlockDR;
                                average_bush_UR=myinfo.resources[j].BlockUR;
                            }
                        }
                    }
                    dis=1e6;
                    temp_dis=0;
                    for(int j = 0;j < 70;j++){
                        for(int k = 0;k < 70;k++){
                            if(map33[j][k]){
                                temp_dis=calDistance(j,k,average_bush_DR,average_bush_UR);
                                if(temp_dis<dis){
                                    dis=temp_dis;
                                    x_granary=j;
                                    y_granary=k;
                                }
                            }
                        }
                    }

                    HumanBuild(myinfo.farmers[i].SN,BUILDING_GRANARY,x_granary,y_granary);
                    /*if(y_tofind33x<=39){
                        if(map33[x_tofind33x][y_tofind33x]){
                            HumanBuild(myinfo.farmers[i].SN,BUILDING_GRANARY,x_tofind33x,y_tofind33x);
                        }
                    }
                    if(x_tofind33x>=43){
                        x_tofind33x=27;
                        y_tofind33x+=4;
                    }*/
                        //x_tofind33x+=4;
                    }
                /*else if(i==1&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)&&amountofgranary&&!amountofcamp&&myinfo.Wood>=125){

                    if(y_tofind33x<=39){
                        if(map33[x_tofind33x][y_tofind33x]){
                            HumanBuild(myinfo.farmers[i].SN,BUILDING_ARMYCAMP,x_tofind33x,y_tofind33x);
                        }

                    }
                    if(x_tofind33x==43){
                        x_tofind33x=27;
                        y_tofind33x+=4;
                    }
                        x_tofind33x+=4;
                    }*/
                /*else if(i==2&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)&&yesupgradearrowtower&&myinfo.Stone>=150&&tower<8){
                    double temp_dis;
                    double dis=1e6;
                    if(map22[x_arrowtower][y_arrowtower]){
                        HumanBuild(myinfo.farmers[i].SN,BUILDING_ARROWTOWER,x_arrowtower,y_arrowtower);
                    }
                    else {

                        for(int j = 0;j < 71;j++){
                            for(int k = 0;k < 71;k++){
                                temp_dis=calDistance(x_arrowtower,y_arrowtower,j,k);
                                if(temp_dis<dis){
                                    dis=temp_dis;
                                    x_arrowtower_build=j;
                                    y_arrowtower_build=k;
                                }
                            }
                        }
                        HumanBuild(myinfo.farmers[i].SN,BUILDING_ARROWTOWER,x_arrowtower_build,y_arrowtower_build);
                    }


                        if(x_arrowtower==26&&y_arrowtower==26){
                            x_arrowtower=46;
                        }
                        else if(x_arrowtower==46&&y_arrowtower==26){
                            y_arrowtower=46;
                        }
                        else if(x_arrowtower==46&&y_arrowtower==46){
                            x_arrowtower=26;
                        }
                        else if(x_arrowtower==26&&y_arrowtower==26){
                            y_arrowtower=26;
                        }


                }*/
                else if((i<2||i==4)&&(nowstate_farmer==HUMAN_STATE_IDLE)){

                    getdistance(RESOURCE_TREE,1,mid,mid);
                    HumanAction(myinfo.farmers[i].SN,SN_todo);
                }
                else if(i>=2&&(nowstate_farmer==HUMAN_STATE_IDLE)){
                    getdistance(RESOURCE_BUSH,1,mid,mid);
                    HumanAction(myinfo.farmers[i].SN,SN_todo);
                }
            }




        /*****遍历军队*****/
            for(int i = 0;i < myinfo.armies.size();i++){

                int nowstate_army=myinfo.armies[i].NowState;

                if(myinfo.armies[i].Blood!=Blood[i]&&nowstate_army!=HUMAN_STATE_ATTACKING&&nowstate_army!=HUMAN_STATE_GOTO_ATTACK){
                    //HumanAction(myinfo.armies[i].SN,myinfo.)
                }
                if(myinfo.armies[i].Sort==ARMY_ARCHER){
                    //static x_round=25;

                    if(nowstate_army==HUMAN_STATE_IDLE||nowstate_army==HUMAN_STATE_STOP){
                       // HumanMove(myinfo.armies[i].SN,)
                    }
                }
                Blood[i]=myinfo.armies[i].Blood;
            }


        break;
    }
    //--------------------------------------------------------------------------------------------------------------------------------------
    case 1:{
        /*****遍历建筑*****/
            for(tagBuilding building : myinfo.buildings){
                if(building.Type==BUILDING_CENTER&&building.Project==ACT_NULL&&myinfo.farmers.size()<12){
                    BuildingAction(building.SN,BUILDING_CENTER_CREATEFARMER);
                }
                if(building.Type==BUILDING_ARROWTOWER&&building.Percent==100&&!(count_clock%10)){
                    if(!building.Project){
                        getdistance(BUILDING_ARROWTOWER,2,building.BlockDR,building.BlockUR);
                        HumanAction(building.SN,SN_todo);
                    }
                }
                /*if(building.Type==BUILDING_ARMYCAMP&&building.Percent==100&&building.Project==ACT_NULL&&clubman<3){
                    BuildingAction(building.SN,BUILDING_ARMYCAMP_CREATE_CLUBMAN);
                }*/
            }




        /*****遍历农民*****/
            for(int i = 0;i < myinfo.farmers.size();i++){
                int nowstate_farmer=myinfo.farmers[i].NowState;
                //农民修复建筑
                for(tagBuilding building : myinfo.buildings){
                    if(building.Blood<building.MaxBlood){
                        if((i==3||i==5)&&(nowstate_farmer==HUMAN_STATE_GOTO_RESOURCE)){
                            HumanAction(myinfo.farmers[i].SN,building.SN);
                        }
                    }
                }


                //农民受到攻击躲避到箭塔旁
                if(bfgenerate){
                    bloodfarmer[i]=myinfo.farmers[i].Blood;
                }
                double x_farmermove;
                double y_farmermove;
                if(myinfo.farmers[i].Blood!=bloodfarmer[i]){
                    for(tagBuilding building : myinfo.buildings){
                        if(building.Type==BUILDING_ARROWTOWER){
                            double temp_dis;
                            double dis=1e6;
                            temp_dis=calDistance(myinfo.farmers[i].BlockDR,myinfo.farmers[i].BlockUR,building.BlockDR,building.BlockUR);
                            if(temp_dis<dis){
                                dis=temp_dis;
                                x_farmermove=building.BlockDR-0.25;
                                y_farmermove=building.BlockUR-0.25;
                            }
                        }
                    }
                    x_farmermove*=BLOCKSIDELENGTH;
                    y_farmermove*=BLOCKSIDELENGTH;
                    HumanMove(myinfo.farmers[i].SN,x_farmermove,y_farmermove);
                }

                //农民的具体操作策略
                if(i==0&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP||nowstate_farmer==HUMAN_STATE_GOTO_RESOURCE)&&amountofstock==1&&myinfo.Wood>120){
                    //positionofresource(RESOURCE_STONE);
                    double dis=1e6;
                    double temp_dis;
                    int x_stone;
                    int y_stone;
                    getdistance(RESOURCE_STONE,1,mid,mid);
                    for(tagResource resource : myinfo.resources){
                        if(resource.Type==RESOURCE_STONE){
                            if(resource.SN==SN_todo){
                                x_stone=resource.BlockDR;
                                y_stone=resource.BlockUR;
                            }
                        }
                    }
                    for(int j = 12;j < 60;j++){
                        for(int k = 12;k < 60;k++){
                            if(map33[j][k]){
                                temp_dis=calDistance(j,k,x_stone,y_stone);
                                if(temp_dis<dis){
                                    dis=temp_dis;
                                    x_stock=j;
                                    y_stock=k;
                                }
                            }
                        }
                    }
                    x_stock1position=x_stock;
                    y_stock1position=y_stock;
                    HumanBuild(myinfo.farmers[i].SN,BUILDING_STOCK,x_stock,y_stock);
                    /*if(y_tofind33x<=39){
                        if(map33[x_tofind33x][y_tofind33x]){
                            HumanBuild(myinfo.farmers[i].SN,BUILDING_GRANARY,x_tofind33x,y_tofind33x);
                        }
                    }
                    if(x_tofind33x>=43){
                        x_tofind33x=27;
                        y_tofind33x+=4;
                    }*/
                        //x_tofind33x+=4;
                    }
                else if(i==3&&(nowstate_farmer==HUMAN_STATE_IDLE || nowstate_farmer==HUMAN_STATE_STOP||nowstate_farmer==HUMAN_STATE_GOTO_RESOURCE)&&amountofstock==2&&house<2&&myinfo.Wood>=30){
                    if(y_home==38){
                        if(map22[x_home][y_home]){
                            HumanBuild(myinfo.farmers[i].SN,BUILDING_HOME,x_home,y_home);
                        }
                    }
                    if(x_home==38){
                        x_home=28;
                        y_home+=5;
                    }
                        x_home+=5;
                }

                else if(i==4&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP||nowstate_farmer==HUMAN_STATE_GOTO_RESOURCE)&&yesupgradearrowtower&&myinfo.Stone>=150&&tower<2){
                    for(int j = 0;j < 16;j++){
                        if(!wall_arrowtower[j]&&map22[x_wall_arrowtower[j]][y_wall_arrowtower[j]]){
                            x_arrowtower=x_wall_arrowtower[j];
                            y_arrowtower=y_wall_arrowtower[j];
                            wall_arrowtower[j]=1;
                            break;
                        }
                    }
                    HumanBuild(myinfo.farmers[i].SN,BUILDING_ARROWTOWER,x_arrowtower,y_arrowtower);
                    /*if(y_home>=19){
                        if(map22[x_home][y_home]){
                            HumanBuild(myinfo.farmers[i].SN,BUILDING_ARROWTOWER,x_home,y_home);
                        }
                    }
                    if(x_home==40){
                        x_home=28;
                        y_home-=2;
                    }
                        x_home+=2;*/
                    /*double temp_dis;
                    double dis=1e6;
                    if(map22[x_arrowtower][y_arrowtower]){
                        HumanBuild(myinfo.farmers[i].SN,BUILDING_ARROWTOWER,x_arrowtower,y_arrowtower);
                    }
                    else {
                        for(int j = 0;j < 71;j++){
                            for(int k = 0;k < 71;k++){
                                if(map22[j][k]){
                                    temp_dis=calDistance(x_arrowtower,y_arrowtower,j,k);
                                    if(temp_dis<dis){
                                        dis=temp_dis;
                                        x_arrowtower_build=j;
                                        y_arrowtower_build=k;
                                    }
                                }
                            }
                        }
                        HumanBuild(myinfo.farmers[i].SN,BUILDING_ARROWTOWER,x_arrowtower_build,y_arrowtower_build);
                    }


                        if(x_arrowtower==26&&y_arrowtower==26){
                            x_arrowtower=46;
                        }
                        else if(x_arrowtower==46&&y_arrowtower==26){
                            y_arrowtower=46;
                        }
                        else if(x_arrowtower==46&&y_arrowtower==46){
                            x_arrowtower=26;
                        }
                        else if(x_arrowtower==26&&y_arrowtower==26){
                            y_arrowtower=26;
                        } */
                }
                /*else if(i==5&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)&&!amountofmarket&&myinfo.Wood>150){
                    if(map33[x_tofind33x][y_tofind33x]){
                        HumanBuild(myinfo.farmers[i].SN,BUILDING_MARKET,x_tofind33x,y_tofind33x);
                    }
                }*/
                else if((i==3||i==5)&&myinfo.Stone>150){
                   continue;
                }
                else if((i<3)&&(nowstate_farmer==HUMAN_STATE_IDLE)){
                    if(amountofstock==1){
                        getdistance(RESOURCE_STONE,1,mid,mid);
                    }
                    else{
                        getdistance(RESOURCE_STONE,1,x_stock1position*BLOCKSIDELENGTH,y_stock1position*BLOCKSIDELENGTH);
                    }
                    HumanAction(myinfo.farmers[i].SN,SN_todo);
                }
                else if(i>=3&&i<5&&(nowstate_farmer==HUMAN_STATE_IDLE)){
                    getdistance(RESOURCE_TREE,1,mid,mid);
                    HumanAction(myinfo.farmers[i].SN,SN_todo);
                }
                else if(i>=5&&(nowstate_farmer==HUMAN_STATE_IDLE)){
                    getdistance(RESOURCE_BUSH,1,mid,mid);
                    HumanAction(myinfo.farmers[i].SN,SN_todo);
                }
                bloodfarmer[i]=myinfo.farmers[i].Blood;

            }
        bfgenerate=0;



        /*****遍历军队*****/
            for(int i = 0;i < myinfo.armies.size();i++){

                int nowstate_army=myinfo.armies[i].NowState;
                if(bagenerate){
                    Blood[i]=myinfo.armies[i].Blood;
                }
                double temp_dis;
                double dis=1e6;
                temp_dis = calDistance(myinfo.armies[i].DR,myinfo.armies[i].UR,mid+100,mid-100);
                        if(temp_dis>100&&myinfo.armies[i].NowState==HUMAN_STATE_IDLE)
                        {   // 如果军队距离中间位置超过200且处于空闲状态，移动到中间位置
                            HumanMove(myinfo.armies[i].SN,mid+100,mid-100);
                        }
                if(nowstate_army!=HUMAN_STATE_ATTACKING&&nowstate_army!=HUMAN_STATE_GOTO_ATTACK){
                    double dis=1e6;
                    double temp_dis;
                    if(myinfo.enemy_armies.size()){
                        for(int j = 0;j < myinfo.enemy_armies.size();j++){
                            temp_dis=calDistance(myinfo.armies[i].DR,myinfo.armies[i].UR,myinfo.enemy_armies[j].DR,myinfo.enemy_armies[j].UR);
                            if(temp_dis<dis){
                                dis=temp_dis;
                                SN_todo=myinfo.enemy_armies[j].SN;
                            }
                        }
                    }
                    HumanAction(myinfo.armies[i].SN,SN_todo);
                }
                if(myinfo.armies[i].Sort==ARMY_ARCHER){
                    //static x_round=25;

                    if(nowstate_army==HUMAN_STATE_IDLE||nowstate_army==HUMAN_STATE_STOP){
                       // HumanMove(myinfo.armies[i].SN,)
                    }
                }
                Blood[i]=myinfo.armies[i].Blood;
                bagenerate=0;
            }


        break;
    }
    //--------------------------------------------------------------------------------------------------------------------------------------
    case 2:{
        /*****遍历建筑*****/
            for(tagBuilding building : myinfo.buildings){
                if(building.Type==BUILDING_CENTER&&building.Project==ACT_NULL&&myinfo.farmers.size()<20){
                    BuildingAction(building.SN,BUILDING_CENTER_CREATEFARMER);
                }
                if(building.Type==BUILDING_ARROWTOWER&&building.Percent==100&&!(count_clock%10)){
                    if(!building.Project){
                        getdistance(BUILDING_ARROWTOWER,2,building.BlockDR,building.BlockUR);
                        HumanAction(building.SN,SN_todo);
                    }
                }
                /*if(building.Type==BUILDING_ARMYCAMP&&building.Percent==100&&building.Project==ACT_NULL&&clubman<3){
                    BuildingAction(building.SN,BUILDING_ARMYCAMP_CREATE_CLUBMAN);
                }*/
            }




        /*****遍历农民*****/
            for(int i = 0;i < myinfo.farmers.size();i++){
                int nowstate_farmer=myinfo.farmers[i].NowState;
                //农民修复建筑
                //i<8&&(((!((i+2)%4)))||(!((i+3)%4)))
                for(tagBuilding building : myinfo.buildings){
                    if(building.Blood<building.MaxBlood){
                        if(i<4&&(nowstate_farmer==HUMAN_STATE_GOTO_RESOURCE||nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)){
                            HumanAction(myinfo.farmers[i].SN,building.SN);
                        }
                    }
                }
                //农民受到攻击躲避到箭塔旁
                if(bfgenerate){
                    bloodfarmer[i]=myinfo.farmers[i].Blood;
                }
                double x_farmermove;
                double y_farmermove;
                if(myinfo.farmers[i].Blood!=bloodfarmer[i]){
                    for(tagBuilding building : myinfo.buildings){
                        if(building.Type==BUILDING_ARROWTOWER){
                            double temp_dis;
                            double dis=1e6;
                            temp_dis=calDistance(myinfo.farmers[i].BlockDR,myinfo.farmers[i].BlockUR,building.BlockDR,building.BlockUR);

                            if(temp_dis<dis){
                                dis=temp_dis;
                                x_farmermove=building.BlockDR-0.25;
                                y_farmermove=building.BlockUR-0.25;
                            }
                        }
                    }
                    x_farmermove*=BLOCKSIDELENGTH;
                    y_farmermove*=BLOCKSIDELENGTH;
                    HumanMove(myinfo.farmers[i].SN,x_farmermove,y_farmermove);
                }

                //农民的具体操作策略
                /*if(i==5&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)&&amountofstock==2&&myinfo.Wood>120){
                    //positionofresource(RESOURCE_STONE);
                    double dis=0;
                    double temp_dis;
                    int x_stone;
                    int y_stone;
                    for(int j = 0 ; j<myinfo.resources.size() ; j++){
                        temp_dis = calDistance(mid,mid,myinfo.resources[j].DR, myinfo.resources[j].UR);
                        if(myinfo.resources[j].Type==RESOURCE_STONE&&temp_dis>dis){
                            SN_todo = myinfo.resources[j].SN;
                            dis = temp_dis;
                            x_stone=myinfo.resources[j].BlockDR;
                            y_stone=myinfo.resources[j].BlockUR;
                         }
                    }
                    for(tagResource resource : myinfo.resources){
                        if(resource.Type==RESOURCE_STONE){
                            if(resource.SN==SN_todo){
                                x_stone=resource.BlockDR;
                                y_stone=resource.BlockUR;
                            }
                        }
                    }
                    for(int j = 12;j < 60;j++){
                        for(int k = 12;k < 60;k++){
                            if(map33[j][k]){
                                temp_dis=calDistance(j,k,x_stone,y_stone);
                                if(temp_dis<dis){
                                    dis=temp_dis;
                                    x_stock=j;
                                    y_stock=k;
                                }
                            }
                        }
                    }
                    x_stock2position=x_stock*BLOCKSIDELENGTH;
                    y_stock2position=y_stock*BLOCKSIDELENGTH;
                    HumanBuild(myinfo.farmers[i].SN,BUILDING_STOCK,x_stock,y_stock);
                    if(y_tofind33x<=39){
                        if(map33[x_tofind33x][y_tofind33x]){
                            HumanBuild(myinfo.farmers[i].SN,BUILDING_GRANARY,x_tofind33x,y_tofind33x);
                        }
                    }
                    if(x_tofind33x>=43){
                        x_tofind33x=27;
                        y_tofind33x+=4;
                    }
                        //x_tofind33x+=4;
                    }*/
                if(i==5&&(nowstate_farmer==HUMAN_STATE_IDLE || nowstate_farmer==HUMAN_STATE_STOP)&&house<4&&myinfo.Wood>=30){
                    if(map22[x_home][y_home]){
                        HumanBuild(myinfo.farmers[i].SN,BUILDING_HOME,x_home,y_home);
                    }
                    if(x_home==38){
                        x_home=28;
                        y_home+=5;
                    }
                    x_home+=5;
                }
                else if(i==1&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP||nowstate_farmer==HUMAN_STATE_GOTO_RESOURCE)&&yesupgradearrowtower&&myinfo.Stone>=150&&tower<16){
                    for(int j = 0;j < 16;j++){
                        if(!wall_arrowtower[j]&&map22[x_wall_arrowtower[j]][y_wall_arrowtower[j]]){
                            x_arrowtower=x_wall_arrowtower[j];
                            y_arrowtower=y_wall_arrowtower[j];
                            wall_arrowtower[j]=1;
                            break;
                        }
                    }
                    HumanBuild(myinfo.farmers[i].SN,BUILDING_ARROWTOWER,x_arrowtower,y_arrowtower);
                    /*if(y_home>=19){
                        if(map22[x_home][y_home]){
                            HumanBuild(myinfo.farmers[i].SN,BUILDING_ARROWTOWER,x_home,y_home);
                        }
                    }
                    if(x_home==40){
                        x_home=28;
                        y_home-=2;
                    }
                        x_home+=2;*/
                    /*double temp_dis;
                    double dis=1e6;
                    if(map22[x_arrowtower][y_arrowtower]){
                        HumanBuild(myinfo.farmers[i].SN,BUILDING_ARROWTOWER,x_arrowtower,y_arrowtower);
                    }
                    else {
                        for(int j = 0;j < 71;j++){
                            for(int k = 0;k < 71;k++){
                                if(map22[j][k]){
                                    temp_dis=calDistance(x_arrowtower,y_arrowtower,j,k);
                                    if(temp_dis<dis){
                                        dis=temp_dis;
                                        x_arrowtower_build=j;
                                        y_arrowtower_build=k;
                                    }
                                }
                            }
                        }
                        HumanBuild(myinfo.farmers[i].SN,BUILDING_ARROWTOWER,x_arrowtower_build,y_arrowtower_build);
                    }


                        if(x_arrowtower==26&&y_arrowtower==26){
                            x_arrowtower=46;
                        }
                        else if(x_arrowtower==46&&y_arrowtower==26){
                            y_arrowtower=46;
                        }
                        else if(x_arrowtower==46&&y_arrowtower==46){
                            x_arrowtower=26;
                        }
                        else if(x_arrowtower==26&&y_arrowtower==26){
                            y_arrowtower=26;
                        } */
                }
                /*else if(i==5&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)&&!amountofmarket&&myinfo.Wood>150){
                    if(map33[x_tofind33x][y_tofind33x]){
                        HumanBuild(myinfo.farmers[i].SN,BUILDING_MARKET,x_tofind33x,y_tofind33x);
                    }
                }*/
                else if(i<4&&myinfo.Stone>150){
                   continue;
                }
                else if((!(i%4)||(!((i+1)%4)))&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)){//0,4,8
                    getdistance(RESOURCE_STONE,1,myinfo.farmers[i].DR,myinfo.farmers[i].UR);
                    HumanAction(myinfo.farmers[i].SN,SN_todo);
                }
                /*else if((!((i+1)%4))&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)){//3,7,11
                    getdistance(RESOURCE_STONE,1,x_stock2position,y_stock2position);
                    HumanAction(myinfo.farmers[i].SN,SN_todo);
                }*/
                else if((!((i+2)%4))&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)&&house<4){//2,6,10
                    getdistance(RESOURCE_TREE,1,mid,mid);
                    HumanAction(myinfo.farmers[i].SN,SN_todo);
                }
                else if((!((i+2)%4))&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)&&house==4){//2,6,10
                    getdistance(RESOURCE_BUSH,1,mid,mid);
                    HumanAction(myinfo.farmers[i].SN,SN_todo);
                }
                else if((!((i+3)%4))&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)){//1,5,9

                    getdistance(RESOURCE_BUSH,1,mid,mid);
                    HumanAction(myinfo.farmers[i].SN,SN_todo);
                }
                if(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP){

                }
                bloodfarmer[i]=myinfo.farmers[i].Blood;

            }
        bfgenerate=0;



        /*****遍历军队*****/
            for(int i = 0;i < myinfo.armies.size();i++){

                int nowstate_army=myinfo.armies[i].NowState;
                if(bagenerate){
                    Blood[i]=myinfo.armies[i].Blood;
                }
                double temp_dis;
                double dis=1e6;
                temp_dis = calDistance(myinfo.armies[i].DR,myinfo.armies[i].UR,mid+100,mid-100);
                        if(temp_dis>100&&myinfo.armies[i].NowState==HUMAN_STATE_IDLE)
                        {   // 如果军队距离中间位置超过200且处于空闲状态，移动到中间位置
                            HumanMove(myinfo.armies[i].SN,mid+100,mid-100);
                        }
                if(nowstate_army!=HUMAN_STATE_ATTACKING&&nowstate_army!=HUMAN_STATE_GOTO_ATTACK){
                    double dis=1e6;
                    double temp_dis;
                    if(myinfo.enemy_armies.size()){
                        for(int j = 0;j < myinfo.enemy_armies.size();j++){
                            temp_dis=calDistance(myinfo.armies[i].DR,myinfo.armies[i].UR,myinfo.enemy_armies[j].DR,myinfo.enemy_armies[j].UR);
                            if(temp_dis<dis){
                                dis=temp_dis;
                                SN_todo=myinfo.enemy_armies[j].SN;
                            }
                        }
                    }
                    HumanAction(myinfo.armies[i].SN,SN_todo);
                }
                if(myinfo.armies[i].Sort==ARMY_ARCHER){
                    //static x_round=25;

                    if(nowstate_army==HUMAN_STATE_IDLE||nowstate_army==HUMAN_STATE_STOP){
                       // HumanMove(myinfo.armies[i].SN,)
                    }
                }
                Blood[i]=myinfo.armies[i].Blood;
                bagenerate=0;
            }


        break;
    }
    //--------------------------------------------------------------------------------------------------------------------------------------
    case 3:{
            /*****遍历建筑*****/
                for(tagBuilding building : myinfo.buildings){
                    if(building.Type==BUILDING_CENTER&&building.Project==ACT_NULL&&myinfo.farmers.size()<20){
                        BuildingAction(building.SN,BUILDING_CENTER_CREATEFARMER);
                    }
                    if(building.Type==BUILDING_ARROWTOWER&&building.Percent==100&&!(count_clock%10)){
                        if(!building.Project){
                            getdistance(BUILDING_ARROWTOWER,2,building.BlockDR,building.BlockUR);
                            HumanAction(building.SN,SN_todo);
                        }
                    }
                    /*if(building.Type==BUILDING_ARMYCAMP&&building.Percent==100&&building.Project==ACT_NULL&&clubman<3){
                        BuildingAction(building.SN,BUILDING_ARMYCAMP_CREATE_CLUBMAN);
                    }*/
                }




            /*****遍历农民*****/
                for(int i = 0;i < myinfo.farmers.size();i++){
                    int nowstate_farmer=myinfo.farmers[i].NowState;
                    //农民修复建筑
                    //i<8&&(((!((i+2)%4)))||(!((i+3)%4)))
                    for(tagBuilding building : myinfo.buildings){
                        if(building.Blood<building.MaxBlood){
                            if((((!((i+2)%4)))||(!((i+3)%4)))&&(nowstate_farmer==HUMAN_STATE_GOTO_RESOURCE||nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)){
                                HumanAction(myinfo.farmers[i].SN,building.SN);
                            }
                        }
                    }
                    //农民受到攻击躲避到箭塔旁
                    if(bfgenerate){
                        bloodfarmer[i]=myinfo.farmers[i].Blood;
                    }
                    double x_farmermove;
                    double y_farmermove;
                    if(myinfo.farmers[i].Blood!=bloodfarmer[i]){
                        for(tagBuilding building : myinfo.buildings){
                            if(building.Type==BUILDING_ARROWTOWER){
                                double temp_dis;
                                double dis=1e6;
                                temp_dis=calDistance(myinfo.farmers[i].BlockDR,myinfo.farmers[i].BlockUR,building.BlockDR,building.BlockUR);

                                if(temp_dis<dis){
                                    dis=temp_dis;
                                    x_farmermove=building.BlockDR-0.25;
                                    y_farmermove=building.BlockUR-0.25;
                                }
                            }
                        }
                        x_farmermove*=BLOCKSIDELENGTH;
                        y_farmermove*=BLOCKSIDELENGTH;
                        HumanMove(myinfo.farmers[i].SN,x_farmermove,y_farmermove);
                    }

                    //农民的具体操作策略
                    /*if(i==5&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)&&amountofstock==2&&myinfo.Wood>120){
                        //positionofresource(RESOURCE_STONE);
                        double dis=0;
                        double temp_dis;
                        int x_stone;
                        int y_stone;
                        for(int j = 0 ; j<myinfo.resources.size() ; j++){
                            temp_dis = calDistance(mid,mid,myinfo.resources[j].DR, myinfo.resources[j].UR);
                            if(myinfo.resources[j].Type==RESOURCE_STONE&&temp_dis>dis){
                                SN_todo = myinfo.resources[j].SN;
                                dis = temp_dis;
                                x_stone=myinfo.resources[j].BlockDR;
                                y_stone=myinfo.resources[j].BlockUR;
                             }
                        }
                        for(tagResource resource : myinfo.resources){
                            if(resource.Type==RESOURCE_STONE){
                                if(resource.SN==SN_todo){
                                    x_stone=resource.BlockDR;
                                    y_stone=resource.BlockUR;
                                }
                            }
                        }
                        for(int j = 12;j < 60;j++){
                            for(int k = 12;k < 60;k++){
                                if(map33[j][k]){
                                    temp_dis=calDistance(j,k,x_stone,y_stone);
                                    if(temp_dis<dis){
                                        dis=temp_dis;
                                        x_stock=j;
                                        y_stock=k;
                                    }
                                }
                            }
                        }
                        x_stock2position=x_stock*BLOCKSIDELENGTH;
                        y_stock2position=y_stock*BLOCKSIDELENGTH;
                        HumanBuild(myinfo.farmers[i].SN,BUILDING_STOCK,x_stock,y_stock);
                        if(y_tofind33x<=39){
                            if(map33[x_tofind33x][y_tofind33x]){
                                HumanBuild(myinfo.farmers[i].SN,BUILDING_GRANARY,x_tofind33x,y_tofind33x);
                            }
                        }
                        if(x_tofind33x>=43){
                            x_tofind33x=27;
                            y_tofind33x+=4;
                        }
                            //x_tofind33x+=4;
                        }*/
                    if(i==5&&(nowstate_farmer==HUMAN_STATE_IDLE || nowstate_farmer==HUMAN_STATE_STOP)&&house<4&&myinfo.Wood>=30){
                        if(map22[x_home][y_home]){
                            HumanBuild(myinfo.farmers[i].SN,BUILDING_HOME,x_home,y_home);
                        }
                        if(x_home==38){
                            x_home=28;
                            y_home+=5;
                        }
                        x_home+=5;
                    }
                    else if(i==1&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP||nowstate_farmer==HUMAN_STATE_GOTO_RESOURCE)&&yesupgradearrowtower&&myinfo.Stone>=150&&tower<16){
                        for(int j = 0;j < 16;j++){
                            if(!wall_arrowtower[j]&&map22[x_wall_arrowtower[j]][y_wall_arrowtower[j]]){
                                x_arrowtower=x_wall_arrowtower[j];
                                y_arrowtower=y_wall_arrowtower[j];
                                wall_arrowtower[j]=1;
                                break;
                            }
                        }
                        HumanBuild(myinfo.farmers[i].SN,BUILDING_ARROWTOWER,x_arrowtower,y_arrowtower);
                        /*if(y_home>=19){
                            if(map22[x_home][y_home]){
                                HumanBuild(myinfo.farmers[i].SN,BUILDING_ARROWTOWER,x_home,y_home);
                            }
                        }
                        if(x_home==40){
                            x_home=28;
                            y_home-=2;
                        }
                            x_home+=2;*/
                        /*double temp_dis;
                        double dis=1e6;
                        if(map22[x_arrowtower][y_arrowtower]){
                            HumanBuild(myinfo.farmers[i].SN,BUILDING_ARROWTOWER,x_arrowtower,y_arrowtower);
                        }
                        else {
                            for(int j = 0;j < 71;j++){
                                for(int k = 0;k < 71;k++){
                                    if(map22[j][k]){
                                        temp_dis=calDistance(x_arrowtower,y_arrowtower,j,k);
                                        if(temp_dis<dis){
                                            dis=temp_dis;
                                            x_arrowtower_build=j;
                                            y_arrowtower_build=k;
                                        }
                                    }
                                }
                            }
                            HumanBuild(myinfo.farmers[i].SN,BUILDING_ARROWTOWER,x_arrowtower_build,y_arrowtower_build);
                        }


                            if(x_arrowtower==26&&y_arrowtower==26){
                                x_arrowtower=46;
                            }
                            else if(x_arrowtower==46&&y_arrowtower==26){
                                y_arrowtower=46;
                            }
                            else if(x_arrowtower==46&&y_arrowtower==46){
                                x_arrowtower=26;
                            }
                            else if(x_arrowtower==26&&y_arrowtower==26){
                                y_arrowtower=26;
                            } */
                    }
                    /*else if(i==5&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)&&!amountofmarket&&myinfo.Wood>150){
                        if(map33[x_tofind33x][y_tofind33x]){
                            HumanBuild(myinfo.farmers[i].SN,BUILDING_MARKET,x_tofind33x,y_tofind33x);
                        }
                    }*/
                    else if((((!((i+2)%4)))||(!((i+3)%4)))&&myinfo.Stone>150){
                       continue;
                    }
                    else if((!(i%4)||(!((i+1)%4)))&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)){//0,4,8
                        getdistance(RESOURCE_STONE,1,myinfo.farmers[i].DR,myinfo.farmers[i].UR);
                        HumanAction(myinfo.farmers[i].SN,SN_todo);
                    }
                    /*else if((!((i+1)%4))&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)){//3,7,11
                        getdistance(RESOURCE_STONE,1,x_stock2position,y_stock2position);
                        HumanAction(myinfo.farmers[i].SN,SN_todo);
                    }*/
                    /*else if((!((i+2)%4))&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)&&house<4){//2,6,10
                        getdistance(RESOURCE_TREE,1,mid,mid);
                        HumanAction(myinfo.farmers[i].SN,SN_todo);
                    }
                    else if((!((i+2)%4))&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)&&house==4){//2,6,10
                        getdistance(RESOURCE_BUSH,1,mid,mid);
                        HumanAction(myinfo.farmers[i].SN,SN_todo);
                    }*/
                    else if((!((i+3)%4))&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)&&myinfo.farmers.size()<18){//1,5,9

                        getdistance(RESOURCE_BUSH,1,mid,mid);
                        HumanAction(myinfo.farmers[i].SN,SN_todo);
                    }
                    else if(i%4<=2&&i%4>=1&&(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP)){
                        double temp_dis;
                        double dis=1e6;
                        temp_dis = calDistance(myinfo.farmers[i].DR,myinfo.farmers[i].UR,mid+100,mid-100);
                                if(temp_dis>100&&myinfo.farmers[i].NowState==HUMAN_STATE_IDLE)
                                {
                                    HumanMove(myinfo.farmers[i].SN,mid+100,mid-100);
                                }
                    }
                    if(nowstate_farmer==HUMAN_STATE_IDLE||nowstate_farmer==HUMAN_STATE_STOP){

                    }
                    bloodfarmer[i]=myinfo.farmers[i].Blood;

                }
            bfgenerate=0;



            /*****遍历军队*****/
                for(int i = 0;i < myinfo.armies.size();i++){

                    int nowstate_army=myinfo.armies[i].NowState;
                    if(bagenerate){
                        Blood[i]=myinfo.armies[i].Blood;
                    }
                    double temp_dis;
                    double dis=1e6;
                    temp_dis = calDistance(myinfo.armies[i].DR,myinfo.armies[i].UR,mid+100,mid-100);
                            if(temp_dis>100&&myinfo.armies[i].NowState==HUMAN_STATE_IDLE)
                            {   // 如果军队距离中间位置超过200且处于空闲状态，移动到中间位置
                                HumanMove(myinfo.armies[i].SN,mid+100,mid-100);
                            }
                    if(nowstate_army!=HUMAN_STATE_ATTACKING&&nowstate_army!=HUMAN_STATE_GOTO_ATTACK){
                        double dis=1e6;
                        double temp_dis;
                        if(myinfo.enemy_armies.size()){
                            for(int j = 0;j < myinfo.enemy_armies.size();j++){
                                temp_dis=calDistance(myinfo.armies[i].DR,myinfo.armies[i].UR,myinfo.enemy_armies[j].DR,myinfo.enemy_armies[j].UR);
                                if(temp_dis<dis){
                                    dis=temp_dis;
                                    SN_todo=myinfo.enemy_armies[j].SN;
                                }
                            }
                        }
                        HumanAction(myinfo.armies[i].SN,SN_todo);
                    }
                    if(myinfo.armies[i].Sort==ARMY_ARCHER){
                        //static x_round=25;

                        if(nowstate_army==HUMAN_STATE_IDLE||nowstate_army==HUMAN_STATE_STOP){
                           // HumanMove(myinfo.armies[i].SN,)
                        }
                    }
                    Blood[i]=myinfo.armies[i].Blood;
                    bagenerate=0;
                }


            break;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------
    default:
        break;
    }
    switch (stage) {
    case 0:
        if(yesupgradearrowtower&&house==1){
            stage++;
            generatebacktobuild();
            //DebugText("stage=1");
        }
        break;
    case 1:
        if(tower==2&&amountofstock==2){
            stage++;
            generatebacktobuild();
            //("stage=2");
        }
        break;
    case 2:
        if(tower>12||myinfo.farmers.size()>15){
            stage++;
            generatebacktobuild();
            //DebugText("stage=3");
        }
        break;
    default:
        break;
    }


}



/*        if(myinfo.armies[i].Blood!=Blood[i]&&nowstate_army[i]!=HUMAN_STATE_IDLE&&chasinglock[i]==0){
            for(int j=0;j<enemyInfo.enemy_armies.size();j++){
                if(myinfo.armies[j].WorkObjectSN==enemyInfo.armies[i].SN){
                    HumanAction(myinfo.armies[i].SN,myinfo.enemy_armies[j].SN);
                    myinfo.armies[i].NowState=CHASE;
                    Blood[i]=myinfo.armies[i].Blood;
                    chasestart_L[i]=enemyInfo.armies[i].DR;
                    chasestart_U[i]=enemyInfo.armies[i].UR;
                    //timer[i]=g_frame;
                    ChasingLock[i]==1;
                }
            }
            if(armystate[i]!=CHASE){
                for(int j=0;j<enemyInfo.enemy_farmers.size();j++){
                    if(myinfo.enemy_farmers[j].WorkObjectSN==myinfo.armies[i].SN){
                        HumanAction(myinfo.armies[i].SN,myinfo.enemy_farmers[j].SN);
                        armystate[i]=CHASE;
                        Blood[i]=myinfo.armies[i].Blood;
                        chasestart_L[i]=myinfo.armies[i].DR;
                        chasestart_U[i]=myinfo.armies[i].UR;
                        //timer[i]=g_frame;
                        ChasingLock[i]==1;
                    }
            }
        }
            if(armystate[i]!=CHASE){
                for(int j=0;j<enemyInfo.enemy_buildings.size();j++){
                    if(myinfo.enemy_buildings[j].Project==myinfo.armies[i].SN){
                        HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[j].SN);
                        armystate[i]=CHASE;
                        Blood[i]=myinfo.armies[i].Blood;
                        chasestart_L[i]=myinfo.armies[i].DR;
                        chasestart_U[i]=myinfo.armies[i].UR;
                        //timer[i]=g_frame;
                        ChasingLock[i]==1;
                    }
                }
            }
        }
    }
}*/
/*for(int i=0;i<enemyInfo.armies.size();i++){
           if(enemyInfo.armies[i].Blood!=Blood[i]&&armystate[i]!=CHASE&&ChasingLock[i]==0){
               for(int j=0;j<enemyInfo.enemy_armies.size();j++){
                   if(enemyInfo.enemy_armies[j].WorkObjectSN==enemyInfo.armies[i].SN){
                       HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[j].SN);
                       armystate[i]=CHASE;
                       Blood[i]=enemyInfo.armies[i].Blood;
                       chasestart_L[i]=enemyInfo.armies[i].DR;
                       chasestart_U[i]=enemyInfo.armies[i].UR;
                       timer[i]=g_frame;
                       ChasingLock[i]==1;
                   }
               }
               if(armystate[i]!=CHASE){
                   for(int j=0;j<enemyInfo.enemy_farmers.size();j++){
                       if(enemyInfo.enemy_farmers[j].WorkObjectSN==enemyInfo.armies[i].SN){
                           HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[j].SN);
                           armystate[i]=CHASE;
                           Blood[i]=enemyInfo.armies[i].Blood;
                           chasestart_L[i]=enemyInfo.armies[i].DR;
                           chasestart_U[i]=enemyInfo.armies[i].UR;
                           timer[i]=g_frame;
                           ChasingLock[i]==1;
                       }
               }
           }
               if(armystate[i]!=CHASE){
                   for(int j=0;j<enemyInfo.enemy_buildings.size();j++){
                       if(enemyInfo.enemy_buildings[j].Project==enemyInfo.armies[i].SN){
                           HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[j].SN);
                           armystate[i]=CHASE;
                           Blood[i]=enemyInfo.armies[i].Blood;
                           chasestart_L[i]=enemyInfo.armies[i].DR;
                           chasestart_U[i]=enemyInfo.armies[i].UR;
                           timer[i]=g_frame;
                           ChasingLock[i]==1;
                       }
                   }
               }
           }}
for(intj = 0 ; j<myInfo.resources.size() ; j++)
            {
                temp_dis = calDistance(mid,mid,myInfo.resources[j].DR, myInfo.resources[j].UR);
                if(myInfo.resources[j].Type==RESOURCE_BUSH &&temp_dis<dis )
                {
                    SN_todo = myInfo.resources[j].SN;
                    dis = temp_dis;
                }
            }
           */
