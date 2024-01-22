#include "globjectloader.h"
#include <vector>

ObjectLoader::ObjectLoader(const char* objectPath) 
{
    this->loader.LoadFile(objectPath);
}

int ObjectLoader::getVertexCount() 
{
    int vertexCount = 0;

    for (int i = 0; i < this->loader.LoadedMeshes.size(); i++) 
    {
        vertexCount += this->loader.LoadedMeshes[i].Vertices.size();
    }

    return vertexCount;
}


int ObjectLoader::getIndexCount() 
{
    int indexCount = 0;

    for (int i = 0; i < this->loader.LoadedMeshes.size(); i++) 
    {
        indexCount += this->loader.LoadedMeshes[i].Indices.size();
    }

    return indexCount;
}

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


void ObjectLoader::createElementBufferObject() 
{
    if (this->loader.LoadedMeshes[0].Vertices.empty()) 
    {
        std::cout << "Error: No vertices in the mesh!" << std::endl;
        return;
    }

    std::vector<unsigned int> indexData;
    std::vector<float> vertexData;

    for (int i = 0; i < this->loader.LoadedMeshes.size(); i++) 
    {
        objl::Mesh mesh = this->loader.LoadedMeshes[i];
        
        for(int index = 0; index < mesh.Indices.size(); index++)
        {
            indexData.push_back(mesh.Indices[index]);
        }
        
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

    // Element Buffer Object
    glGenBuffers(1, &this->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(unsigned int), indexData.data(), GL_STATIC_DRAW);
}