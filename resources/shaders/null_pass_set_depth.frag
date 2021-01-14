#version 400 core

uniform sampler2D portalTexture;
uniform vec2 portalResize;

in vec2 texCoord;

out vec4 fragColor;

void main()
{
	fragColor = vec4(texture(portalTexture, 1.0 - texCoord).xyz, 1.0);
}
