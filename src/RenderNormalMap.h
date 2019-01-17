#pragma once
#include "RenderMaterial.h"
class RenderNormalMap : public RenderMaterial
{
public:
	GLuint textureNormal;
	GLuint tangentBuffer;
	GLuint glTan;
	int tangentSize;

	void loadNormal(const char * normal);
	void setProgram(GLuint& program);
	void render(glm::mat4 perspectiveCamera, glm::mat4 modelMatrix, glm::vec3 lightDir, glm::vec3 cameraPos, glm::vec4 clipPlane);
	void render(glm::mat4 perspectiveCamera, glm::mat4 modelMatrix, glm::vec3 lightDir, glm::vec3 cameraPos);
	void render(RenderBundle bundle);
	void loadModelFromFile(const char* fileName);

	//void render(glm::mat4 transformation, glm::vec3 lightDir, glm::vec3 cameraPos);
	//void render(glm::mat4 transformation, glm::vec3 lightDir, glm::vec3 cameraPos, glm::vec4 clipPlane);
	RenderNormalMap();
	~RenderNormalMap();
private:
	void countTangents(obj::Model& model);
};