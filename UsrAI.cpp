#include "UsrAI.h"
tagGame tagUsrGame;
ins UsrIns;
/*##########请勿修改以上部分##########*/

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
