#version 330 core

uniform vec3 lightWorldPos;
uniform vec3 lightIntensity;
uniform vec3 ambient;

/* NOT AVAILABLE ANYMORE AS CONSTANTS --> READ FROM TEXTURE OR WAIT UNTIL MATERIAL IS READ  */
//uniform vec3 kS; // specular material parameter  NOT AVAILABLE ANYMORE
//uniform vec3 kD; // diffuse material parameter   NOT AVAILABLE ANYMORE
//uniform float n;  // phong exponent of the material  NOT AVAILABLE ANYMORE
float n = 10;
vec3 kS = vec3(1.0f, 1.0f, 1.0f);





uniform vec3 camWorldPos; // the camera position in world space

uniform sampler2D textureDiffuse0; // NUMBER OF TEXTURES CURRENTLY SUPPORTED IS 1 EACH
uniform sampler2D textureSpecular0;

in vec3 worldPosition;            // the (interpolated) world space position corresponding to the fragment
in vec3 worldNormalInterpolated; // the (interpolated) world space normal
in vec2 texCoord;

out vec4 fragColor; // the resulting color value (will be written into the framebuffer)

void
main()
{
	vec3 kDTex = texture(textureDiffuse0, texCoord).rgb;

	vec3 lightDir = normalize(lightWorldPos - worldPosition);
	vec3 N = normalize(worldNormalInterpolated);
	vec3 camDir = camWorldPos - worldPosition;
	vec3 reflectedDir = normalize(reflect(camDir, N));
	vec3 diffuse = kDTex * max(0, dot(lightDir, N));
	vec3 specular = kS * pow(max(0, dot(reflectedDir, lightDir)), n);
	vec3 intensity = lightIntensity / pow(length(worldPosition - lightWorldPos), 2);
	
	fragColor = vec4((diffuse + specular ) * intensity + ambient, 1.0);
}
