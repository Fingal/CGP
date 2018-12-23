#include "RenderMaterial.h"



RenderMaterial::RenderMaterial()
{
}

void RenderMaterial::loadMaterial(const char* roughness, const char* metallic, const char* albedo) {
	textureRoughness	= Core::LoadTexture(roughness);
	textureMetallic		= Core::LoadTexture(metallic);
	textureAlbedo		= Core::LoadTexture(albedo);
}

void RenderMaterial::render(glm::mat4 transformation, glm::vec3 lightDir, glm::vec3 cameraPos) {
	glUseProgram(program);
	glBindVertexArray(VertexArray);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexIndexBuffer);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	Core::SetActiveTexture(textureMetallic, "Tmetallic", program, 0);
	Core::SetActiveTexture(textureRoughness, "Troughness", program, 1);
	Core::SetActiveTexture(textureAlbedo, "Talbedo", program, 2);
	glm::mat4 modelMatrix(1.0f);
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

RenderMaterial::~RenderMaterial()
{
}
