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

	void render(glm::mat4 perspectiveCameraMatrix, glm::vec4 clipPlane);

	void render(glm::mat4 perspectiveCameraMatrix);


	~RenderSkyBox();
};

