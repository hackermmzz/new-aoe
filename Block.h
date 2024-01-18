#ifndef BLOCK_H
#define BLOCK_H

#include <Coordinate.h>

class Block:public Coordinate
{
public:
    Block();

    static std::list<ImageResource>* getblock(int i) {
        return block[i];
    }

    static std::list<ImageResource>* getgrayblock(int i)
    {
        return grayblock[i];
    }

    static std::list<ImageResource>* getblackblock(int i)
    {
        return blackblock[i];
    }

    static void setblock(int i, std::list<ImageResource>* newValue) {
        block[i] = newValue;
    }

    static void setgrayblock(int i,std::list<ImageResource>* newValue)
    {
        grayblock[i]=newValue;
    }

    static void setblackblock(int i,std::list<ImageResource>* newValue)
    {
        blackblock[i]=newValue;
    }

    static void allocateblock(int i)
    {
        block[i]=new std::list<ImageResource>;
    }
    static void allocategrayblock(int i)
    {
        grayblock[i]=new std::list<ImageResource>;
    }
    static void allocateblackblock(int i)
    {
        blackblock[i]=new std::list<ImageResource>;
    }

    static void deallocateblock(int i) {
        delete block[i];
        block[i] = nullptr;
    }
    static void deallocategrayblock(int i) {
        delete grayblock[i];
        grayblock[i] = nullptr;
    }
    static void deallocateblackblock(int i) {
        delete blackblock[i];
        blackblock[i] = nullptr;
    }
    static std::string getBlockname(int num)
    {
        return Blockname[num];
    }

    void nextframe();

    int Num;

    bool Visible=false;//是否可见
    bool Explored=false;//是否被探索

    static std::list<ImageResource> *block[17];
    static std::list<ImageResource> *grayblock[17];
    static std::list<ImageResource> *blackblock[17];
    static std::string Blockname[17];
    //    int Environment=0;//当前地皮编号 考虑用num代替
    //用基类中num代替

    int getMapHeight() const;
    void setMapHeight(int value);

private:
    int MapType;        // 地图块种类
    int MapPattern;     // 地图块样式
    int MapHeight;      // 地图块高度
    int MapOffset;      // 地图块偏移量
    int MapResource;    // 地图块存放的资源类型

};

#endif // BLOCK_H
