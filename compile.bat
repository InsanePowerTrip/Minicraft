@echo off
gcc main.c ^
 -I. ^
 -Iglfw ^
 -Lglfw/lib ^
 -lglfw3dll ^
 -lopengl32 ^
 -lglu32 ^
 -lgdi32 ^
 -mwindows ^
 -o cube.exe
if %errorlevel% neq 0 pause