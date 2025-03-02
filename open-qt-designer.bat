@echo off
REM 设置Qt环境变量
set PATH=C:\ProgramData\Qt5.9.2\5.9.2\mingw53_32\bin;C:\ProgramData\Qt5.9.2\Tools\mingw530_32\bin;%PATH%
set QTDIR=C:\ProgramData\Qt5.9.2\5.9.2\mingw53_32

echo 正在启动Qt Designer...
start "" "C:\ProgramData\Qt5.9.2\5.9.2\mingw53_32\bin\designer.exe"

echo 已启动Qt Designer 