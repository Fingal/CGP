#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelMatrix;
uniform vec4 clip_plane;
uniform mat4 lightMVP;

uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform vec3 lightDir;

out vec3 pos;
out vec3 interpNormal;
out vec3 interpPos;
out vec2 texCoord;

out vec3 interpLight;
out vec3 interpCamera;
out vec3 interpTangent;
out vec3 bTangent;
out vec4 lightSpacePos;

float gl_ClipDistance[1];

void main()
{
	gl_Position = modelViewProjectionMatrix * modelMatrix * vec4(vertexPosition, 1.0);
	interpNormal = normalize((modelMatrix * vec4(vertexNormal, 0.0)).xyz);
	interpTangent = normalize((modelMatrix * vec4(vertexTangent, 0.0)).xyz);
	vec3 bTangent = normalize(cross(interpNormal,interpTangent));
	interpTangent = normalize(cross(bTangent,interpNormal));
	texCoord = vertexTexCoord;
	pos = (modelMatrix * vec4(vertexPosition, 1.0)).xyz;
	
	interpLight = vec3(dot(interpTangent,lightDir),dot(bTangent,lightDir),dot(interpNormal,lightDir));
	interpPos = vec3(dot(interpTangent,pos),dot(bTangent,pos),dot(interpNormal,pos));
	interpCamera = vec3(dot(interpTangent,cameraPos),dot(bTangent,cameraPos),dot(interpNormal,cameraPos));

	lightSpacePos = lightMVP* modelMatrix*vec4(vertexPosition, 1.0);

	gl_ClipDistance[0]=dot(modelMatrix*vec4(vertexPosition,1.0),clip_plane);
}
