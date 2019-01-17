#version 430 core

layout(location = 0) in vec3 vertexPosition;

uniform mat4 lightMVPMatrix;
void main()
{
	gl_Position = lightMVPMatrix * vec4(vertexPosition,1);
}
