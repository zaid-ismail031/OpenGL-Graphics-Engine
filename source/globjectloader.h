#ifndef GLOBJECTLOADER_H
#define GLOBJECTLOADER_H

#include <windows.h>
#include <OBJ_Loader.h>
#include <glad/glad.h>

class ObjectLoader 
{
    public:
        objl::Loader loader;
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
        ObjectLoader(const char* objectPath);
        int getVertexCount();
        int getIndexCount();
        void createVertexArrayObject();
        void createElementBufferObject();
};

#endif