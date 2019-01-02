#include "RenderNormalMap.h"



RenderNormalMap::RenderNormalMap()
{
}

void RenderNormalMap::loadNormal(const char* normal) {
	textureNormal	= Core::LoadTexture(normal);
}
void RenderNormalMap::loadModelFromFile(const char* fileName) {
	obj::Model model = obj::loadModelFromFile(fileName);
	_loadFromModel(model);
	//ogldev.atspace.co.uk/www/tutorial26/tutorial26.html TODO implement adding tangents and change render metod
}
void RenderNormalMap::render(glm::mat4 perspectiveCamera,glm::mat4 modelMatrix, glm::vec3 lightPos, glm::vec3 cameraPos,glm::vec4 clipPlane) {
	glUseProgram(program);
	glBindVertexArray(VertexArray);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexIndexBuffer);
	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform4fv(glGetUniformLocation(program, "clip_plane"), 1, (float*)&clipPlane);
	Core::SetActiveTexture(textureMetallic, "Tmetallic", program, 0);
	Core::SetActiveTexture(textureRoughness, "Troughness", program, 1);
	Core::SetActiveTexture(textureAlbedo, "Talbedo", program, 2);
	glm::mat4 transformation = perspectiveCamera * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glDrawElements(
		GL_TRIANGLES,      // mode
		size,    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);
	glBindVertexArray(0);
	glUseProgram(0);
}
void RenderNormalMap::render(glm::mat4 perspectiveCamera, glm::mat4 modelMatrix, glm::vec3 lightDir, glm::vec3 cameraPos) {
	render(perspectiveCamera, modelMatrix, lightDir, cameraPos, glm::vec4(0.0));
}

RenderNormalMap::~RenderNormalMap()
{
}