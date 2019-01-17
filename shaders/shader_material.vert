#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelMatrix;
uniform vec4 clip_plane;
uniform mat4 lightMVP;

out vec3 interpNormal;
out vec3 interpPos;
out vec2 interpTexCoord;
out vec4 lightSpacePos;

float gl_ClipDistance[1];

void main()
{
	gl_Position = modelViewProjectionMatrix * modelMatrix * vec4(vertexPosition, 1.0);
	interpNormal = normalize((modelMatrix * vec4(vertexNormal, 0.0)).xyz);
	interpTexCoord = vertexTexCoord;
	interpPos = (modelMatrix * vec4(vertexPosition, 1.0)).xyz;

	
	lightSpacePos = lightMVP* modelMatrix*vec4(vertexPosition, 1.0);

	gl_ClipDistance[0]=dot(modelMatrix*vec4(vertexPosition,1.0),clip_plane);
}
