#version 400 core

in vec4 fragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    float dist = length(fragPos.xyz - lightPos);
    dist = dist / far_plane;
    
    gl_FragDepth = dist;
}  