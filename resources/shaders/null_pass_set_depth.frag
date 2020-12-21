#version 400 core

uniform float depth;

void main()
{
	gl_FragDepth = depth;
}
