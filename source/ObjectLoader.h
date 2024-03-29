#ifndef GLOBJECTLOADER_H
#define GLOBJECTLOADER_H

#include <windows.h>
#include <OBJ_Loader.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

struct MeshBuffers 
{
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    int indexCount;
    int vertexCount;
};

class ObjectLoader 
{
public:
    ObjectLoader(const char* objectPath, bool normalMapping);
    void loadAllMeshes();
    int getVertexCount();
    int getIndexCount();
    int getMeshCount();
    objl::Material loadMaterial(int index);
    std::vector<MeshBuffers> bufferVector;
    bool normalMapping;

private:
    objl::Loader loader;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    std::vector <objl::Material> materialVector;
    void createVertexArrayObject();
    void createElementBufferObject(objl::Mesh mesh, MeshBuffers *buffers);
    void vertexArrayObjectNormalMapping(objl::Mesh mesh, MeshBuffers* buffers);
    void calculateTangents(std::vector<float>* newVertexData, 
        objl::Mesh* mesh);
};

#endif