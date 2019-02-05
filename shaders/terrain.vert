#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

uniform mat4 modelViewProjectionMatrix;
uniform vec4 clip_plane;
uniform mat4 lightMVP;

out vec3 pos;
out vec3 normal;
out vec4 lightSpacePos;

float gl_ClipDistance[1];
void main()
{
	gl_Position = modelViewProjectionMatrix * vec4(vertexPosition, 1.0);
	lightSpacePos = lightMVP* vec4(vertexPosition, 1.0);
	pos = vertexPosition;
	normal = vertexNormal;
	gl_ClipDistance[0]=dot(clip_plane,vec4(vertexPosition, 1.0)	);
}
