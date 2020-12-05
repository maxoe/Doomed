#version 400 core

layout(location = 0) in vec3 POSITION; // position of the vertex in object space

uniform mat4 VP;              // the view-projection matrix
uniform mat4 modelMatrix;     // the world transformation of the model

void main() {
	gl_Position = VP * modelMatrix * vec4(POSITION, 1);
}