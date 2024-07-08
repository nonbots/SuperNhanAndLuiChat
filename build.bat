@echo off 
cls
 cl /nologo /std:c17 -external:W0 -external:IC:/raylib-5.0_win64_msvc16/include app.c C:/raylib-5.0_win64_msvc16/lib/raylib.lib winmm.lib gdi32.lib User32.lib Shell32.lib -MD -Wall && start app.exe 

