#include "ObjectLoader.h"
#include <vector>

//-------------------------------------------------------------------------------------------------------------

ObjectLoader::ObjectLoader(const char* objectPath, bool normalMapping) 
{
    this->loader.LoadFile(objectPath);
    this->VAO = 0;
    this->VBO = 0;
    this->EBO = 0;
    this->normalMapping = normalMapping;
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
    buffers->vertexCount = mesh.Vertices.size();

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal coord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------

void ObjectLoader::calculateTangents(std::vector<float>* newVertexData,
    objl::Mesh* mesh)
{
    for (int i = 0; i < mesh->Indices.size(); i += 3) 
    {
        int i0 = mesh->Indices[i];
        int i1 = mesh->Indices[i + 1];
        int i2 = mesh->Indices[i + 2];

        std::vector<objl::Vertex> vertices = {
            mesh->Vertices[i0],
            mesh->Vertices[i1],
            mesh->Vertices[i2]
        };

        objl::Vertex vertex0 = vertices[0];
        objl::Vertex vertex1 = vertices[1];
        objl::Vertex vertex2 = vertices[2];

        glm::vec3 position0(vertex0.Position.X,
            vertex0.Position.Y,
            vertex0.Position.Z);

        glm::vec3 position1(vertex1.Position.X,
            vertex1.Position.Y,
            vertex1.Position.Z);

        glm::vec3 position2(vertex2.Position.X,
            vertex2.Position.Y,
            vertex2.Position.Z);

        glm::vec3 edge0(position1 - position0);
        glm::vec3 edge1(position2 - position0);

        glm::mat3x2 edgeMatrix(edge0.x, edge1.x, edge0.y, edge1.y, edge0.z, edge1.z);

        float deltaU1 = vertex1.TextureCoordinate.X - vertex0.TextureCoordinate.X;
        float deltaV1 = vertex1.TextureCoordinate.Y - vertex0.TextureCoordinate.Y;
        float deltaU2 = vertex2.TextureCoordinate.X - vertex0.TextureCoordinate.X;
        float deltaV2 = vertex2.TextureCoordinate.Y - vertex0.TextureCoordinate.Y;

        glm::mat2 deltaUV(deltaV2, -1 * deltaU2, -1 * deltaV1, deltaU1);

        float detUV = glm::determinant(deltaUV);

        glm::mat3x2 tangentMatrix = (1.0f / detUV) * deltaUV * edgeMatrix;

        for (int i = 0; i < vertices.size(); i++) 
        {
            // Positions
            newVertexData->push_back(vertices[i].Position.X);
            newVertexData->push_back(vertices[i].Position.Y);
            newVertexData->push_back(vertices[i].Position.Z);

            // Normals
            newVertexData->push_back(vertices[i].Normal.X);
            newVertexData->push_back(vertices[i].Normal.Y);
            newVertexData->push_back(vertices[i].Normal.Z);

            // Texture Coords
            newVertexData->push_back(vertices[i].TextureCoordinate.X);
            newVertexData->push_back(vertices[i].TextureCoordinate.Y);

            // Tangents
            newVertexData->push_back(tangentMatrix[0][0]);
            newVertexData->push_back(tangentMatrix[1][0]);
            newVertexData->push_back(tangentMatrix[2][0]);

            // Bitangents
            newVertexData->push_back(tangentMatrix[0][1]);
            newVertexData->push_back(tangentMatrix[1][1]);
            newVertexData->push_back(tangentMatrix[2][1]);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------

void ObjectLoader::vertexArrayObjectNormalMapping(objl::Mesh mesh, MeshBuffers* buffers)
{
    if (mesh.Vertices.empty())
    {
        std::cout << "Error: No vertices in the mesh!" << std::endl;
        return;
    }

    std::vector<float> vertexData;

    calculateTangents(&vertexData, &mesh);

    // Vertex Buffer Object and Vertex Array Object
    glGenVertexArrays(1, &buffers->VAO);
    glGenBuffers(1, &buffers->VBO);

    glBindVertexArray(buffers->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO);

    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    buffers->indexCount = mesh.Indices.size();
    buffers->vertexCount = vertexData.size() / 14;

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

void ObjectLoader::loadAllMeshes() 
{
    for (int i = 0; i < this->loader.LoadedMeshes.size(); i++) 
    {
        MeshBuffers buffer;
        if (this->normalMapping) 
        {
            vertexArrayObjectNormalMapping(this->loader.LoadedMeshes[i], &buffer);
        }
        else 
        {
            createElementBufferObject(this->loader.LoadedMeshes[i], &buffer);
        }

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