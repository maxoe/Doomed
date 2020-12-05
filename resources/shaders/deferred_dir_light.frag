#version 400 core

float n = 10;
vec3 kS = vec3(1.0f, 1.0f, 1.0f);

uniform vec3 directionalLightDir;
uniform vec3 directionalLightIntensity;

uniform vec3 camWorldPos; // the camera position in world space

uniform sampler2D positionMap;
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

uniform vec2 screenSize;

out vec4 fragColor; // the resulting color value (will be written into the framebuffer)

// lightDir must be not normalized
vec3 getDirLightContribution(vec3 worldPos, vec3 worldNormal, vec3 color)
{
	vec3 lightDir = -directionalLightDir;
	vec3 normalizedLightDir = normalize(lightDir);
	vec3 kDTex = color;
	vec3 N = normalize(worldNormal);
	vec3 camDir = camWorldPos - worldPos;
	vec3 reflectedDir = normalize(reflect(camDir, N));
	
	vec3 diffuse = kDTex * max(0, dot(normalizedLightDir, N));
	vec3 specular = kS * pow(max(0, dot(reflectedDir, normalizedLightDir)), n);
	
//  no attenuation
//	vec3 intensity = directionalLightIntensity / pow(length(lightDir), 2);

	return vec3((diffuse + specular ) * directionalLightIntensity);
}

vec2 calcTexCoord()
{
    return gl_FragCoord.xy / screenSize;
}


void main()
{
	vec2 gBufferTexCoord = calcTexCoord();
	vec3 worldPos = texture(positionMap, gBufferTexCoord).xyz;
	vec3 diff = texture(diffuseMap, gBufferTexCoord).xyz;
	vec3 worldNormal = texture(normalMap, gBufferTexCoord).xyz;
	
	fragColor = vec4(getDirLightContribution(worldPos, worldNormal, diff), 1.0);
}