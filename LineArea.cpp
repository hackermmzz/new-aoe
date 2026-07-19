#include "LineArea.h"


LineArea::LineArea(GameWidget *widget_)
{
    widget=widget_;
    triger=0;
    down=0;
}


void LineArea::onLeftMouseDown()
{
    triger=1;
    down=1;
    // current.areaType 需要在MainWidget中设置
}

void LineArea::onLeftMouseUp()
{
    down=0;
}


void LineArea::onMouseMove(int delta_x, int delta_y)
{
    if(triger&&down){
        current.data.push_back({widget->TranGlobalPosToDR(MouseX(),MouseY()),widget->TranGlobalPosToUR(MouseX(),MouseY())});
    }
}

void LineArea::Draw()
{
    // 绘制已完成的线性区域，根据区域类型使用不同颜色
    for(int i=0;i<lines.size();++i){
        auto&lineData=lines[i];
        QColor color = getAreaColor(lineData.areaType);
        for(int j=0;j+1<lineData.data.size();++j){
            auto&p0=lineData.data[j],&p1=lineData.data[j+1];
            widget->AddLine(p0[0],p0[1],p1[0],p1[1],color);
        }
    }
    
    // 绘制正在创建的线性区域，使用绿色
    if(triger)
    for(int j=0;j+1<current.data.size();++j){
        auto&p0=current.data[j],&p1=current.data[j+1];
        widget->AddLine(p0[0],p0[1],p1[0],p1[1],Qt::green);
    }
    //绘制所有待关联的对象
    for(auto*obj:coordinate){
        widget->AddEdge(obj->getDR(),obj->getUR(),obj->getCrashLength(),obj->getCrashLength(),Qt::red);
    }
    //
    for(auto&ele:relation){
        auto*obj=ele.first;
        if(coordinate.count(obj)==0){
            widget->AddEdge(obj->getDR(),obj->getUR(),obj->getCrashLength(),obj->getCrashLength(),Qt::darkRed);
        }
    }
}

void LineArea::onRightMouseClick()
{
    // 选择对象
    Coordinate*obj=Core::getObject(MouseX()/4,MouseY()/4);
    if(obj!=0)coordinate.insert(obj);
    else{
        //如果没选到对象,默认就是保存本次区域
        triger=0;
        if(current.data.size()>=2){
            lines.push_back(current);
            //将关联对象与当前区域进行关联
            for(auto*coor:coordinate){
                // 检查是否已存在相同类型的区域，如果存在则替换，否则添加
                bool found = false;
                for(auto it = relation.equal_range(coor); it.first != it.second; ++it.first) {
                    if(it.first->second.areaType == current.areaType) {
                        it.first->second = current;  // 替换相同类型的区域
                        found = true;
                        break;
                    }
                }
                if(!found) {
                    relation.insert({coor, current});  // 添加新区域
                }
            }
            coordinate.clear();
            //
            current.data.clear();
        }
    }
}

void LineArea::onRightMouseDown()
{
    // 如果正在绘制区域，则取消当前选择
    if(triger) {
        triger = false;
        down = false;
        current.data.clear();
        coordinate.clear();
    }
}

LineAreaData *LineArea::GetPosIn(Double dr, Double ur)
{

}


string LineArea::Name()
{
    return "Line";
}

// 设置当前绘制的区域类型
void LineArea::setCurrentAreaType(int type) {
    current.areaType = type;
}

// 根据区域类型获取颜色
QColor LineArea::getAreaColor(int areaType) {
    switch(areaType) {
        case 1: return Qt::blue;    // 巡逻区域
        default: return Qt::gray;   // 原有区域
    }
}

// 设置要绑定的单位
void LineArea::setTargetUnits(const vector<Coordinate*>& units) {
    coordinate.clear();
    for (Coordinate* unit : units) {
        coordinate.insert(unit);
    }
}
