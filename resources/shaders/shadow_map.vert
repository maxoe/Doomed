#version 400 core

layout(location = 0) in vec3 POSITION; // position of the vertex in object space
layout (location = 2) in vec2 TEXCOORD;

uniform mat4 VP;              // the view-projection matrix
uniform mat4 modelMatrix;     // the world transformation of the model

out vec2 texCoord;

void main() {
	gl_Position = VP * modelMatrix * vec4(POSITION, 1);
	texCoord = TEXCOORD;
}