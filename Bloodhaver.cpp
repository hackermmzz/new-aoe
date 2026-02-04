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


void BloodHaver::updateBlood(int damage){
    double maxBlood=(double)getMaxBlood();
    Blood -= (double)damage/maxBlood; Blood=max(min(1.0,Blood),0.0);
}
