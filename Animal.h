#ifndef ANIMAL_H
#define ANIMAL_H

#include <MoveObject.h>
#include <Resource.h>

class Animal:public MoveObject,public Resource
{
private:
    int Friendly=1;
    //友好度 1为友好 2为敌对
    
    //以下为图片资源
    static std::list<ImageResource> *Walk[5][8];

    static std::list<ImageResource> *Stand[5][8];

    static std::list<ImageResource> *Attack[5][8];

    static std::list<ImageResource> *Die[5][8];

    static std::list<ImageResource> *Run[5][8];

    static std::string Animalname[5];
    static std::string Animalcarcassname[5];
    static std::string AnimalDisplayName[5];
    
public:
    Animal();
    Animal(int Num,double DR,double UR);

};

#endif // ANIMAL_H
