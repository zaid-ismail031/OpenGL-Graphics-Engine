@echo off
mkdir build
pushd build
cl -Zi /std:c++20 /I"../linking/includes" /EHsc ../source/MainWindow.cpp ../source/Glad.c ../source/Shaders.cpp ../source/Text.cpp ../source/Textures.cpp ../source/ObjectLoader.cpp user32.lib gdi32.lib opengl32.lib ../linking/libs/freetype/freetype.lib
popd