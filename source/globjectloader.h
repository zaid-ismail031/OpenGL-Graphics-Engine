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
};

struct MaterialProperties
{
	// Material Name
	std::string name;
	// Ambient Color
	glm::vec3 Ka;
	// Diffuse Color
	glm::vec3 Kd;
	// Specular Color
	glm::vec3 Ks;
	// Specular Exponent
	float Ns;
	// Optical Density
	float Ni;
	// Dissolve
	float d;
	// Illumination
	int illum;
	// Ambient Texture Map
	std::string map_Ka;
	// Diffuse Texture Map
	std::string map_Kd;
	// Specular Texture Map
	std::string map_Ks;
	// Specular Hightlight Map
	std::string map_Ns;
	// Alpha Texture Map
	std::string map_d;
	// Bump Map
	std::string map_bump;
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
        void loadMaterial(int index, std::vector<MaterialProperties> *materialProps);
};

#endif