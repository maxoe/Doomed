#version 400 core

layout(location = 0) in vec3 POSITION; // position of the vertex in object space

void main() {
	gl_Position = vec4(POSITION, 1);
}