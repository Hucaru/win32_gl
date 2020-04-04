@echo off
:: Init env
set VSTOOLS="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

if not exist %VSTOOLS% (
    echo VS Build Tools are missing!
    exit
)

call %VSTOOLS%

:: Call debugger
devenv example.exe