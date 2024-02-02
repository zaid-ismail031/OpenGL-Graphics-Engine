#include "Textures.h"
#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>

//-------------------------------------------------------------------------------------------------------------

Texture::Texture(const char* texturePath) 
{
    // Generate texturee
    glGenTextures(1, &this->textureID);
    glBindTexture(GL_TEXTURE_2D, this->textureID);

    // Use 2D texture for S and T axes, with texture wrapping option GL_MIRRORED_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture and mipmap filtering modes (GL_NEAREST = point filtering, GL_LINEAR = bilinear filtering) for magnifying and minifying operations
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image for texture, using stb_image.h image loading library
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(texturePath, &width, &height, &nrChannels, 0);

    if (data) 
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else 
    {
        OutputDebugStringA("Failed to load texture\n");
    }

    stbi_image_free(data);
}

//-------------------------------------------------------------------------------------------------------------

void Texture::use() 
{
    glBindTexture(GL_TEXTURE_2D, this->textureID);
}

//-------------------------------------------------------------------------------------------------------------