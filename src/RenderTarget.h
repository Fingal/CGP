#pragma once
#include "misc_tools.h"
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"

#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include "RenderBundle.cpp"
#include <vector>

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget(); 
	
	void drawElements();

	void loadModelFromFile(const char * fileName);

	void setShadowProgram(GLuint& sProgram, GLuint& depthTexture);

	void setProgram(GLuint & program);

	void render(glm::mat4 transformation, glm::mat4 model, glm::vec3 lightDir, glm::vec3 cameraPos, glm::vec4 clipPlane);

	void render(glm::mat4 transformation, glm::mat4 model, glm::vec3 lightDir, glm::vec3 cameraPos);

	void render(glm::mat4 transformation, glm::vec3 lightDir, glm::vec3 cameraPos, glm::vec4 clipPlane);

	void renderShadow(glm::mat4 lightMVP);

	GLuint VertexIndexBuffer;
	GLuint VertexBuffer;
	GLuint VertexArray;

	GLuint program;
	GLuint shadowProgram;

	GLuint glPos;
	GLuint glNor;
	GLuint glUV;

	GLuint depthTexture;

	int vertexSize;
	int normalSize;
	int size;
protected:
	void _loadFromModel(obj::Model& model);
};

