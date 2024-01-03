#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    // Calculate the ModelViewProjection Matrix
    mat4 MVP = projectionMatrix * viewMatrix * modelMatrix; 

    // Transform the vertex into clipping space
    gl_Position =  MVP * vec4(vertexPosition_modelspace, 1);
}