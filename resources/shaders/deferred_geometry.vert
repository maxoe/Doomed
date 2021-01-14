#version 400 core

layout(location = 0) in vec3 POSITION; // position of the vertex in object space
layout(location = 1) in vec3 NORMAL;   // normal of the vertex in object space
layout (location = 2) in vec2 TEXCOORD;

uniform mat4 VP;              // the view-projection matrix
uniform mat4 modelMatrix;     // the world transformation of the model
uniform mat3 normalMatrix;    // the world transformation for normals

out vec3 worldPosition;              // position of the vertex in world space
out vec3 worldNormalInterpolated;   // normal of the vertex in world space
out vec2 texCoord;

// for portals
out float gl_ClipDistance[1];
uniform vec4 nearClipPortalPlane; // additional near clip plane, basically the portal

void main() {
	gl_Position = VP * modelMatrix * vec4(POSITION, 1);
	worldNormalInterpolated = normalMatrix * NORMAL;
	worldPosition = (modelMatrix * vec4(POSITION, 1)).xyz;
	texCoord = TEXCOORD;

	gl_ClipDistance[0] = dot(vec4(worldPosition, 1), nearClipPortalPlane);
}