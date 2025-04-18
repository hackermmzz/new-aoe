#ifndef BLOCK_H
#define BLOCK_H

#include "Coordinate.h"

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
    void reset();
    void resetOffset();

    // 用于分辨地块种类的变量
    int Num;

    bool Visible=false;//是否可见
    bool Explored=false;//是否被探索

    static std::list<ImageResource> *block[BLOCK_COUNT];
    static std::list<ImageResource> *grayblock[BLOCK_COUNT];
    static std::list<ImageResource> *blackblock[BLOCK_COUNT];
    static std::string Blockname[BLOCK_COUNT];
    // int Environment=0;//当前地皮编号 考虑用num代替
    //用基类中num代替

    int getMapHeight() const;
    void setMapHeight(int value);

    int getMapPattern() const;
    void setMapPattern(int value);

    int getMapResource() const;
    void setMapResource(int value);

    int getOffsetX() const;
    void setOffsetX(int value);

    int getOffsetY() const;
    void setOffsetY(int value);

    int getMapType() const;
    void setMapType(int value);

private:
    // 可优化成char类型
    int Type;               // 地图块种类（地形凹凸）
    int Pattern;            // 地图块样式（草地、沙漠等）
    int Height;             // 地图块高度
    int OffsetX, OffsetY;   // 地图块偏移量
    int Resource;           // 地图块存放的资源类型（默认为无资源，即空地）

};

#endif // BLOCK_H
