#version 400 core

/* NOT AVAILABLE ANYMORE AS CONSTANTS --> READ FROM TEXTURE OR WAIT UNTIL MATERIAL IS READ  */
//uniform vec3 kS; // specular material parameter  NOT AVAILABLE ANYMORE
//uniform vec3 kD; // diffuse material parameter   NOT AVAILABLE ANYMORE
//uniform float n;  // phong exponent of the material  NOT AVAILABLE ANYMORE
float n = 10;
vec3 kS = vec3(0.0f);

struct Light
{
  vec3 pos;
  vec3 intensity;
  float constAtt;
  float linAtt;
  float quadAtt;
  bool shadows;
};

#define MAX_POINT_LIGHTS 10
uniform Light pointLights[MAX_POINT_LIGHTS];
uniform samplerCube shadowMaps[MAX_POINT_LIGHTS];
uniform int pointLightCount;


uniform vec3 directionalLightDir;
uniform vec3 directionalLightIntensity;
// set to true when directional light is set the first time
uniform bool hasDirectionalLight = false;

uniform vec3 ambient;


uniform vec3 camWorldPos; // the camera position in world space

uniform sampler2D textureDiffuse0; // NUMBER OF TEXTURES CURRENTLY SUPPORTED IS 1 EACH
uniform sampler2D textureSpecular0;

uniform int diffuseNr;
uniform int specularNr;

uniform float far_plane;

in vec3 worldPosition;            // the (interpolated) world space position corresponding to the fragment
in vec3 worldNormalInterpolated; // the (interpolated) world space normal
in vec2 texCoord;
in vec4 lightSpacePos;      

out vec4 fragColor; // the resulting color value (will be written into the framebuffer)

// lightDir must be not normalized
vec3 getPointLightContribution(float shadowFactor, int i)
{
	vec3 lightIntensity = pointLights[i].intensity;
	vec3 lightDir = pointLights[i].pos - worldPosition;
	vec3 normalizedLightDir = normalize(lightDir);
	vec3 N = normalize(worldNormalInterpolated);
	vec3 camDir = camWorldPos - worldPosition;
	vec3 reflectedDir = normalize(reflect(camDir, N));

	vec3 diffuse = vec3(0.0);
	if (diffuseNr == 1) {
		vec3 kDTex = texture(textureDiffuse0, texCoord).rgb;
		diffuse = kDTex * max(0, dot(normalizedLightDir, N));
	}

	vec3 specular = vec3(0.0);
	if (specularNr == 1) {
		vec3 kSTex = texture(textureSpecular0, texCoord).rgb;
		specular = kSTex * pow(max(0, dot(reflectedDir, normalizedLightDir)), n);
	}
	
	float dist = length(lightDir);
	vec3 intensity = lightIntensity / (pointLights[i].constAtt + pointLights[i].linAtt * dist + 
    		    pointLights[i].quadAtt * (dist * dist));

	return (diffuse + specular ) * intensity * shadowFactor;
}

vec3 getDirectionalLightContribution()
{
	vec3 lightDir = -directionalLightDir;
	vec3 normalizedLightDir = normalize(lightDir);
	vec3 N = normalize(worldNormalInterpolated);
	vec3 camDir = camWorldPos - worldPosition;
	vec3 reflectedDir = normalize(reflect(camDir, N));
	
	vec3 diffuse = vec3(0.0);
	if (diffuseNr == 1) {
		vec3 kDTex = texture(textureDiffuse0, texCoord).rgb;
		diffuse = kDTex * max(0, dot(normalizedLightDir, N));
	}
	
	vec3 specular = vec3(0.0);
	if (specularNr == 1) {
		vec3 kSTex = texture(textureSpecular0, texCoord).rgb;
		specular = kSTex * pow(max(0, dot(reflectedDir, normalizedLightDir)), n);
	}
//  no attenuation
//	vec3 intensity = directionalLightIntensity / pow(length(lightDir), 2);

	return vec3((diffuse + specular ) * directionalLightIntensity);
}

// check depth in cube shadow map
float calcShadow(int index)
{
    vec3 lightDir = worldPosition - pointLights[index].pos; 
    float closestDepth = texture(shadowMaps[index], lightDir).r;
	// closestDepth is normalized to [0, 1]
	closestDepth *= far_plane;

	//for debugging uncomment this and comment the original fragColor out
	//fragColor = vec4(vec3(closestDepth / far_plane), 1.0); 

	float fragDepth = length(lightDir);

	float bias = 0.05; 
	return fragDepth - bias > closestDepth ? 0.0 : 1.0; 
}

// percentage-closer filtering
// see https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
); 

float calcShadowPCF(int index) {
	vec3 lightDir = worldPosition - pointLights[index].pos;
	float fragDepth = length(lightDir);
	float shadow  = 0.0;
	float bias    = 0.15; 
	float viewDistance = length(camWorldPos - worldPosition);
	float diskRadius = 0.05;

	int samples  = 20;

	for (int i = 0; i < samples; ++i)
	{
		float closestDepth = texture(shadowMaps[index], lightDir + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= far_plane;   // undo mapping [0;1]
		if(fragDepth - bias > closestDepth)
			shadow += 1.0;
	}

	return 1.0 - (shadow / float(samples));
}


void main()
{
	vec3 color = vec3(0.0f);
	
	if (hasDirectionalLight) {
		color = getDirectionalLightContribution();
	}
	
	for (int i = 0; i < pointLightCount; ++i) {
		float shadowFactor = pointLights[i].shadows ? calcShadowPCF(i) : 1.0f; 
		color += getPointLightContribution(shadowFactor, i);
	}

	vec3 ambientColor = ambient * texture(textureDiffuse0, texCoord).rgb;

	fragColor = vec4(color + ambientColor, 1.0);
}
