@echo off
mkdir build
pushd build
cl -Zi /I"../includes" ../source/main.cpp ../source/glad.c user32.lib gdi32.lib opengl32.lib
popd