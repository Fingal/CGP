#pragma once
#include "RenderTarget.h"
class RenderSkyBox :
	public RenderTarget
{
public:
	GLuint cubemapTexture;

	RenderSkyBox();

	void init();

	void setProgram(GLuint & program);

	void render(glm::mat4 perspectiveCameraMatrix, glm::vec4 clipPlane, glm::vec3 cameraPos);

	void render(glm::mat4 perspectiveCameraMatrix, glm::vec3 cameraPos);


	~RenderSkyBox();
};

