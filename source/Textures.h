#include <windows.h>
#include <glad/glad.h>

#ifndef TEXTURES_H
#define TEXTURES_H

class Texture 
{
    public:
    
    // the program ID
    unsigned int textureID;
    // constructor
    Texture(const char* texturePath);
    // use/activate texture
    void use();
};


#endif