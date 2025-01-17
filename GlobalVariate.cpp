#include "GlobalVariate.h"
#include "config.h"
#include <QDirIterator>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QtWidgets>
using namespace std;

bool isExamining = EXAMINE_MODE;

map<std::string, std::list<QPixmap>> resMap;
map<string, QSoundEffect*> SoundMap;
std::queue<string> soundQueue;

std::list<Coordinate*> drawlist;

Score usrScore=Score(0);
Score enemyScore=Score(1);
Coordinate *nowobject=NULL;
std::queue<st_DebugMassage>debugMassagePackage;
std::map<QString , int>debugMessageRecord;

bool only_debug_Player0 = IS_DEBUGTEXT_ONLYNOWPLAYER;
bool filterRepetitionMessage = IS_FILTER_DEBUGMESSAGE;

std::string direction[5]={"Down","LeftDown","Left","LeftUp","Up"};

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
    //    qDebug()<<"路径中一共存在"<<dirCount<<"张图片";
    if(dirCount <= 0)
    {
        qDebug()<<"路径内无png和gif文件";
        return -1;
    }

    //获取分隔符
    //QChar separator = QDir::separator();
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
        //文件全路径
        QString filePath = path + separator + fileName;

        //debug用，可删
        QStringList ImageList;
        ImageList.append(filePath);


        //获取文件后缀
        QFileInfo testInfo(filePath);
        QString testSuffix = testInfo.suffix();
        int index = fileName.lastIndexOf("_");
        QString imageMapName;
        imageMapName = fileName.left(index);

        //debug用，可删
        //        qDebug()<<"输出第"<<i + 1<<"张图片的信息：";
        //        qDebug()<<"图片所在List名为："<<imageMapName;
        //        qDebug()<<"图片是所在List中第"<<fileName.right(7).left(3)<<"张";

        std::string tmpListName = imageMapName.toStdString();
        resMap[tmpListName].push_back(QPixmap(filePath));
    }
    ////////////////////////////////加载额外资源
    resMap["Button_Dock"].push_back(QPixmap("res/Dock3.png"));
    resMap["Dock"].push_back(QPixmap("res/Dock3.png"));
    ////////////////////////////////
    //    qDebug()<<"return后自动调用析构函数，将函数内临时对象析构。";

//查看rcc
//    std::map<string, list<QPixmap>>::iterator iter,itere;
//    iter = resMap.begin();
//    itere = resMap.end();
//    while(iter != itere)
//    {
//        qDebug()<<QString::fromStdString(iter->first);
//        iter++;
//    }
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
    //    qDebug()<<"路径中一共存在"<<dirCount<<"个音频";
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
        //文件全路径
        QString filePath = path + separator + fileName;

        //debug用，可删
        QStringList SoundList;
        SoundList.append(filePath);


        //获取文件后缀
        QFileInfo testInfo(filePath);
        QString testSuffix = testInfo.suffix();
        int index = fileName.lastIndexOf(".");
        QString SoundMapName;
        SoundMapName = fileName.left(index);

        //debug用，可删
        //        qDebug()<<"输出第"<<i + 1<<"个音频的信息：";
        //        qDebug()<<"音频所对应String为："<<SoundMapName;

        std::string tmpMapName = SoundMapName.toStdString();

        int volume = 50;
        QSoundEffect* qSoundEffect = new QSoundEffect();

        filePath ="qrc"+filePath;

        qSoundEffect->setSource(QUrl(filePath));
        qSoundEffect->setVolume(volume);

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

// 城镇中心附近7*7范围
int MidX = MAP_L / 2;
int MidY = MAP_U / 2;
int MAP_LSide[2] = {MidX - 7, MidX + 4};
int MAP_USide[2] = {MidY - 7, MidY + 4};

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

double countdistance(double L, double U, double L0, double U0)
{
    return sqrt((L-L0)*(L-L0)+(U-U0)*(U-U0));
}
bool isNear_Manhattan( double dr , double ur , double dr1  , double ur1 , double distance )
{
    return fabs(dr - dr1)<=distance && fabs(ur - ur1)<=distance;
}


void flipResource(std::list<ImageResource> *currentlist, std::list<ImageResource> *targetlist)
{
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

double calculateManhattanDistance(double x1, double y1, double x2, double y2)
{
    return fabs(x1-x2)+fabs(y1-y2);
}

void calMirrorPoint( double& dr , double &ur , double dr_mirror, double ur_mirror , double dis)
{
    double dr_deta = dr_mirror-dr, ur_deta = ur_mirror - ur;
    double total = fabs(dr_deta)+fabs(ur_deta);

    dr = dr_mirror+dr_deta/total*dis;
    ur = ur_mirror+ur_deta/total*dis;
}

double trans_BlockPointToDetailCenter( int p )
{
    return (p+0.5)*BLOCKSIDELENGTH;
}

void call_debugText(QString color, QString content,int playerID)
{
    if( !isExamining && (!only_debug_Player0 || playerID==NOWPLAYERREPRESENT || playerID == REPRESENT_BOARDCAST_MESSAGE) )
    {
        if(  !filterRepetitionMessage || debugMessageRecord[content] == 0 || color == "black"|| color == "green" )
        {
            debugMassagePackage.push(st_DebugMassage(color, content));
            debugMessageRecord[content] = g_frame;
        }
    }
}
//*************************************************************

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
instruction::instruction(int type,int SN,double L,double U){
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

int sgn(double __x)
{
    if(__x > 0) return 1;
    else if(__x < 0) return -1;
    else return 0;
}
