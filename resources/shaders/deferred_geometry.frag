#version 330 core	

uniform sampler2D textureDiffuse0;

in vec3 worldPosition;            // the (interpolated) world space position corresponding to the fragment
in vec3 worldNormalInterpolated; // the (interpolated) world space normal
in vec2 texCoord;


layout (location = 0) out vec4 worldPosOut;
layout (location = 1) out vec4 diffuseOut;
layout (location = 2) out vec4 normalOut;
layout (location = 3) out vec4 texCoordOut;

void main()
{
	worldPosOut = vec4(worldPosition, 1.0);
	diffuseOut = vec4(texture(textureDiffuse0, texCoord).rgb, 1.0);
	normalOut = vec4(normalize(worldNormalInterpolated), 1.0);
	texCoordOut = vec4(1.0);
}
