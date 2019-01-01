#include "RenderSkyBox.h"


#define BUFFER_OFFSET(i) ((char *)NULL + (i))

const float vertices[]{
	30.5f, 30.5f, 30.5f,
	30.5f, 30.5f, -30.5f,
	30.5f, -30.5f, 30.5f,
	30.5f, -30.5f, -30.5f,
	-30.5f, 30.5f, 30.5f,
	-30.5f, 30.5f, -30.5f,
	-30.5f, -30.5f, 30.5f,
	-30.5f, -30.5f, -30.5f,

};

const unsigned short indexes[]{
	0,
	2,
	4,

	2,
	6,
	4,

	1,
	5,
	3,

	3,
	5,
	7,

	4,
	6,
	7,

	4,
	7,
	5,

	0,
	3,
	2,

	0,
	1,
	3,

	1,
	0,
	4,

	1,
	4,
	5,

	3,
	6,
	2,

	3,
	7,
	6,
};


RenderSkyBox::RenderSkyBox()
{

}
void RenderSkyBox::init() {

	Core::Shader_Loader shaderLoader;
	program = shaderLoader.CreateProgram("shaders/sky.vert", "shaders/sky.frag");
	glGenVertexArrays(1, &VertexArray);
	glBindVertexArray(VertexArray);

	glGenBuffers(1, &VertexIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned short), indexes, GL_STATIC_DRAW);


	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 8 * 3 * sizeof(float), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * 3 * sizeof(float), vertices);
	//glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), vertices, GL_STATIC_DRAW);
	cubemapTexture = Core::setupCubeMap("textures/xpos.png", "textures/xneg.png", "textures/ypos.png", "textures/yneg.png", "textures/zpos.png", "textures/zneg.png");
	size = 36;
	vertexSize = 8;
	glPos = glGetAttribLocation(program, "vertexPosition");
	glEnableVertexAttribArray(glPos);
	glVertexAttribPointer(glPos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glBindVertexArray(0);
}
void RenderSkyBox::setProgram(GLuint& program) {
	glBindVertexArray(VertexArray);
	this->program = program;
	glPos = glGetAttribLocation(program, "vertexPosition");
	glEnableVertexAttribArray(glPos);
	glVertexAttribPointer(glPos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glBindVertexArray(0);
}
void RenderSkyBox::render(glm::mat4 perspectiveCameraMatrix,glm::vec4 clipPlane) {
	glUseProgram(program);
	glBindVertexArray(VertexArray);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glUniform4fv(glGetUniformLocation(program, "clip_plane"), 1, (float*)&clipPlane);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&perspectiveCameraMatrix);
	glDrawElements(
		GL_TRIANGLES,      // mode
		36,    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);
	glBindVertexArray(0);
	glUseProgram(0);

}
void RenderSkyBox::render(glm::mat4 perspectiveCameraMatrix) {
	render(perspectiveCameraMatrix, glm::vec4(0.0));

}

RenderSkyBox::~RenderSkyBox()
{
}
