#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexValue1;
layout(location = 2) in vec4 vertexValue2;

uniform mat4 modelViewProjectionMatrix;
uniform vec4 clip_plane;
uniform mat4 lightMVP;

out vec3 oPos;
out vec4 val1;
out vec4 val2;

float gl_ClipDistance[1];
void main()
{
	gl_Position =  vec4(vertexPosition, 1.0);
	oPos = vertexPosition;
	//lightSpacePos = lightMVP* vec4(vertexPosition, 1.0);
	val1 = vertexValue1;
	val2 = vertexValue2;
	//gl_ClipDistance[0]=dot(clip_plane,vec4(vertexPosition, 1.0)	);
}
