#include "Core.h"

Core::Core()
{

}

void Core::gameUpdate(Map* map, Player* player[], int** memorymap)
{
    std::list<Animal*>::iterator animaliter=map->animal.begin();
    while(animaliter!=map->animal.end())
    {
        (*animaliter)->nextframe();
        animaliter++;

    }
}
