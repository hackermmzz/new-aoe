#ifndef BLOCK_H
#define BLOCK_H

#include <Coordinate.h>

class Block:public Coordinate
{
public:
    Block();

    static std::list<Resource>* getblock(int i) {
        return block[i];
    }

    static std::list<Resource>* getgrayblock(int i)
    {
        return grayblock[i];
    }

    static std::list<Resource>* getblackblock(int i)
    {
        return blackblock[i];
    }

    static void setblock(int i, std::list<Resource>* newValue) {
        block[i] = newValue;
    }

    static void setgrayblock(int i,std::list<Resource>* newValue)
    {
        grayblock[i]=newValue;
    }

    static void setblackblock(int i,std::list<Resource>* newValue)
    {
        blackblock[i]=newValue;
    }

    static void allocateblock(int i)
    {
        block[i]=new std::list<Resource>;
    }
    static void allocategrayblock(int i)
    {
        grayblock[i]=new std::list<Resource>;
    }
    static void allocateblackblock(int i)
    {
        blackblock[i]=new std::list<Resource>;
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

    static std::list<Resource> *block[17];
    static std::list<Resource> *grayblock[17];
    static std::list<Resource> *blackblock[17];
    static std::string Blockname[17];
    //    int Environment=0;//当前地皮编号 考虑用num代替
    //用基类中num代替


};

#endif // BLOCK_H
