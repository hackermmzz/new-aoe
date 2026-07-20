#include "GlobalVariate.h"
#include "RuntimeConfig_private.h"
#include "config.h"
#include <QDirIterator>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QtWidgets>
#include<iostream>

using namespace std;

/*************************全局随机数生成器******************/
Random Rand(119);
/*************************配置读取量***********************/
QString ResultLogFile;//实时信息输出日志
/************************全局初始量************************/
int mapmoveFrequency;//地图移动速度
bool is_cheatAction = false;
EventFilter *eventFilter;
NetworkPlugin*NetworkManager;
map<std::string, std::list<QPixmap>> resMap;
map<string, QSoundEffect*> SoundMap;
std::queue<string> soundQueue;
Score usrScore=Score(0);
Score enemyScore=Score(1);

Score& scoreForPlayerRepresent(int playerRepresent)
{
    return playerRepresent == NOWPLAYERREPRESENT ? usrScore : enemyScore;
}
Coordinate *nowobject=NULL;
bool tryCaptured=0;//尝试捕获点击对象后置1
Coordinate* LeftMouseObjCapture=0;
Coordinate* RightMouseObjCaptrue=0;
std::queue<st_DebugMassage>debugMassagePackage;
std::map<QString , int>debugMessageRecord;
int** memorymap;    //记录出现在当前画面上的object,用于g_Object[]中访问
std::string direction[5]={"Down","LeftDown","Left","LeftUp","Up"};
bool GenerateHumanLock=0;//每一帧保证只有一个人可以诞生
///////////////////////////////////////////////////////////////////////////////
int InitImageResMap(QString path)
{
    //判断路径是否存在
    QDir dir(path);
    if(!dir.exists())
    {
        qDebug()<<"Error: A path that does not exist. path: "<<path;
        return -1;
    }

    QStringList filters;
    filters<<QString("*.png")<<QString("*.gif");
    //文件类型过滤器（去除符号链接symlink）
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    //文件名称过滤器（去除其他后缀的文件）
    dir.setNameFilters(filters);
    int dirCount = dir.count();
    //
    if(dirCount <= 0)
    {
        qDebug()<<"路径内无png和gif文件";
        return -1;
    }
    //遍历所有文件

    for(int i=0; i<dirCount; i++)
    {
        //文件名称
        QString fileName = dir[i];
        //文件全路径
        QString filePath = path + "/" + fileName;
        //获取文件后缀
        int index = fileName.lastIndexOf("_");
        QString imageMapName;
        imageMapName = fileName.left(index);
        //
        std::string tmpListName = imageMapName.toStdString();
        //获取图片
        QPixmap img;
        /*
         * 鉴于项目耦合度太高了，汪立洪根本不可能去花时间去专门解耦，
         * 特此为了满足oj的低内存运行，必须把图片整体给砍掉,
         * 特此提醒，建议别动这行代码
         *
        */
        if(!OffScreen)img=QPixmap(filePath);
        /*
         * 正所谓，
         * 项目越大，代码越屎。
         * 项目越小，神人越神。
         * 我已经无力回天。
        */

        //存储全局资源
        resMap[tmpListName].push_back(img);
    }
    ////////////////////////////////对资源进行额外操作
    //对船的帧数进行调整
    for(auto&ele:resMap){
        string name=ele.first;
        if(name.find("Ship")!=string::npos){
            auto&list=ele.second;
            auto tmp=list.front();
            for(int i=0;i<10;++i)list.push_back(tmp);
        }
        else if(name.find("Sailing")!=string::npos){
            auto&list=ele.second;
            auto tmp=list.front();
            for(int i=0;i<10;++i)list.push_back(tmp);
        }
    }
    //投石车巨石就使用普通投石兵石头，但是我们对其进行放大
    {
        auto&stone=resMap["Cobblestone"];
        auto&boulders=resMap["Boulders"];
        for(QPixmap&pix:stone){
            QPixmap scaledPix = pix.scaled(
                pix.size() * 3.0,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
            );
            boulders.push_back(scaledPix);
        }
    }

    ////////////////////////////////
    return -1;
}
int InitSoundResMap(QString path)
{
    //判断路径是否存在
    QDir dir(path);
    if(!dir.exists())
    {
        qDebug()<<"路径不存在";
        return -1;
    }

    QStringList filters;
    filters<<QString("*.wav");

    //文件类型过滤器（去除符号链接symlink）
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    //文件名称过滤器（去除其他后缀的文件）
    dir.setNameFilters(filters);
    int dirCount = dir.count();
    //
    if(dirCount <= 0)
    {
        qDebug()<<"路径内无wav文件";
        return -1;
    }

    //获取分隔符
    QChar separator = QChar('/');

    if(!path.contains(separator))
    {
        separator = QChar('\\');
    }
    QChar lastChar = path.at(path.length()-1);
    if(lastChar == separator)
    {
        separator = QChar();
    }

    for(int i=0; i<dirCount; i++)
    {
        //文件名称
        QString fileName = dir[i];
        //获取文件后缀
        int index = fileName.lastIndexOf(".");
        QString SoundMapName;
        SoundMapName = fileName.left(index);
        std::string tmpMapName = SoundMapName.toStdString();
        //
        QSoundEffect* qSoundEffect = new QSoundEffect();
        QString filePath ="qrc:///"+fileName;
        //
        qSoundEffect->setSource(QUrl(filePath));
        qSoundEffect->setVolume(MUSIC_VOLUME*100);
        SoundMap.insert(map<string, QSoundEffect*>::value_type(tmpMapName, qSoundEffect));
    }

    return -1;
}

QPixmap applyTransparencyEffect(const QPixmap& originalPixmap, qreal opacity)
{
    // 创建一个新的 QPixmap，大小与原始图片相同
    QPixmap modifiedPixmap(originalPixmap.size());
    modifiedPixmap.fill(Qt::transparent);

    // 使用 QPainter 在新 QPixmap 上绘制原始图片
    QPainter painter(&modifiedPixmap);
    painter.setOpacity(1.0);  // 将绘制原始图片的不透明度设置为1.0
    painter.drawPixmap(0, 0, originalPixmap);


    // 创建原始图片的掩码
    QBitmap mask = originalPixmap.createMaskFromColor(Qt::transparent);

    // 创建一个剪切区域，仅包含原始图片的不透明部分
    QRegion opaqueRegion = QRegion(originalPixmap.rect()).subtracted(QRegion(mask));
    QPainterPath clipPath;
    clipPath.addRect(originalPixmap.rect());
    clipPath.addRegion(opaqueRegion);
    painter.setClipPath(clipPath);

    //    // 创建一个剪切区域，仅包含原始图片的透明部分
    //    QPainterPath clipPath;
    //    clipPath.addRect(originalPixmap.rect());
    //    clipPath.addRegion(QRegion(originalPixmap.mask().inverted()));
    //    painter.setClipPath(clipPath);

    // 绘制半透明黑色矩形，不影响原始图片的透明部分
    painter.setOpacity(opacity);
    painter.fillRect(modifiedPixmap.rect(), QColor(0, 0, 0));
    painter.end();

    return modifiedPixmap;
}

/*
种类：
0为空地；
1为树木；
2为浆果；
3为瞪羚；
4为石头；
5为金矿；
6为狮子；
7为大象；
9为主营；
10为箭塔废墟。
*/

/*
  下面是预设的资源样式
*/
int Forest[3][15][15] =
{
    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0},
     {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
     {1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
     {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
     {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
     {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}},

    {{0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
     {0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
     {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
     {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}},

    {{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1}}
};

int Food[5][5][5] =
{
    {{1, 0, 1, 0, 0},
     {0, 1, 0, 0, 1},
     {0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0},
     {1, 1, 0, 0, 0}},

    {{0, 0, 0, 1, 0},
     {0, 0, 0, 0, 0},
     {0, 0, 0, 1, 1},
     {0, 0, 0, 1, 0},
     {0, 0, 1, 0, 1}},

    {{0, 0, 0, 1, 0},
     {1, 0, 1, 1, 0},
     {0, 0, 0, 0, 0},
     {0, 1, 0, 0, 0},
     {1, 0, 0, 0, 0}},

    {{0, 1, 0, 1, 0},
     {0, 0, 1, 0, 1},
     {0, 0, 0, 0, 0},
     {0, 0, 0, 0, 1},
     {0, 0, 0, 1, 0}},

    {{1, 1, 0, 1, 0},
     {1, 0, 0, 0, 0},
     {0, 0, 0, 0, 1},
     {0, 0, 0, 0, 0},
     {0, 0, 0, 1, 0}}
};

int Stone[5][5][5] =
{
    {{1, 0, 0, 1, 1},
     {0, 0, 1, 1, 1},
     {0, 0, 1, 1, 0},
     {0, 1, 0, 0, 0},
     {0, 0, 0, 1, 1}},

    {{1, 0, 0, 1, 0},
     {0, 0, 0, 1, 0},
     {0, 1, 1, 1, 0},
     {0, 0, 1, 0, 0},
     {1, 0, 1, 1, 0}},

    {{1, 1, 1, 1, 1},
     {1, 0, 0, 0, 1},
     {1, 0, 1, 0, 0},
     {0, 1, 0, 0, 1},
     {0, 0, 0, 1, 0}},

    {{0, 1, 0, 0, 1},
     {0, 1, 0, 1, 1},
     {0, 0, 1, 0, 1},
     {0, 1, 1, 1, 0},
     {0, 0, 0, 1, 0}},

    {{0, 0, 0, 1, 1},
     {0, 1, 0, 1, 0},
     {1, 0, 1, 0, 1},
     {0, 0, 0, 1, 0},
     {0, 1, 0, 1, 1}}
};


void loadResource(std::string name, std::list<ImageResource> *targetlist)
{

    //
    targetlist->clear();
    auto temp=&resMap[name];
    auto index=temp->begin();
    while(index!=temp->end())
    {

        ImageResource *res=new ImageResource();
        //赋值pix
        res->pix=(*index);
        initMemory(res);
        if(name=="Center1")
        {
            res->memorymap.fillBlockMemoryMap();
        }
        targetlist->push_back(*res);
        delete res;
        index++;
    }
}

void initMemory(ImageResource *res)
{
    //赋值内存图
    QImage piximage=res->pix.toImage();
    res->memorymap = pixMemoryMap(res->pix.width(),res->pix.height());
    for(int i=0;i<res->pix.width();i++)
    {
        for(int j=0;j<res->pix.height();j++)
        {
            if((piximage.pixel(i,j))&(0xff000000)!=0)
            {
                res->memorymap.setMemoryMap(i,j);
                if(i>0&&j>0)
                {
                    res->memorymap.setMemoryMap(i-1,j-1);
                    res->memorymap.setMemoryMap(i-1,j);
                    res->memorymap.setMemoryMap(i,j-1);
                }
                else if(i>0)
                {
                    res->memorymap.setMemoryMap(i-1,j);
                }
                else if(j>0)
                {
                    res->memorymap.setMemoryMap(i,j-1);
                }
                if(i<res->pix.width()-1&&j<res->pix.height()-1)
                {
                    res->memorymap.setMemoryMap(i+1,j+1);
                    res->memorymap.setMemoryMap(i+1,j);
                    res->memorymap.setMemoryMap(i,j+1);
                }
                else if(i<res->pix.width()-1)
                {
                    res->memorymap.setMemoryMap(i+1,j);
                }
                else if(j<res->pix.height()-1)
                {
                    res->memorymap.setMemoryMap(i,j+1);
                }
            }
        }
    }
}

Double countdistance(Double L, Double U, Double L0, Double U0)
{
    return sqrt((L-L0)*(L-L0)+(U-U0)*(U-U0));
}
bool isNear_Manhattan( Double dr , Double ur , Double dr1  , Double ur1 , Double distance )
{
    return abs(dr - dr1)<=distance && abs(ur - ur1)<=distance;
}


void flipResource(std::list<ImageResource> *currentlist, std::list<ImageResource> *targetlist)
{
    if(currentlist==0)return;
    targetlist->clear();
    std::list<ImageResource>::iterator iter = currentlist->begin();
    while (iter != currentlist->end())
    {
        QImage image = (*iter).pix.toImage();
        image = image.mirrored(true, false);
        ImageResource *res = new ImageResource(QPixmap::fromImage(image));
        initMemory(res);
        targetlist->push_back(*res);
        delete res;
        iter++;
    }
}

void loadGrayRes(std::list<ImageResource> *res, std::list<ImageResource> *grayres)
{
    auto graypointer = res->begin();
    while (graypointer != res->end())
    {
        ImageResource* res = new ImageResource(applyTransparencyEffect((*graypointer).pix, 0.5));
        res->memorymap = (*graypointer).memorymap;
        grayres->push_back(*res);
        delete res;
        graypointer++;
    }
}

void loadBlackRes(std::list<ImageResource> *res, std::list<ImageResource> *blackres)
{
    auto blackpointer = res->begin();
    while (blackpointer != res->end())
    {
        ImageResource* res = new ImageResource(applyTransparencyEffect((*blackpointer).pix, 1));
        res->memorymap = (*blackpointer).memorymap;
        blackres->push_back(*res);
        delete res;
        blackpointer++;
    }
}

int calculateManhattanDistance(int x1, int y1, int x2, int y2)
{
        int distance = abs(x1 - x2) + abs(y1 - y2);
        return distance;
}

Double calculateManhattanDistance(Double x1, Double y1, Double x2, Double y2)
{
    return abs(x1-x2)+abs(y1-y2);
}

void calMirrorPoint( Double& dr , Double &ur , Double dr_mirror, Double ur_mirror , Double dis)
{
    Double dr_deta = dr_mirror-dr, ur_deta = ur_mirror - ur;
    Double total = abs(dr_deta)+abs(ur_deta);

    dr = dr_mirror+dr_deta/total*dis;
    ur = ur_mirror+ur_deta/total*dis;

}

Double trans_BlockPointToDetailCenter( int p )
{
    return (p+Double("0.5"))*BLOCKSIDELENGTH;
}

void call_debugText(QString color, QString content,int playerID)
{
    if( !IsExamining && (!only_debug_Player0 || playerID==NOWPLAYERREPRESENT || playerID == REPRESENT_BOARDCAST_MESSAGE) )
    {
        if(  !filterRepetitionMessage || debugMessageRecord[content] == 0 || color == "black"|| color == "green" )
        {
            debugMassagePackage.push(st_DebugMassage(color, content));
            debugMessageRecord[content] = g_frame;
        }
    }
}
//*************************************************************

bool instruction::isExist() {
    return type != -1;
}

instruction::instruction(int type,int SN, int obSN , bool twoCoredinate){
    this->SN = SN;
    this->obSN = obSN;
    this->type=type;
    this->self=g_Object[SN];
    this->obj=g_Object[obSN];
}
instruction::instruction(int type,int SN,int BL,int BU,int option){
    this->SN = SN;
    this->type=type;
    this->self=g_Object[SN];
    this->BlockDR=BL;
    this->BlockUR=BU;
    this->option=option;
}
instruction::instruction(int type,int SN,Double L,Double U){
    this->SN = SN;
    this->type=type;
    this->self=g_Object[SN];
    this->DR=L;
    this->UR=U;
}
instruction::instruction(int type,int SN,int option){
    this->SN = SN;
    this->type=type;
    this->self=g_Object[SN];
    this->option=option;
}

int sgn(Double __x)
{
    if(__x > Double(0)) return 1;
    else if(__x < Double(0)) return -1;
    else return 0;
}

MouseEvent::MouseEvent()
{
    Reset();
}

int MouseEvent::GetMouseEventType()
{
    return mouseEventType;
}

void MouseEvent::SetMouseEventType(int tp)
{
    mouseEventType=tp;
}

bool MouseEvent::HaveEvent()
{
    return mouseEventType!=NULL_MOUSEEVENT;
}

int MouseEvent::GetMemoryMapX()
{
    return memoryMapX;
}

int MouseEvent::GetMemoryMapY()
{
    return memoryMapY;
}

void MouseEvent::SetMemoeyMapX(int v)
{
    memoryMapX=v;
}

void MouseEvent::SetMemoryMapY(int v)
{
    memoryMapY=v;
}

Double MouseEvent::GetDR()
{
    return DR;
}

Double MouseEvent::GetUR()
{
    return UR;
}

void MouseEvent::SetDR(Double v)
{
    DR=v;
}

void MouseEvent::SetUR(Double v)
{
    UR=v;
}

void MouseEvent::Reset()
{
    memoryMapX=0;
    memoryMapY=0;
    DR=UR=0;
    mouseEventType=NULL_MOUSEEVENT;
}
/**********************************工具函数**********************************************/
//配置参数
void ParseArguments(const QApplication&app){
    ////////////////////////////////解析参数
    QCommandLineParser parser;
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    // 添加帮助选项（自动处理--help/-h）
    parser.addHelpOption();
    QCommandLineOption option0(
        QStringList()<<"exam",
         "开启考试模式"
       );
    QCommandLineOption option1(
        QStringList()<<"offscreen",
        "关闭图像渲染"
       );
    QCommandLineOption option2(
        QStringList()<<"ResultLogFile",
         "实时数据输出存储日志",
         "a.txt"
       );
    QCommandLineOption option3(
        QStringList()<<"freq",
         "开启几倍速",
         "1|2|4|8|MAX"
       );
    QCommandLineOption option4(
        QStringList()<<"fix",
         "fixed map file for debugging",
         "map.njust"
       );
    QList<QCommandLineOption>options={option0,option1,option2,option3,option4};
    parser.addOptions(options);
    parser.process(app);
    //
    if(parser.isSet("exam")){
        RuntimeConfig_setIsExamining(true);
    }
    //
    if(parser.isSet("offscreen")){
        RuntimeConfig_setOffScreen(true);
    }
    //
    if(parser.isSet("ResultLogFile")){
        auto value=parser.value("ResultLogFile");
        ResultLogFile=value;
    }
    //
    if(parser.isSet("freq")){
        auto value=parser.value("freq");
        if(value=="MAX")RuntimeConfig_setINITIAL_FREQUENCY(INT_MAX);
        else RuntimeConfig_setINITIAL_FREQUENCY(value.toInt());
    }
    //
    if(parser.isSet("fix")){
        RuntimeConfig_setFixedMapFile(parser.value("fix"));
    }
}
//Json化一个Map
QString JsonMap(const QMap<QString, QVariant>&data){
    QJsonObject obj;
    for(auto itr=data.begin();itr!=data.end();++itr){
        obj.insert(itr.key(),QJsonValue::fromVariant(itr.value()));
    }
    QJsonDocument jsonDoc(obj);
    QString ret= jsonDoc.toJson(QJsonDocument::Indented);
    return ret;
}



ResultLogInfo::ResultLogInfo(bool win_, int score_,int wood_, int food_, int gold_, int stone_, string msg_){
    win=win_;
    wood=wood_;
    food=food_;
    gold=gold_;
    stone=stone_;
    msg=msg_;
    score=score_;
}




void ResultLogInfo::LogOut()
{

   static QTextStream*out=0;
   if(!out){
       //打开文件：WriteOnly 只写模式
       QFile*file=new QFile(ResultLogFile);
       if(!file->open(QIODevice::WriteOnly | QIODevice::Text))
       {
           qDebug() << "文件打开失败：" << file->errorString();
           return;
       }
       //创建文本流
       out=new QTextStream(file);
   }
   //////////////////////////////////////写入信息
   (*out)<<ToString()<<"\n";
   out->flush();
}

QString ResultLogInfo::ToString()
{
    QJsonObject obj;
    obj["win"]=win;
    obj["time"]=g_frame*TimePerFrame;
    obj["frame"]=g_frame;
    obj["score"]=score;
    obj["wood"]=wood;
    obj["food"]=food;
    obj["gold"]=gold;
    obj["stone"]=stone;
    if(msg!="")obj["msg"]=QString::fromStdString(msg);
    QJsonDocument doc(obj);
    QString txt=doc.toJson(QJsonDocument::Compact);//确保每次输出只占一行
    return txt;
}

st_DebugMassage::st_DebugMassage(QString color, QString content)
{
    this->color = color;
    this->content = content;
}

void Score::addScore(int points, const QString &message) {
    score += points;
    if (id == 0)
        call_debugText("blue", " 玩家" + message, REPRESENT_BOARDCAST_MESSAGE);
    else
        call_debugText("red", " 敌方" + message, REPRESENT_BOARDCAST_MESSAGE);
}

Score::Score(int id) : id(id), score(0) {}

int Score::getScore() {
    return score;
}

void Score::update(int type, int num, bool isConversion) {
    if(type==_FINDENEMYLAND){
        addScore(10,"登录地方大陆,分数+10");
        return;
    }
    if (type <= _ISSTONE && scoreTypes[type] == 0 && type > _MEAT) {
        addScore(5, " 采集到新资源，分数+5");
        if (type == _ISGOLD) {
            addScore(10, " 采集到黄金，分数+10");
        }
    }

    if (type > _MEAT && type <= _ISSTONE) {
        scoreTypes[type] = scoreTypes[type] | 1;
        return;
    }

    int before = scoreTypes[type] / 100;
    scoreTypes[type] += num;

    if (type <= _MEAT) {
        int after = scoreTypes[type] / 100;
        int change = after - before;
        while (change > 0) {
            addScore(1, " 单种资源收集满100个，分数+1");
            change--;
        }
    }

    switch (type) {
    case _TECH:
        addScore(2, " 解锁新科技，分数+2");
        break;
    case _HUMAN1:
        addScore(1, " 生产普通单位，分数+1");
        break;
    case _HUMAN2:
        addScore(2, " 生产特殊单位，分数+2");
        break;
    case _BUILDING1:
        addScore(1, " 建造住房或农田，分数+1");
        break;
    case _BUILDING2:
        addScore(2, " 建造一般建筑，分数+2");
        break;
    case _KILL2:
        addScore(2 * num, (isConversion ? " 转换一般敌人，分数+" : " 击杀一般敌人，分数+") + QString::number(2 * num));
        break;
    case _KILL4:
        addScore(4 * num, (isConversion ? " 转换高级敌人，分数+" : " 击杀高级敌人，分数+") + QString::number(4 * num));
        break;
    case _DESTORY2:
        addScore(2 * num, (isConversion ? " 转换房屋或农田，分数+" : " 摧毁房屋或农田，分数+") + QString::number(2 * num));
        break;
    case _DESTORY4:
        addScore(4 * num, (isConversion ? " 转换一般建筑，分数+" : " 摧毁一般建筑，分数+") + QString::number(4 * num));
        break;
    case _DESTORY5:
        addScore(5 * num, (isConversion ? " 转换箭塔，分数+" : " 摧毁箭塔，分数+") + QString::number(5 * num));
        break;
    case _DESTORY10:
        addScore(10 * num, (isConversion ? " 转换主营，分数+" : " 摧毁主营，分数+") + QString::number(10 * num));
        break;
    default:
        break;
    }
}

bool tagObj::operator <(const tagObj &obj) const{
    return SN<obj.SN;
}

tagBuilding tagBuilding::toEnemy() {
    this->Cnt = -1;
    this->Project = -1;
    this->ProjectPercent = -1;
    return *this;
}

void tagHuman::cast_from(tagHuman taghuman) {
    this->DR = taghuman.DR;
    this->UR = taghuman.UR;
    this->BlockDR = taghuman.BlockDR;
    this->BlockUR = taghuman.BlockUR;
    this->DR0 = taghuman.DR0;
    this->UR0 = taghuman.UR0;
    this->NowState = taghuman.NowState;
    this->WorkObjectSN = taghuman.WorkObjectSN;
    this->Blood = taghuman.Blood;
    this->SN = taghuman.SN;
}

tagFarmer tagFarmer::toEnemy() {
    Resource = -1;
    DR0 = Double(-1);
    UR0 = Double(-1);
    return *this;
}

tagArmy tagArmy::toEnemy() {
    DR0 = Double(-1);
    UR0 = Double(-1);
    ConvertCooldown = -1;
    return *this;
}



Point::Point() {}

Point::Point(int x, int y) { this->x = x, this->y = y; }

Point::Point(const Point &board) { x = board.x, y = board.y; }

Point Point::operator +(const Point &ps) { return Point(x + ps.x, y + ps.y); }

Point Point::operator -(const Point &ps) { return Point(x - ps.x, y - ps.y); }

bool Point::operator ==(const Point &ps) const { return ps.x == x && ps.y == y; }

bool Point::operator <(const Point &ps) const { return x < ps.x && y < ps.y; }

tagInfo &tagInfo::operator=(const tagInfo &other) {
    if (this != &other) { // Check for self-assignment
        buildings = other.buildings;
        farmers = other.farmers;
        armies = other.armies;
        enemy_buildings = other.enemy_buildings;
        enemy_farmers = other.enemy_farmers;
        enemy_armies = other.enemy_armies;
        resources = other.resources;
        ins_ret = other.ins_ret;

        // Deep copy theMap array
        theMap=other.theMap;
        /*
            for (int i = 0; i < MAP_L; ++i) {
                for (int j = 0; j < MAP_U; ++j) {
                    theMap[i][j] = other.theMap[i][j];
                }
            }
            */
        GameFrame = other.GameFrame;
        civilizationStage = other.civilizationStage;
        Wood = other.Wood;
        Meat = other.Meat;
        Stone = other.Stone;
        Gold = other.Gold;
        Human_MaxNum = other.Human_MaxNum;
    }
    return *this;
}

void tagInfo::clear() {
    buildings.clear();
    farmers.clear();
    armies.clear();
    enemy_buildings.clear();
    enemy_farmers.clear();
    enemy_armies.clear();
    resources.clear();
    ins_ret.clear();
}

void tagGame::update(tagInfo *newinfo) {
    //控制ins_ret的大小小于100，若大于100，则优先删除旧值
   // QMutexLocker locker(&Locker);//之前是严格的帧同步，现在改成严格的非帧同步
    if (this->Info != NULL) {
        while (Info->ins_ret.size() > 100) {
            Info->ins_ret.erase(Info->ins_ret.begin());
        }
    }
    if (this->Info != NULL)
        newinfo->ins_ret = this->Info->ins_ret;
    Info = newinfo;
    //对内部打乱
    static const bool openHunYao = 1;
    //
    if (openHunYao) {
        WLHHunYao(Info->buildings);
        WLHHunYao(Info->farmers);
        WLHHunYao(Info->armies);
        WLHHunYao(Info->enemy_buildings);
        WLHHunYao(Info->enemy_farmers);
        WLHHunYao(Info->enemy_armies);
        WLHHunYao(Info->resources);
    }
}

bool tagGame::tryLock()
{
    return Locker.tryLock();
}

void tagGame::release()
{
    Locker.unlock();
}

void tagGame::insertInsRet(int id, instruction ins) {
    QMutexLocker locker(&Locker);
    this->Info->ins_ret.insert(make_pair(id, ins.ret));
}

tagInfo tagGame::getInfo() {
    QMutexLocker locker(&Locker);
    return *Info;
}

void tagGame::clearInsRet() {
    QMutexLocker locker(&Locker);
    Info->ins_ret.clear();
}

pixMemoryMap::pixMemoryMap(int w, int h) : width(w), height(h) {
    // 分配内存图空间
    MemoryMap.resize(width * height);
}

pixMemoryMap::pixMemoryMap() : width(0), height(0) {}

pixMemoryMap::pixMemoryMap(const pixMemoryMap &other) : width(other.width), height(other.height)
{

    MemoryMap = other.MemoryMap;
}

pixMemoryMap &pixMemoryMap::operator=(const pixMemoryMap &other)
{
    width = other.width;
    height = other.height;

    MemoryMap = other.MemoryMap;

    return *this;
}

void pixMemoryMap::setMemoryMap(int i, int j) {
    int index = i * height + j;
    MemoryMap[index] = 1;
}

char pixMemoryMap::getMemoryMap(int i, int j) {
    int index = i * height + j;
    return MemoryMap[index];
}

void pixMemoryMap::fillBlockMemoryMap()
{
    for (int i = 0;i < width / 2;i++)
    {
        for (int j = 0;j < height / 2;j++)
        {
            if (j * width >= height * (width / 2 - i))
            {
                setMemoryMap(i, j);
            }
        }
    }
    for (int i = width / 2;i < width;i++)
    {
        for (int j = 0;j < height / 2;j++)
        {
            if (j * width >= height * (i - width / 2))
            {
                setMemoryMap(i, j);
            }
        }
    }
    for (int i = 0;i < width / 2;i++)
    {
        for (int j = height / 2;j < height;j++)
        {
            if (j * width <= height * (i + width / 2))
            {
                setMemoryMap(i, j);
            }
        }
    }
    for (int i = width / 2;i < width;i++)
    {
        for (int j = height / 2;j < height;j++)
        {
            if (j * width <= -height * i + 3 * width * height / 2)
            {
                setMemoryMap(i, j);
            }
        }
    }
}

ImageResource::ImageResource(QPixmap pix) :pix(pix)
{
    if (pix.isNull()) {
        // 图片未成功加载，执行错误处理操作
        qDebug() << "fault";
    }
}

ImageResource::ImageResource()
{

}

conditionDevelop::conditionDevelop() {}

conditionDevelop::conditionDevelop(int civilization, int sort_building, Double needTimes, int need_Wood, int need_Food, int need_Stone, int need_Gold)
{
    this->civilization = civilization;
    this->sort_building = sort_building;
    this->need_Wood = need_Wood;
    this->need_Food = need_Food;
    this->need_Stone = need_Stone;
    this->need_Gold = need_Gold;
    this->times_second = needTimes;
}

void conditionDevelop::addPreCondition(conditionDevelop *con_need) { preCondition.push_back(con_need); }

void conditionDevelop::setCreatObjectAfterAction(int creatSort)
{
    isCreatObjectAction = true;
    creatObjectSort = creatSort;
}

void conditionDevelop::setCreatObjectAfterAction(int creatSort, int creatNum)
{
    setCreatObjectAfterAction(creatSort);
    creatObjectNum = creatNum;
}

bool conditionDevelop::executable(int wood, int food, int stone, int gold) { return wood >= need_Wood && food >= need_Food && stone >= need_Stone && gold >= need_Gold; }

void conditionDevelop::get_needResource(int &wood, int &food, int &stone, int &gold) { wood = need_Wood, food = need_Food, stone = need_Stone, gold = need_Gold; }

bool conditionDevelop::isShowable(int nowcivilization)
{
    if (civilization > nowcivilization) return false;

    for (list<conditionDevelop*>::iterator iter = preCondition.begin(); iter != preCondition.end(); iter++)
        if (!(*iter)->acttimes) return false;

    return true;
}

void conditionDevelop::finishAct() { acttimes++; }

bool conditionDevelop::isNeedCreatObject(int &creatSort, int &creatNum)
{
    creatSort = creatObjectSort;
    creatNum = creatObjectNum;
    return isCreatObjectAction;
}

bool conditionDevelop::isNeedCreatObject() { return isCreatObjectAction; }

int conditionDevelop::getActTimes() { return acttimes; }

st_upgradeLab::st_upgradeLab() {}

st_upgradeLab::~st_upgradeLab()
{
    while (headAct != endNode)
    {
        nowExecuteNode = headAct;
        headAct = headAct->nextDevAction;
        delete nowExecuteNode;
    }
    delete endNode;
}

void st_upgradeLab::setHead(conditionDevelop *head) { endNode = nowExecuteNode = headAct = head; }

void st_upgradeLab::push_back(conditionDevelop *node)
{
    endNode->nextDevAction = node;
    endNode = node;
}

void st_upgradeLab::endNodeAsOver() { endNode->nextDevAction = endNode; }

void st_upgradeLab::shift()
{
    if (nowExecuteNode != NULL)
    {
        overExecute();
        haveFinishedPhaseNum++;
        nowExecuteNode = nowExecuteNode->nextDevAction;
    }
}

bool st_upgradeLab::isShowAble(int nowcivilization)
{
    if (nowExecuteNode == NULL) return false;
    
    if (headAct != endNode && nowExecuteNode == nowExecuteNode->nextDevAction && nowExecuteNode->getActTimes() > 0)
        return false;
    
    return nowExecuteNode->isShowable(nowcivilization) && (!nowExecuting || nowExecuteNode == nowExecuteNode->nextDevAction);
}

bool st_upgradeLab::executable(int nowcivilization, int wood, int food, int stone, int gold) { return isShowAble(nowcivilization) && nowExecuteNode->executable(wood, food, stone, gold); }

void st_upgradeLab::beginExecute() { this->nowExecuting = true; }

void st_upgradeLab::overExecute() { this->nowExecuting = false; }

int st_upgradeLab::getPhaseTimes() { return this->haveFinishedPhaseNum; }

void st_upgradeLab::get_needResource(int &wood, int &food, int &stone, int &gold)
{
    if (nowExecuteNode != NULL)
        nowExecuteNode->get_needResource(wood, food, stone, gold);
    else wood = 0, food = 0, stone = 0, gold = 0;
}

bool st_upgradeLab::isNeedCreatObject() {
    if (nowExecuteNode != NULL) return nowExecuteNode->isNeedCreatObject();
    else return false;
}

st_buildAction::st_buildAction() {}

st_buildAction::~st_buildAction()
{
    if (buildCon != NULL)
    {
        delete buildCon;
        buildCon = NULL;
    }
}

void st_buildAction::finishBuild() { buildCon->finishAct(); }

void st_buildAction::finishAction(int actNum)
{
    actCon[actNum].nowExecuteNode->finishAct();
    actCon[actNum].shift();
}



Q_COREAPP_STARTUP_FUNCTION(ReadConfig)
void ReadConfig()
{
    // 1. 打开配置文件
    QFile file("config.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
       return;
    }
    // 2. 读取文件内容
    QByteArray jsonData = file.readAll();
    file.close();
    // 3. 解析JSON数据
    QJsonParseError parseError;
    QJsonDocument json_config = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        return;
    }
    // 4. 检查是否为JSON对象（而非数组）
    if (!json_config.isObject()) {
       return;
    }
    QJsonObject config=json_config.object();
    ApplyRuntimeConfigFromJson(config);


}
