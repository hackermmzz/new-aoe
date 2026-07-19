#include "Bloodhaver.h"

int BloodHaver::getDEF(int attackType_got)
{
    if(attackType_got == ATTACKTYPE_CLOSE)
        return defence_close;
    else if(attackType_got == ATTACKTYPE_SHOOT)
        return defence_shoot;
    else
        return 0;
}


void BloodHaver::updateBlood(Double damage){
    Double maxBlood=(Double)getMaxBlood();
    Blood -= damage/maxBlood; Blood=max(min(Double(1),Blood),Double::Zero());
}
