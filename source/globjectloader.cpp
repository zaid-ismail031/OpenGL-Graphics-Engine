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

void ObjectLoader::createVertexArrayObject() 
{
    // Vertex Buffer Object and Vertex Array Object
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

    if (this->loader.LoadedMeshes[0].Vertices.empty()) 
    {
        std::cout << "Error: No vertices in the mesh!" << std::endl;
        return;
    }

    
    for (int i = 0; i < this->loader.LoadedMeshes.size(); i++) 
    {
        size_t totalSizeInBytes = this->loader.LoadedMeshes[i].Vertices.size() * sizeof(objl::Vertex);
        glBufferData(GL_ARRAY_BUFFER, totalSizeInBytes, (const void *)this->loader.LoadedMeshes[i].Vertices.data(), GL_STATIC_DRAW);
    }
    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)6);
    glEnableVertexAttribArray(1);
    //// normal coord attribute
    //glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)24);
    //glEnableVertexAttribArray(2);
}