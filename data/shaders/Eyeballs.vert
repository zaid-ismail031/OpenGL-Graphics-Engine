#version 330 core

// Input vertex data.
layout(location = 0) in vec3 aPos;

// Input normal data.
layout(location = 1) in vec3 aNormal;

// Input texture data.
layout(location = 2) in vec2 aTexCoord;

// Input tangent data
layout(location = 3) in vec3 aTanCoord;

// Input bitangent data
layout(location = 4) in vec3 aBitanCoord;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;
out vec3 Tangent;
out vec3 Bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Calculate the ModelViewProjection Matrix
    mat4 MVP = projection * view * model; 

    // Transform the vertex into clipping space
    gl_Position =  MVP * vec4(aPos, 1);

    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = aNormal;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    Tangent = vec3(aTanCoord.x, aTanCoord.y, aTanCoord.z);
    Bitangent = vec3(aBitanCoord.x, aBitanCoord.y, aBitanCoord.z);
}