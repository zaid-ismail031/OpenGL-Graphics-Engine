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
    if (mesh.Vertices.empty()) 
    {
        std::cout << "Error: No vertices in the mesh!" << std::endl;
        return;
    }

    std::vector<unsigned int> indexData;
    std::vector<float> vertexData;
    std::vector<float> tangentData;
    std::vector<float> bitangentData;

    calculateTangents(&tangentData, &bitangentData, &mesh);
        
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
        
        int tangentIndex = 6 * vertex;

        //vertexData.push_back(tangentData[tangentIndex]);
        //vertexData.push_back(tangentData[tangentIndex + 1]);
        //vertexData.push_back(tangentData[tangentIndex + 2]);
        //vertexData.push_back(tangentData[tangentIndex + 3]);
        //vertexData.push_back(tangentData[tangentIndex + 4]);
        //vertexData.push_back(tangentData[tangentIndex + 5]);
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal coord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);

    // tangent coord attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(sizeof(float) * 8));
    glEnableVertexAttribArray(3);

    // bitangent coord attribute
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(sizeof(float) * 11));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------

void ObjectLoader::calculateTangents(std::vector<float>* tangentVector, 
    std::vector<float>* bitangentVector, 
    objl::Mesh* mesh) 
{
    for (int i = 0; i < mesh->Indices.size(); i += 3) 
    {
        int i0 = mesh->Indices[i];
        int i1 = mesh->Indices[i + 1];
        int i2 = mesh->Indices[i + 2];

        glm::vec3 position0(mesh->Vertices[i0].Position.X, 
            mesh->Vertices[i0].Position.Y, 
            mesh->Vertices[i0].Position.Z);

        glm::vec3 position1(mesh->Vertices[i1].Position.X,
            mesh->Vertices[i1].Position.Y,
            mesh->Vertices[i1].Position.Z);

        glm::vec3 position2(mesh->Vertices[i2].Position.X,
            mesh->Vertices[i2].Position.Y,
            mesh->Vertices[i2].Position.Z);

        glm::vec3 edge0(position1 - position0);
        glm::vec3 edge1(position2 - position1);

        glm::mat3x2 edgeMatrix(edge0.r, edge1.r, edge0.g, edge1.g, edge0.b, edge1.b);

        float deltaU1 = mesh->Vertices[i1].TextureCoordinate.X - mesh->Vertices[i0].TextureCoordinate.X;
        float deltaV1 = mesh->Vertices[i1].TextureCoordinate.Y - mesh->Vertices[i0].TextureCoordinate.Y;
        float deltaU2 = mesh->Vertices[i2].TextureCoordinate.X - mesh->Vertices[i2].TextureCoordinate.X;
        float deltaV2 = mesh->Vertices[i2].TextureCoordinate.Y - mesh->Vertices[i2].TextureCoordinate.Y;

        glm::mat2 deltaUV(deltaV2, -1 * deltaU2, -1 * deltaV1, deltaU1);

        float detUV = glm::determinant(deltaUV);

        glm::mat3x2 tangentMatrix = (1.0f / detUV) * deltaUV * edgeMatrix;

        tangentVector->push_back(tangentMatrix[0][0]);
        tangentVector->push_back(tangentMatrix[1][0]);
        tangentVector->push_back(tangentMatrix[2][0]);

        tangentVector->push_back(tangentMatrix[0][1]);
        tangentVector->push_back(tangentMatrix[1][1]);
        tangentVector->push_back(tangentMatrix[2][1]);
    }
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

    for (int i = 0; i < this->loader.LoadedMaterials.size(); i++) 
    {
        materialVector.push_back(this->loader.LoadedMeshes[i].MeshMaterial);
    }
}

//-------------------------------------------------------------------------------------------------------------

objl::Material ObjectLoader::loadMaterial(int index)
{
    return materialVector[index];
}

//-------------------------------------------------------------------------------------------------------------