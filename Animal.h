#ifndef ANIMAL_H
#define ANIMAL_H

#include <MoveObject.h>
#include <Resource.h>

class Animal:public MoveObject,public Resource
{
public:
    Animal();
    Animal(int Num,double DR,double UR);

private:

    int Friendly=1;
    //友好度 1为友好 2为敌对
};

#endif // ANIMAL_H
