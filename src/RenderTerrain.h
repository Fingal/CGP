#pragma once
#include "RenderTarget.h"
#include "../lib/open-simplex-noise.h"
#include <iostream>
#include <cmath>
#include "glm.hpp"
#include <vector>
class RenderTerrain :
	public RenderTarget
{
public:
	osn_context *ctx1;
	osn_context *ctx2;
	osn_context *ctx3;
	osn_context *ctx4;

	GLuint glVal1;
	GLuint glVal2;

	GLuint lookupTexture;

	float scale;
	float start;
	float end;

	RenderTerrain();

	void buildPolygons(glm::vec3 p, std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, float scale, float * vertices);



	float get_density(glm::vec3 pos);

	glm::vec3 getNormal(glm::vec3 pos);

	void init_vertex_to_edge();

	void sampleTerrain(glm::vec3 pos, float * values, float scale);

	void setValue(glm::vec3 pos, float scale, std::vector<glm::vec4>& val1, std::vector<glm::vec4>& val2, std::vector<glm::vec3>& points);


	void initCPU();



	void init();

	void initGPU();

	void setProgram(GLuint & program);

	void render(glm::mat4 perspectiveCameraMatrix, glm::vec4 clipPlane, glm::vec3 cameraPos);

	void render(RenderBundle bundle);

	void renderGeometry(RenderBundle bundle);

	void setShadowProgram(GLuint & sProgram, GLuint & depthTexture);

	void renderShadow(glm::mat4 lightMVP);

	void render(glm::mat4 perspectiveCameraMatrix, glm::vec3 cameraPos);

	~RenderTerrain();
};

