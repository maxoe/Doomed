#version 400 core

float n = 10;

struct Light
{
  vec3 pos;
  vec3 intensity;
  float constAtt;
  float linAtt;
  float quadAtt;
  bool shadows;
};

uniform Light pointLights[1];

uniform vec3 camWorldPos; // the camera position in world space

uniform sampler2D positionMap;
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;

uniform samplerCube shadowMaps[1];

uniform float far_plane;

uniform vec2 screenSize;

out vec4 fragColor; // the resulting color value (will be written into the framebuffer)

// lightDir must be not normalized
vec3 getPointLightContribution(vec3 worldPos, vec3 worldNormal, vec3 diff, vec3 spec)
{
	vec3 lightIntensity = pointLights[0].intensity;
	vec3 lightDir = pointLights[0].pos - worldPos;
	vec3 normalizedLightDir = normalize(lightDir);
	vec3 kDTex = diff;
	vec3 kSTex = spec;
	vec3 N = normalize(worldNormal);
	vec3 camDir = camWorldPos - worldPos;
	vec3 reflectedDir = normalize(reflect(camDir, N));
	
	vec3 diffuse = kDTex * max(0, dot(normalizedLightDir, N));
	vec3 specular = kSTex * pow(max(0, dot(reflectedDir, normalizedLightDir)), n);
	
	float dist = length(lightDir);
	vec3 intensity = lightIntensity / (pointLights[0].constAtt + pointLights[0].linAtt * dist + 
    		    pointLights[0].quadAtt * (dist * dist));

	return (diffuse + specular) * intensity;
}

vec2 calcTexCoord()
{
    return gl_FragCoord.xy / screenSize;
}

// check depth in cube shadow map
float calcShadow(vec3 worldPosition)
{
    vec3 lightDir = worldPosition - pointLights[0].pos; 
    float closestDepth = texture(shadowMaps[0], lightDir).r;
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

float calcShadowPCF(vec3 worldPosition) {
	vec3 lightDir = worldPosition - pointLights[0].pos;
	float fragDepth = length(lightDir);
	float shadow  = 0.0;
	float bias    = 0.15; 
	float viewDistance = length(camWorldPos - worldPosition);
	float diskRadius = 0.05;

	int samples  = 20;

	for (int i = 0; i < samples; ++i)
	{
		float closestDepth = texture(shadowMaps[0], lightDir + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= far_plane;   // undo mapping [0;1]
		if(fragDepth - bias > closestDepth)
			shadow += 1.0;
	}

	return 1.0 - (shadow / float(samples));
}

void main()
{
	vec2 gBufferTexCoord = calcTexCoord();
	vec3 worldPos = texture(positionMap, gBufferTexCoord).xyz;
	vec3 diff = texture(diffuseMap, gBufferTexCoord).xyz;
	vec3 spec = texture(specularMap, gBufferTexCoord).xyz;
	vec3 worldNormal = texture(normalMap, gBufferTexCoord).xyz;
	
	float shadowFactor = pointLights[0].shadows ? calcShadowPCF(worldPos) : 1.0f;

	vec3 color = getPointLightContribution(worldPos, worldNormal, diff, spec) * shadowFactor;

	fragColor = vec4(color, 1.0);
}
