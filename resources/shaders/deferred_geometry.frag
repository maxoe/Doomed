#version 400 core	

uniform sampler2D textureDiffuse0;
uniform sampler2D textureSpecular0;

// 0 or 1
uniform int diffuseNr;
uniform int specularNr;

in vec3 worldPosition;            // the (interpolated) world space position corresponding to the fragment
in vec3 worldNormalInterpolated; // the (interpolated) world space normal
in vec2 texCoord;


layout (location = 0) out vec3 worldPosOut;
layout (location = 1) out vec3 diffuseOut;
layout (location = 2) out vec3 specularOut;
layout (location = 3) out vec3 normalOut;

void main()
{
	worldPosOut = worldPosition;
	// following works because max 1 each supported
	diffuseOut = diffuseNr * texture(textureDiffuse0, texCoord).rgb;
	specularOut = specularNr * texture(textureSpecular0, texCoord).rgb;
	normalOut = normalize(worldNormalInterpolated);
}
