@echo off
gcc main.c -I./glfw -L./glfw/lib -l:libglfw3.a -lopengl32 -lglu32 -lgdi32 -luser32 -lkernel32 -lwinmm -lwinpthread -mwindows -o main.exe
if %errorlevel% neq 0 pause
