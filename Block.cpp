#include "Block.h"

std::list<ImageResource>* Block::block[BLOCK_COUNT];
std::list<ImageResource>* Block::grayblock[BLOCK_COUNT];
std::list<ImageResource>* Block::blackblock[BLOCK_COUNT];

std::string Block::Blockname[BLOCK_COUNT] = {
    "Grass",            // 0
    "Desert",           // 1
    "Grass_Border_111", // 2
    "Grass_Border_112", // 3
    "Grass_Border_113", // 4
    "Grass_Border_114", // 5
    "Grass_Border_121", // 6
    "Grass_Border_122", // 7
    "Grass_Border_123", // 8
    "Grass_Border_124", // 9
    "Grass_Border_125", // 10
    "Grass_Border_126", // 11
    "Grass_Border_131", // 12
    "Grass_Border_132", // 13
    "Grass_Border_133", // 14
    "Grass_Border_134", // 15
    "Grass_Border_140", // 16
    "Grass_002",        // 17
    "Grass_003",        // 18
    "Grass_004",        // 19
    "Grass_005",        // 20
    "Grass_006",        // 21
    "Grass_007",        // 22
    "Grass_008",        // 23
    "Grass_009",        // 24
    "Grass_010",        // 25
    "Grass_011",        // 26
    "Grass_012",        // 27
    "Grass_013",        // 28
    "Link_Deep_000",    // 29
    "Link_Deep_002",    // 30
    "Link_Deep_004",    // 31
    "Link_Deep_006",    // 32
    "Link_Deep_008",    // 33
    "Link_Deep_010",    // 34
    "Link_Deep_012",    // 35
    "Link_Deep_014",    // 36
    "Link_Deep_016",    // 37
    "Link_Deep_018",    // 38
    "Link_Deep_020",    // 39
    "Link_Deep_007",    //40
};


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

void Block::reset()
{
    resetOffset();
    setMapType(MAPTYPE_FLAT);
}

void Block::resetOffset()
{
    setOffsetX(0);
    setOffsetY(0);
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
