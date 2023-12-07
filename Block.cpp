#include "Block.h"

std::list<Resource>* Block::block[17];
std::list<Resource>* Block::grayblock[17];
std::list<Resource>* Block::blackblock[17];
std::string Block::Blockname[17]={"Grass","Desert","Grass_Border_111","Grass_Border_112","Grass_Border_113","Grass_Border_114",
                                  "Grass_Border_121","Grass_Border_122","Grass_Border_123","Grass_Border_124","Grass_Border_125","Grass_Border_126",
                                  "Grass_Border_131","Grass_Border_132","Grass_Border_133","Grass_Border_134",
                                  "Grass_Border_140"};

Block::Block()
{
    this->Num=0;
    this->nowres=block[this->Num]->begin();
}

void Block::nextframe()
{

}
