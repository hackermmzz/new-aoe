@echo off
REM 设置Qt环境变量
set PATH=C:\ProgramData\Qt5.9.2\5.9.2\mingw53_32\bin;C:\ProgramData\Qt5.9.2\Tools\mingw530_32\bin;%PATH%
set QTDIR=C:\ProgramData\Qt5.9.2\5.9.2\mingw53_32

echo Qt 5.9.2 环境已配置
echo QTDIR = %QTDIR%
echo.
echo 可用Qt命令:
echo  - qmake
echo  - mingw32-make
echo  - designer
echo  - assistant
echo  - uic
echo  - moc
echo  - rcc
echo.
echo 输入'exit'退出
echo.

cmd /k 