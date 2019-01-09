#pragma once
#include "RenderTarget.h"
class RenderTexture : public RenderTarget
{
public:
	GLuint texture;

	void loadTexture(const char * texture);
	void render(glm::mat4 perspectiveCamera, glm::mat4 modelMatrix, glm::vec3 lightDir, glm::vec3 cameraPos, glm::vec4 clipPlane);
	void render(glm::mat4 perspectiveCamera, glm::mat4 modelMatrix, glm::vec3 lightDir, glm::vec3 cameraPos);

	void render(RenderBundle bundle);
	//void render(glm::mat4 transformation, glm::vec3 lightDir, glm::vec3 cameraPos);
	//void render(glm::mat4 transformation, glm::vec3 lightDir, glm::vec3 cameraPos, glm::vec4 clipPlane);
	RenderTexture();
	~RenderTexture();
};