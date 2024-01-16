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


    size_t totalSizeInBytes = 0;
    
    for (int i = 0; i < this->loader.LoadedMeshes.size(); i++) 
    {
        totalSizeInBytes += this->loader.LoadedMeshes[i].Vertices.size() * sizeof(objl::Vertex);
    }

    objl::Vertex* screenBuffer = static_cast<objl::Vertex*>(VirtualAlloc(0, totalSizeInBytes, MEM_COMMIT, PAGE_READWRITE));

    size_t offset = 0;
    for (int i = 0; i < this->loader.LoadedMeshes.size(); i++) 
    {
        size_t size = this->loader.LoadedMeshes[i].Vertices.size() * sizeof(objl::Vertex);
        memcpy(screenBuffer + offset, this->loader.LoadedMeshes[i].Vertices.data(), size);
        offset += this->loader.LoadedMeshes[i].Vertices.size();
    }

    glBufferData(GL_ARRAY_BUFFER, totalSizeInBytes, (const void *)screenBuffer, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    //// normal coord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)3);
    glEnableVertexAttribArray(2);

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)6);
    glEnableVertexAttribArray(1);    
}