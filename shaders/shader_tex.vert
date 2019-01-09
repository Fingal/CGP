#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;

uniform vec3 lightDir;
uniform vec3 cameraPos;

uniform mat4 perspectiveCameraMatrix;
uniform mat4 modelMatrix;

uniform vec4 clip_plane;

uniform mat4 lightMVP;

out vec3 interpNormal;
out vec2 interpTexCoord;
out vec3 interpPosition;
out vec4 lightSpacePos;

float gl_ClipDistance[1];
void main()
{
	interpNormal = normalize((modelMatrix * vec4(vertexNormal, 0.0)).xyz);
	interpTexCoord = vec2(vertexTexCoord.x, 1.0 - vertexTexCoord.y);
	interpPosition = (modelMatrix * vec4(vertexPosition, 1.0)).xyz;
	gl_Position = perspectiveCameraMatrix * modelMatrix * vec4(vertexPosition, 1.0);

	
	
	lightSpacePos = lightMVP* modelMatrix*vec4(vertexPosition, 1.0);

	gl_ClipDistance[0]=dot(modelMatrix*vec4(vertexPosition,1.0),clip_plane);
}
