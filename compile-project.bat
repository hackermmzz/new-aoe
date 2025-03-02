@echo off
REM 设置Qt环境变量
set PATH=C:\ProgramData\Qt5.9.2\5.9.2\mingw53_32\bin;C:\ProgramData\Qt5.9.2\Tools\mingw530_32\bin;%PATH%
set QTDIR=C:\ProgramData\Qt5.9.2\5.9.2\mingw53_32

echo 正在清理之前的构建...
if exist Makefile (
    mingw32-make clean
)

echo 正在运行qmake...
qmake -spec win32-g++ CONFIG+=debug CONFIG+=qml_debug

echo 正在编译项目...
mingw32-make

echo.
if %ERRORLEVEL% EQU 0 (
    echo 编译成功！按任意键继续...
) else (
    echo 编译失败，错误代码：%ERRORLEVEL%。按任意键继续...
)
pause > nul 