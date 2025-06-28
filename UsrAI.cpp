#include "UsrAI.h"
#include <iostream>
tagGame tagUsrGame;
ins UsrIns;
/*##########DO NOT MODIFY THE CODE ABOVE##########*/
int id = -1; //使用全局变量记录指令id
tagInfo info;
void UsrAI::processData()
{
    info = getInfo();
    if (id != -1) {
        cout << "id: " << id << " 返回值: " << info.ins_ret[id] << endl;
        // 输出-3，表示指定位置越界
        id = -1; // 重置id
    }
    for (tagFarmer& farmer : info.farmers) {
        if (farmer.NowState == HUMAN_STATE_IDLE) {
            // 记录指令id，在下一次调用processData时查看返回值
            id = HumanMove(farmer.SN, 256 * BLOCKSIDELENGTH, 256 * BLOCKSIDELENGTH);
            break;
        }
    }
}
