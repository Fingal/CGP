#include "RenderTarget.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))





RenderTarget::RenderTarget()
{
}
void RenderTarget::drawElements() {
	glDrawElements(
		GL_TRIANGLES,      // mode
		size,    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);

}

void RenderTarget::loadModelFromFile(const char* fileName) {
	obj::Model model = obj::loadModelFromFile(fileName);
	_loadFromModel(model);
}
void RenderTarget::_loadFromModel(obj::Model& model) {
	glGenVertexArrays(1, &VertexArray);
	glBindVertexArray(VertexArray);

	glGenBuffers(1, &VertexIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.faces["default"].size() * sizeof(unsigned short), &model.faces["default"][0], GL_STATIC_DRAW);


	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	int buffer_size = model.vertex.size() * sizeof(float) + model.normal.size() * sizeof(float) + model.texCoord.size() * sizeof(glm::vec2);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, model.vertex.size() * sizeof(float), &model.vertex[0]);
	glBufferSubData(GL_ARRAY_BUFFER, model.vertex.size() * sizeof(float), model.normal.size() * sizeof(float), &model.normal[0]);
	glBufferSubData(GL_ARRAY_BUFFER, model.vertex.size() * sizeof(float) + model.normal.size() * sizeof(float), model.texCoord.size() * sizeof(glm::vec2), &model.texCoord[0]);
	glBindVertexArray(0);
	size = model.faces["default"].size();
	vertexSize = model.vertex.size();
	normalSize = model.normal.size();

}
void RenderTarget::setProgram(GLuint& program) {
	glBindVertexArray(VertexArray);
	this->program = program;
	glPos = glGetAttribLocation(program, "vertexPosition");
	glEnableVertexAttribArray(glPos);
	glVertexAttribPointer(glPos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glNor = glGetAttribLocation(program, "vertexNormal");
	glEnableVertexAttribArray(glNor);
	glVertexAttribPointer(glNor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize * sizeof(float)));

	glUV = glGetAttribLocation(program, "vertexTexCoord");
	glEnableVertexAttribArray(glUV);
	glVertexAttribPointer(glUV, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize * sizeof(float) + normalSize * sizeof(float)));
	glBindVertexArray(0);
}
void RenderTarget::render(glm::mat4 transformation, glm::vec3 lightDir, glm::vec3 cameraPos,glm::vec4 clipPlane) {
	glUseProgram(program);
	glBindVertexArray(VertexArray);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexIndexBuffer);
	glUniform3f(glGetUniformLocation(program, "lightPos"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform4fv(glGetUniformLocation(program, "clip_plane"), 1,(float*)&clipPlane);
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

void RenderTarget::render(glm::mat4 transformation, glm::vec3 lightDir, glm::vec3 cameraPos) {
	render(transformation, lightDir, cameraPos, glm::vec4(0.0));
}
RenderTarget::~RenderTarget()
{
}
