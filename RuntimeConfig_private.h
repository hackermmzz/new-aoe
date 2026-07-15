#ifndef RUNTIMECONFIG_PRIVATE_H
#define RUNTIMECONFIG_PRIVATE_H

#include <QString>

/** 仅由 GlobalVariate.cpp 包含：命令行覆盖 json 中的值。 */
void RuntimeConfig_setIsExamining(bool v);
void RuntimeConfig_setOffScreen(bool v);
void RuntimeConfig_setINITIAL_FREQUENCY(int v);
void RuntimeConfig_setFixedMapFile(const QString& v);

#endif
