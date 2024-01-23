#ifndef GLOBJECTLOADER_H
#define GLOBJECTLOADER_H

#include <windows.h>
#include <OBJ_Loader.h>
#include <glad/glad.h>

struct MeshBuffers 
{
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    int indexCount;
};

class ObjectLoader 
{
    public:
        objl::Loader loader;
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
        std::vector<MeshBuffers> bufferVector;
        ObjectLoader(const char* objectPath);
        int getVertexCount();
        int getIndexCount();
        int getMeshCount();
        void createVertexArrayObject();
        void createElementBufferObject(objl::Mesh mesh, MeshBuffers *buffers);
        void loadAllMeshes();
};

#endif