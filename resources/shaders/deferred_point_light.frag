#version 330 core	

uniform sampler2D textureDiffuse0;

in vec3 worldPosition;            // the (interpolated) world space position corresponding to the fragment
in vec3 worldNormalInterpolated; // the (interpolated) world space normal
in vec2 texCoord;


out vec4 fragColor;

void main()
{
	fragColor = vec4(1.0);
}
