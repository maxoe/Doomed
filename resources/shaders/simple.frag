#version 330 core

uniform vec3 lightWorldPos;
uniform vec3 lightIntensity;

uniform vec3 kS; // specular material parameter
uniform vec3 kD; // diffuse material parameter
uniform float n;  // phong exponent of the material

uniform vec3 camWorldPos; // the camera position in world space

in vec3 worldPosition;            // the (interpolated) world space position corresponding to the fragment
in vec3 worldNormalInterpolated; // the (interpolated) world space normal

out vec4 fragColor; // the resulting color value (will be written into the framebuffer)

void
main()
{
	vec3 lightDir = normalize(lightWorldPos - worldPosition);
	vec3 N = normalize(worldNormalInterpolated);
	vec3 camDir = camWorldPos - worldPosition;
	vec3 reflectedDir = normalize(reflect(camDir, N));
	vec3 diffuse = kD * max(0, dot(lightDir, N));
	vec3 specular = kS * pow(max(0, dot(reflectedDir, lightDir)), n);
	vec3 intensity = lightIntensity / pow(length(worldPosition - lightWorldPos), 2);
	
	fragColor = vec4((diffuse + specular) * intensity, 1.0);
}
