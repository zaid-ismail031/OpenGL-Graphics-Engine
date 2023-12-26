#ifndef TEXT_H
#define TEXT_H

#include <windows.h>
#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>
#include <string>
#include <map>
#include "shaders.h"

typedef struct 
{
    unsigned int textureId; // texture id storing character
    glm::ivec2 size; // size of char
    glm::ivec2 bearing; // distance from origin to top left of char
    unsigned int advance; // distance from char's origin to next char's origin
} Character;

//-------------------------------------------------------------------------------------------------------------

class TextRenderer 
{
    public:
        TextRenderer();
        TextRenderer(int height);

        bool loadFont(FT_Library& ft, std::string path);
        void render(Shader shader, std::string text, float xPos, float yPos, float scale, glm::vec3 color);

        void cleanup();

    private:
        int height;
        std::map<char, Character> chars;

        //ArrayObject VAO;
};

//-------------------------------------------------------------------------------------------------------------

#endif