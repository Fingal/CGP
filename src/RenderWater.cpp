#include "RenderWater.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

const float vertices[]{
	300.5f, -0.0f, 300.5f,
	300.5f, -0.0f, -300.5f,
	-300.5f, -0.0f, 300.5f,
	-300.5f, -0.0f, -300.5f,

};

const unsigned short indexes[]{
	0,
	1,
	2,

	3,
	2,
	1,
};


RenderWater::RenderWater()
{

}
void RenderWater::init(GLuint& reflectTexture, GLuint& refractTrexture, GLuint& refractDepthTrexture) {
	this->reflectTexture = reflectTexture;
	this->refractTrexture = refractTrexture;
	this->refractDepthTrexture = refractDepthTrexture;
	Core::Shader_Loader shaderLoader;
	glGenVertexArrays(1, &VertexArray);
	glBindVertexArray(VertexArray);

	glGenBuffers(1, &VertexIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned short), indexes, GL_STATIC_DRAW);


	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(float), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * 3 * sizeof(float), vertices);
	//glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), vertices, GL_STATIC_DRAW);

	size = 6;
	vertexSize = 4;
	glPos = glGetAttribLocation(program, "vertexPosition");
	glEnableVertexAttribArray(glPos);
	glVertexAttribPointer(glPos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glBindVertexArray(0);
	glBindVertexArray(VertexArray);
	program = shaderLoader.CreateProgram("shaders/water.vert", "shaders/water.frag");
	glPos = glGetAttribLocation(program, "vertexPosition");
	glEnableVertexAttribArray(glPos);
	glVertexAttribPointer(glPos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glBindVertexArray(0);
	dUdVTexture = Core::LoadTexture("E:\\opengl\\CGP2-1 - Copy\\textures\\waterDUDV.png");
	normalTexture = Core::LoadTexture("E:\\opengl\\CGP2-1 - Copy\\textures\\matchingNormalMap.png");


}

void RenderWater::render(glm::mat4 perspectiveMatrix, glm::mat4  cameraMatrix, glm::mat4 lightMVP, glm::vec3 lightPos, glm::vec3 cameraPos) {
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	glm::mat4 transformation = perspectiveMatrix * cameraMatrix;
	glUseProgram(program);
	glBindVertexArray(VertexArray);
	Core::SetActiveTexture(reflectTexture, "reflectSampler", program, 0);
	Core::SetActiveTexture(refractTrexture, "refractSampler", program, 1);
	Core::SetActiveTexture(dUdVTexture, "dudvSampler", program, 2);
	Core::SetActiveTexture(normalTexture, "normalSampler", program, 3);
	Core::SetActiveTexture(refractDepthTrexture, "depthSampler", program, 4);
	Core::SetActiveTexture(depthTexture, "shadowSampler", program, 5);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "lightMVP"), 1, GL_FALSE, (float*)&lightMVP);
	glUniform3fv(glGetUniformLocation(program, "cameraPos"), 1, (float*)&cameraPos);
	glUniform3fv(glGetUniformLocation(program, "lightPos"), 1, (float*)&lightPos);
	glUniform1f(glGetUniformLocation(program, "time"), time);
	glDrawElements(
		GL_TRIANGLES,      // mode
		size,    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);
	glBindVertexArray(0);
	glUseProgram(0);

}

RenderWater::~RenderWater()
{
}
