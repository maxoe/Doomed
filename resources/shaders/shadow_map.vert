#version 400 core

layout(location = 0) in vec3 POSITION; // position of the vertex in object space

uniform mat4 modelMatrix; 

void main() {
	gl_Position = modelMatrix * vec4(POSITION, 1);
}