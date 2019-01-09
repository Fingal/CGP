#include "RenderMaterial.h"



RenderMaterial::RenderMaterial()
{
}

void RenderMaterial::loadMaterial(const char* roughness, const char* metallic, const char* albedo) {
	textureRoughness	= Core::LoadTexture(roughness);
	textureMetallic		= Core::LoadTexture(metallic);
	textureAlbedo		= Core::LoadTexture(albedo);
}
void RenderMaterial::render(glm::mat4 perspectiveCamera,glm::mat4 modelMatrix, glm::vec3 lightPos, glm::vec3 cameraPos,glm::vec4 clipPlane) {
	glUseProgram(program);
	glBindVertexArray(VertexArray);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexIndexBuffer);
	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform4fv(glGetUniformLocation(program, "clip_plane"), 1, (float*)&clipPlane);
	Core::SetActiveTexture(textureMetallic, "Tmetallic", program, 0);
	Core::SetActiveTexture(textureRoughness, "Troughness", program, 1);
	Core::SetActiveTexture(textureAlbedo, "Talbedo", program, 2);
	Core::SetActiveTexture(depthTexture, "shadowSampler", program, 3);
	glm::mat4 transformation = perspectiveCamera;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&perspectiveCamera);
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

void RenderMaterial::render(RenderBundle bundle) {
	glUseProgram(program);
	glBindVertexArray(VertexArray);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexIndexBuffer);
	glm::vec3 lightPos = bundle.lightPos;
	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	glm::vec3 cameraPos = bundle.cameraPos;
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform4fv(glGetUniformLocation(program, "clip_plane"), 1, (float*)&bundle.clipPlane);
	Core::SetActiveTexture(textureMetallic, "Tmetallic", program, 0);
	Core::SetActiveTexture(textureRoughness, "Troughness", program, 1);
	Core::SetActiveTexture(textureAlbedo, "Talbedo", program, 2);
	Core::SetActiveTexture(depthTexture, "shadowSampler", program, 3);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&bundle.perspectiveCameraMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&bundle.modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "lightMVP"), 1, GL_FALSE, (float*)&bundle.lightMVP);
	glDrawElements(
		GL_TRIANGLES,      // mode
		size,    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);
	glBindVertexArray(0);
	glUseProgram(0);
}
void RenderMaterial::render(glm::mat4 perspectiveCamera, glm::mat4 modelMatrix, glm::vec3 lightDir, glm::vec3 cameraPos) {
	render(perspectiveCamera, modelMatrix, lightDir, cameraPos, glm::vec4(0.0));
}

RenderMaterial::~RenderMaterial()
{
}
