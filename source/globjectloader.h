#ifndef GLOBJECTLOADER_H
#define GLOBJECTLOADER_H

#include <OBJ_Loader.h>
#include <glad/glad.h>

class ObjectLoader 
{
    public:
        objl::Loader loader;
        unsigned int VAO;
        unsigned int VBO;
        ObjectLoader(const char* objectPath);
        int getVertexCount();
        void createVertexArrayObject();
};

#endif