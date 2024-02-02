#include "ObjectLoader.h"
#include <vector>

//-------------------------------------------------------------------------------------------------------------

ObjectLoader::ObjectLoader(const char* objectPath) 
{
    this->loader.LoadFile(objectPath);
    this->VAO = 0;
    this->VBO = 0;
    this->EBO = 0;
}

//-------------------------------------------------------------------------------------------------------------

int ObjectLoader::getVertexCount() 
{
    int vertexCount = 0;

    for (int i = 0; i < this->loader.LoadedMeshes.size(); i++) 
    {
        vertexCount += this->loader.LoadedMeshes[i].Vertices.size();
    }

    return vertexCount;
}

//-------------------------------------------------------------------------------------------------------------

int ObjectLoader::getIndexCount() 
{
    int indexCount = 0;

    for (int i = 0; i < this->loader.LoadedMeshes.size(); i++) 
    {
        indexCount += this->loader.LoadedMeshes[i].Indices.size();
    }

    return indexCount;
}

//-------------------------------------------------------------------------------------------------------------

int ObjectLoader::getMeshCount() 
{
    return this->loader.LoadedMeshes.size();
}

//-------------------------------------------------------------------------------------------------------------

void ObjectLoader::createVertexArrayObject() 
{

    if (this->loader.LoadedMeshes[0].Vertices.empty()) 
    {
        std::cout << "Error: No vertices in the mesh!" << std::endl;
        return;
    }

    std::vector<float> vertexData;

    for (int i = 0; i < this->loader.LoadedMeshes.size(); i++) 
    {
        objl::Mesh mesh = this->loader.LoadedMeshes[i];
        
        for(int vertex = 0; vertex < mesh.Vertices.size(); vertex++)
        {
            vertexData.push_back(mesh.Vertices[vertex].Position.X);
            vertexData.push_back(mesh.Vertices[vertex].Position.Y);
            vertexData.push_back(mesh.Vertices[vertex].Position.Z);
            vertexData.push_back(mesh.Vertices[vertex].Normal.X);
            vertexData.push_back(mesh.Vertices[vertex].Normal.Y);
            vertexData.push_back(mesh.Vertices[vertex].Normal.Z);
            vertexData.push_back(mesh.Vertices[vertex].TextureCoordinate.X);
            vertexData.push_back(mesh.Vertices[vertex].TextureCoordinate.Y);
        }
    }
    

    // Vertex Buffer Object and Vertex Array Object
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //// normal coord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);
}

//-------------------------------------------------------------------------------------------------------------

void ObjectLoader::createElementBufferObject(objl::Mesh mesh, MeshBuffers *buffers) 
{
    if (this->loader.LoadedMeshes[0].Vertices.empty()) 
    {
        std::cout << "Error: No vertices in the mesh!" << std::endl;
        return;
    }

    std::vector<unsigned int> indexData;
    std::vector<float> vertexData;
        
    for (int index = 0; index < mesh.Indices.size(); index++)
    {
        indexData.push_back(mesh.Indices[index]);
    }
    
    for (int vertex = 0; vertex < mesh.Vertices.size(); vertex++)
    {
        vertexData.push_back(mesh.Vertices[vertex].Position.X);
        vertexData.push_back(mesh.Vertices[vertex].Position.Y);
        vertexData.push_back(mesh.Vertices[vertex].Position.Z);
        vertexData.push_back(mesh.Vertices[vertex].Normal.X);
        vertexData.push_back(mesh.Vertices[vertex].Normal.Y);
        vertexData.push_back(mesh.Vertices[vertex].Normal.Z);
        vertexData.push_back(mesh.Vertices[vertex].TextureCoordinate.X);
        vertexData.push_back(mesh.Vertices[vertex].TextureCoordinate.Y);
    }

    // Vertex Buffer Object and Vertex Array Object
    glGenVertexArrays(1, &buffers->VAO);
    glGenBuffers(1, &buffers->VBO);

    glBindVertexArray(buffers->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO);

    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    // Element Buffer Object
    glGenBuffers(1, &buffers->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(unsigned int), indexData.data(), GL_STATIC_DRAW);

    buffers->indexCount = mesh.Indices.size();

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //// normal coord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);
}

//-------------------------------------------------------------------------------------------------------------

void ObjectLoader::loadAllMeshes() 
{
    for (int i = 0; i < this->loader.LoadedMeshes.size(); i++) 
    {
        MeshBuffers buffer;
        createElementBufferObject(this->loader.LoadedMeshes[i], &buffer);
        bufferVector.push_back(buffer);
    }
}

//-------------------------------------------------------------------------------------------------------------

void ObjectLoader::loadMaterial(int index, std::vector<MaterialProperties> *materialProps) 
{
    objl::Material material = this->loader.LoadedMeshes[index].MeshMaterial;

    materialProps->push_back(MaterialProperties{
        .name = material.name,
        .Ka = {material.Ka.X, material.Ka.Y, material.Ka.Z},
        .Kd = {material.Kd.X, material.Kd.Y, material.Kd.Z},
        .Ks = {material.Ks.X, material.Ks.Y, material.Ks.Z},
        .Ke = {material.Ke.X, material.Ke.Y, material.Ke.Z},
        .Ns = material.Ns,
        .Ni = material.Ni,
        .d = material.d,
        .illum = material.illum,
        .map_Ka = material.map_Ka,
        .map_Kd = material.map_Kd,
        .map_Ks = material.map_Ks,
        .map_Ke = material.map_Ke,
        .map_Ns = material.map_Ns,
        .map_d = material.map_d,
        .map_bump = material.map_bump,
        .refl = material.refl
    });
}

//-------------------------------------------------------------------------------------------------------------