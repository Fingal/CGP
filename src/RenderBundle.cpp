#pragma once
#include "glm.hpp"


struct RenderBundle
{
	glm::mat4 perspectiveCameraMatrix; 
	glm::mat4 lightMVP; 
	glm::vec3 lightPos; 
	glm::vec3 cameraPos;  
	glm::mat4 modelMatrix;
	glm::vec4 clipPlane;
};