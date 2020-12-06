#version 400 core	

uniform sampler2D textureDiffuse0;
uniform sampler2D textureSpecular0;

in vec3 worldPosition;            // the (interpolated) world space position corresponding to the fragment
in vec3 worldNormalInterpolated; // the (interpolated) world space normal
in vec2 texCoord;


layout (location = 0) out vec3 worldPosOut;
layout (location = 1) out vec3 diffuseOut;
layout (location = 2) out vec3 normalOut;
//layout (location = 3) out vec3 texCoordOut;

void main()
{
	worldPosOut = worldPosition;
	diffuseOut = texture(textureDiffuse0, texCoord).rgb * texture(textureSpecular0, texCoord).rgb;
	normalOut = normalize(worldNormalInterpolated);
//	texCoordOut = vec3(texCoord, 1.0);
}
