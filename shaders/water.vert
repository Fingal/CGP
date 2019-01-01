#version 430 core

layout(location = 0) in vec3 vertexPosition;

uniform mat4 modelViewProjectionMatrix;
uniform vec3 cameraPos;
uniform vec3 lightPos;

out vec2 texDirection;
out vec4 clipSpace;
out vec3 vertexPos;
void main()
{
	clipSpace = modelViewProjectionMatrix * vec4(vertexPosition, 1.0);
	gl_Position = modelViewProjectionMatrix * vec4(vertexPosition, 1.0);
	texDirection = vertexPosition.xz/8;
	vertexPos = vertexPosition;
}
