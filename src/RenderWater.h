#pragma once
#include "RenderTarget.h"
class RenderWater :
	public RenderTarget
{
public:
	GLuint reflectTexture;
	GLuint refractTrexture;
	GLuint refractDepthTrexture;

	GLuint dUdVTexture;
	GLuint normalTexture;

	RenderWater();

	void init(GLuint& reflectTexture, GLuint& refractTrexture, GLuint& refractDepthTrexture);

	void render(glm::mat4 perspectiveMatrix, glm::mat4  cameraMatrix, glm::vec3 lightDir, glm::vec3 cameraPos);


	~RenderWater();
};

