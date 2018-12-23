#pragma once
#include "RenderTarget.h"
class RenderMaterial : public RenderTarget
{
public:
	GLuint textureRoughness;
	GLuint textureMetallic;
	GLuint textureAlbedo;

	void loadMaterial(const char * roughness, const char * metallic, const char * albedo);
	void render(glm::mat4 transformation, glm::vec3 lightDir, glm::vec3 cameraPos);
	RenderMaterial();
	~RenderMaterial();
};