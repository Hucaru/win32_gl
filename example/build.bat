@echo off
:: Init env
set VSTOOLS="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

if not exist %VSTOOLS% (
    echo VS Build Tools are missing!
    exit
)

call %VSTOOLS%

:: Compile options
set COMPILER_FLAGS=/Zi /Fe: example.exe /FC /EHsc
set INCLUDE_DIR=/I..\include\ /I.
set CPP_FILES=main.cpp glad.c draw.cpp
set LIBS= Opengl32.lib User32.lib Gdi32.lib

:: Compile
cl.exe %COMPILER_FLAGS% %INCLUDE_DIR% %CPP_FILES% %LIBS%

exit