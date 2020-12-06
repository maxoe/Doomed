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
};

#define MAX_POINT_LIGHTS 10
uniform Light pointLights[MAX_POINT_LIGHTS];
uniform int pointLightCount;

uniform vec3 directionalLightDir;
uniform vec3 directionalLightIntensity;
// set to true when directional light is set the first time
uniform bool hasDirectionalLight = false;

uniform vec3 ambient;


uniform vec3 camWorldPos; // the camera position in world space

uniform sampler2D textureDiffuse0; // NUMBER OF TEXTURES CURRENTLY SUPPORTED IS 1 EACH
uniform sampler2D textureSpecular0;

in vec3 worldPosition;            // the (interpolated) world space position corresponding to the fragment
in vec3 worldNormalInterpolated; // the (interpolated) world space normal
in vec2 texCoord;

out vec4 fragColor; // the resulting color value (will be written into the framebuffer)

// lightDir must be not normalized
vec3 getPointLightContribution(int index)
{
	vec3 lightIntensity = pointLights[index].intensity;
	vec3 lightDir = pointLights[index].pos - worldPosition;
	vec3 normalizedLightDir = normalize(lightDir);
	vec3 kDTex = texture(textureDiffuse0, texCoord).rgb;
	vec3 N = normalize(worldNormalInterpolated);
	vec3 camDir = camWorldPos - worldPosition;
	vec3 reflectedDir = normalize(reflect(camDir, N));
	
	vec3 diffuse = kDTex * max(0, dot(normalizedLightDir, N));
	vec3 specular = kS * pow(max(0, dot(reflectedDir, normalizedLightDir)), n);
	
	float dist = length(lightDir);
	vec3 intensity = lightIntensity / (pointLights[index].constAtt + pointLights[index].linAtt * dist + 
    		    pointLights[index].quadAtt * (dist * dist));    

	return vec3((diffuse + specular ) * intensity);
}

vec3 getDirectionalLightContribution()
{
	vec3 lightDir = -directionalLightDir;
	vec3 normalizedLightDir = normalize(lightDir);
	vec3 kDTex = texture(textureDiffuse0, texCoord).rgb;
	vec3 N = normalize(worldNormalInterpolated);
	vec3 camDir = camWorldPos - worldPosition;
	vec3 reflectedDir = normalize(reflect(camDir, N));
	
	vec3 diffuse = kDTex * max(0, dot(normalizedLightDir, N));
	vec3 specular = kS * pow(max(0, dot(reflectedDir, normalizedLightDir)), n);
	
//  no attenuation
//	vec3 intensity = directionalLightIntensity / pow(length(lightDir), 2);

	return vec3((diffuse + specular ) * directionalLightIntensity);
}

void main()
{
	vec3 color = vec3(0.0f);
	
	if (hasDirectionalLight) {
		color = getDirectionalLightContribution();
	}

	for (int i = 0; i < pointLightCount; ++i) {
		color += getPointLightContribution(i);
	}

	fragColor = vec4(color + ambient, 1.0);
}
