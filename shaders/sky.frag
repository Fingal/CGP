#version 430 core

uniform samplerCube cubemapTexture; 
in vec3 texDirection;

void main()
{
	vec3 color = texture(cubemapTexture, texDirection).rgb;
	gl_FragColor = vec4(color, 1.0);
}
