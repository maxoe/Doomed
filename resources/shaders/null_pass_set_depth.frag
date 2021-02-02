#version 400 core

uniform sampler2D portalTexture;
uniform vec2 portalResize;
uniform vec2 screenSize;

in vec2 texCoord;

out vec4 fragColor;

vec2 calcTexCoord()
{
    return gl_FragCoord.xy / screenSize;
}

void main()
{
	fragColor = vec4(texture(portalTexture, calcTexCoord()).xyz, 1.0);
}
