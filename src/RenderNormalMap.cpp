#include "RenderNormalMap.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


RenderNormalMap::RenderNormalMap()
{
}
void RenderNormalMap::countTangents(obj::Model& model){
	std::vector<glm::vec3> tangents;
	for (int i = 0; i < normalSize; i++) {
		tangents.push_back(glm::vec3(0, 0, 0));
	}
	for (int i = 0; i < model.faces["default"].size(); i+=3) {
		glm::vec3 v0(model.vertex[3 * i], model.vertex[3 * i + 1], model.vertex[3 * i + 2]);
		glm::vec2 uv0(model.vertex[2 * i], model.vertex[2 * i + 1]);
		glm::vec3 v1(model.vertex[3 * (i + 1)], model.vertex[3 * (i + 1) + 1], model.vertex[3 * (i + 1) + 2]);
		glm::vec2 uv1(model.vertex[2 * (i + 1)], model.vertex[2 * (i + 1) + 1]);
		glm::vec3 v2(model.vertex[3 * (i + 2)], model.vertex[3 * (i + 2) + 1], model.vertex[3 * (i + 2) + 2]);
		glm::vec2 uv2(model.vertex[2 * (i + 2)], model.vertex[2 * (i + 2) + 1]);

		glm::vec3 Edge1 = v1 - v0;
		glm::vec3 Edge2 = v2 - v0;

		float DeltaU1 = uv1.x - uv0.x;
		float DeltaV1 = uv1.y - uv0.y;
		float DeltaU2 = uv2.x - uv0.x;
		float DeltaV2 = uv2.y - uv0.y;

		float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

		glm::vec3 Tangent, Bitangent;

		Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
		Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
		Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);

		Bitangent.x = f * (-DeltaU2 * Edge1.x - DeltaU1 * Edge2.x);
		Bitangent.y = f * (-DeltaU2 * Edge1.y - DeltaU1 * Edge2.y);
		Bitangent.z = f * (-DeltaU2 * Edge1.z - DeltaU1 * Edge2.z);

		tangents[i] -= Tangent;
		tangents[i+1] -= Tangent;
		tangents[i+2] -= Tangent;
	}
	std::vector<float> tangentValues;
	for (auto a : tangents) {
		a=glm::normalize(a);
		tangentValues.push_back(a.x);
		tangentValues.push_back(a.y);
		tangentValues.push_back(a.z);
	}
	glBindVertexArray(VertexArray);
	glGenBuffers(1, &tangentBuffer);
	glBufferData(GL_ARRAY_BUFFER, tangentValues.size()*sizeof(float), &tangentValues[0], GL_STATIC_DRAW);
	tangentSize = tangentValues.size();
	/*
	for (unsigned int i = 0; i < size; i += 3) {
		Vertex& v0 = Vertices[Indices[i]];
		Vertex& v1 = Vertices[Indices[i + 1]];
		Vertex& v2 = Vertices[Indices[i + 2]];

		Vector3f Edge1 = v1.m_pos - v0.m_pos;
		Vector3f Edge2 = v2.m_pos - v0.m_pos;

		float DeltaU1 = v1.m_tex.x - v0.m_tex.x;
		float DeltaV1 = v1.m_tex.y - v0.m_tex.y;
		float DeltaU2 = v2.m_tex.x - v0.m_tex.x;
		float DeltaV2 = v2.m_tex.y - v0.m_tex.y;

		float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

		Vector3f Tangent, Bitangent;

		Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
		Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
		Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);

		Bitangent.x = f * (-DeltaU2 * Edge1.x - DeltaU1 * Edge2.x);
		Bitangent.y = f * (-DeltaU2 * Edge1.y - DeltaU1 * Edge2.y);
		Bitangent.z = f * (-DeltaU2 * Edge1.z - DeltaU1 * Edge2.z);

		v0.m_tangent += Tangent;
		v1.m_tangent += Tangent;
		v2.m_tangent += Tangent;
	}

	for (unsigned int i = 0; i < Vertices.size(); i++) {
		Vertices[i].m_tangent.Normalize();
	}*/
}
void RenderNormalMap::setProgram(GLuint& program) {
	this->RenderTarget::setProgram(program);
	glBindVertexArray(VertexArray);
	glTan = glGetAttribLocation(program, "vertexTangent");
	glEnableVertexAttribArray(glTan);
	glVertexAttribPointer(glTan, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glBindVertexArray(0);

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
	Core::SetActiveTexture(textureNormal, "Tnormal", program, 3);
	Core::SetActiveTexture(depthTexture, "shadowSampler", program, 4);
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

void RenderNormalMap::render(RenderBundle bundle) {
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
	Core::SetActiveTexture(textureNormal, "Tnormal", program, 3);
	Core::SetActiveTexture(depthTexture, "shadowSampler", program, 4);
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
void RenderNormalMap::render(glm::mat4 perspectiveCamera, glm::mat4 modelMatrix, glm::vec3 lightDir, glm::vec3 cameraPos) {
	render(perspectiveCamera, modelMatrix, lightDir, cameraPos, glm::vec4(0.0));
}

RenderNormalMap::~RenderNormalMap()
{
}
