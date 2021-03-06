#version 430 core

layout(location = 0) in vec3 vertexPosition;

uniform mat4 modelViewProjectionMatrix;
uniform vec4 clip_plane;
out vec3 texDirection;
out vec3 pos;
float gl_ClipDistance[1];
void main()
{
	gl_Position = modelViewProjectionMatrix * vec4(vertexPosition, 1.0);
	pos = vertexPosition;
	texDirection = vertexPosition;
	gl_ClipDistance[0]=1;
}
