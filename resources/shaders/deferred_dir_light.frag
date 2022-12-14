#version 400 core

float n = 10;

uniform vec3 directionalLightDir;
uniform vec3 directionalLightIntensity;

uniform vec3 camWorldPos; // the camera position in world space

uniform sampler2D positionMap;
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;

uniform vec2 screenSize;

uniform vec3 ambient;

out vec4 fragColor; // the resulting color value (will be written into the framebuffer)

// lightDir must be not normalized
vec3 getDirLightContribution(vec3 worldPos, vec3 worldNormal, vec3 diff, vec3 spec)
{
	vec3 lightDir = -directionalLightDir;
	vec3 normalizedLightDir = normalize(lightDir);
	vec3 kDTex = diff;
	vec3 kSTex = spec;
	vec3 N = normalize(worldNormal);
	vec3 camDir = camWorldPos - worldPos;
	vec3 reflectedDir = normalize(reflect(camDir, N));
	
	vec3 diffuse = kDTex * max(0, dot(normalizedLightDir, N));
	vec3 specular = kSTex * pow(max(0, dot(reflectedDir, normalizedLightDir)), n);
	
//  no attenuation
//	vec3 intensity = directionalLightIntensity / pow(length(lightDir), 2);

	return (diffuse + specular) * directionalLightIntensity;
}

vec2 calcTexCoord()
{
    return gl_FragCoord.xy / screenSize;
}


void main()
{
	vec2 gBufferTexCoord = calcTexCoord();
	vec3 diff = texture(diffuseMap, gBufferTexCoord).xyz;
	vec3 worldPos = texture(positionMap, gBufferTexCoord).xyz;
	vec3 spec = texture(specularMap, gBufferTexCoord).xyz;
	vec3 worldNormal = texture(normalMap, gBufferTexCoord).xyz;
	vec3 ambientColor = ambient * diff;

	if (length(directionalLightDir) != 0)
	{
		// has directional light
		fragColor = vec4(getDirLightContribution(worldPos, worldNormal, diff, spec) + ambientColor, 1.0);
	}
	else
	{
		fragColor = vec4(ambientColor, 1.0);
	}	
}
