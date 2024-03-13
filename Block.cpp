#include "Block.h"

std::list<ImageResource>* Block::block[BLOCK_COUNT];
std::list<ImageResource>* Block::grayblock[BLOCK_COUNT];
std::list<ImageResource>* Block::blackblock[BLOCK_COUNT];
std::string Block::Blockname[BLOCK_COUNT]={"Grass","Desert",
                                  "Grass_Border_111","Grass_Border_112","Grass_Border_113","Grass_Border_114",
                                  "Grass_Border_121","Grass_Border_122","Grass_Border_123","Grass_Border_124","Grass_Border_125","Grass_Border_126",
                                  "Grass_Border_131","Grass_Border_132","Grass_Border_133","Grass_Border_134",
                                  "Grass_Border_140",
                                  "Grass_002","Grass_003","Grass_004","Grass_005","Grass_006","Grass_007",
                                  "Grass_008","Grass_009","Grass_010","Grass_011","Grass_012","Grass_013"};

Block::Block()
{
    this->Num = 0;
    this->nowres = block[this->Num]->begin();
    this->OffsetX = 0;
    this->OffsetY = 0;
    this->Type = MAPTYPE_FLAT;
    this->Pattern = MAPPATTERN_GRASS;
    this->Height = MAPHEIGHT_FLAT;
    this->Resource = RESOURCE_EMPTY;
}

void Block::nextframe()
{

}

int Block::getMapHeight() const
{
    return Height;
}

void Block::setMapHeight(int value)
{
    Height = value;
}

int Block::getMapPattern() const
{
    return Pattern;
}

void Block::setMapPattern(int value)
{
    Pattern = value;
}

int Block::getMapResource() const
{
    return Resource;
}

void Block::setMapResource(int value)
{
    Resource = value;
}

int Block::getOffsetX() const
{
    return OffsetX;
}

void Block::setOffsetX(int value)
{
    OffsetX = value;
}

int Block::getOffsetY() const
{
    return OffsetY;
}

void Block::setOffsetY(int value)
{
    OffsetY = value;
}

int Block::getMapType() const
{
    return Type;
}

void Block::setMapType(int value)
{
    Type = value;
}
