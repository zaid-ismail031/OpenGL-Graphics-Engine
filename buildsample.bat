@echo off
mkdir buildsample
pushd buildsample
cl -Zi /I"../includes" ../source/sample.cpp ../source/glad.c user32.lib gdi32.lib opengl32.lib
popd