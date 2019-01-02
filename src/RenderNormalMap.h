#pragma once
#include "RenderMaterial.h"
class RenderNormalMap : public RenderMaterial
{
public:
	GLuint textureNormal;

	void loadNormal(const char * normal);
	void render(glm::mat4 perspectiveCamera, glm::mat4 modelMatrix, glm::vec3 lightDir, glm::vec3 cameraPos, glm::vec4 clipPlane);
	void render(glm::mat4 perspectiveCamera, glm::mat4 modelMatrix, glm::vec3 lightDir, glm::vec3 cameraPos);
	void loadModelFromFile(const char* fileName);
	//void render(glm::mat4 transformation, glm::vec3 lightDir, glm::vec3 cameraPos);
	//void render(glm::mat4 transformation, glm::vec3 lightDir, glm::vec3 cameraPos, glm::vec4 clipPlane);
	RenderNormalMap();
	~RenderNormalMap();
};