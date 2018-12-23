#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <vector>

#include "misc_tools.h"
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "RenderMaterial.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))


RenderMaterial sphere;

GLuint programColor;
GLuint programDepth;
GLuint programTexture;
GLuint programRender;

GLuint depthTexture;
GLuint FrameBufferObject;


GLuint VertexArrayID;
GLuint VertexBuffer;
GLuint VertexIndexBuffer;

Core::Shader_Loader shaderLoader;

obj::Model shipModel;
obj::Model sphereModel;
obj::Model renderModel;

//model sphereModel;

glm::vec3 cameraPos = glm::vec3(0, 0, 3);
glm::vec3 cameraDir;
glm::vec3 cameraSide;

glm::mat4 cameraMatrix, perspectiveMatrix;

glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f));

glm::quat rotation = glm::quat(1, 0, 0, 0);
glm::vec3 rotationChangeXYZ = glm::vec3(0, 0, 0);

GLuint textureAsteroid;
GLuint textureEarth;
GLuint textureMetallic;
GLuint textureRoughness;
GLuint textureAlbedo;

GLuint cubemapTexture;

std::vector<glm::vec4> planets;
std::vector<float> tangent(1203);

float quad[] = {
	0.25,0.25, 0.0, 1.0,
	0.25, -0.25, 0.0, 1.0,
	-0.25, -0.25, 0.0, 1.0,

	0.25, 0.25, 0.0, 1.0,
	-0.25, 0.25, 0.0, 1.0,
	-0.25, -0.25, 0.0, 1.0
};


void keyboard(unsigned char key, int x, int y)
{

	float angleSpeed = 0.1f;
	float moveSpeed = 0.1f;
	switch (key)
	{
	case 'z': rotationChangeXYZ.z += 0.1f; break;
	case 'x': rotationChangeXYZ.z -= 0.1f; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += cameraSide * moveSpeed; break;
	case 'a': cameraPos -= cameraSide * moveSpeed; break;
	}
}

void mouse(int x, int y)
{
	
}

glm::mat4 createCameraMatrix()
{
	glm::quat rotationChange = glm::quat(rotationChangeXYZ);
	rotation = rotationChange * rotation;
	rotation = glm::normalize(rotation);
	rotationChangeXYZ = glm::vec3(0);
	cameraDir = glm::inverse(rotation) * glm::vec3(0, 0, -1);
	cameraSide = glm::inverse(rotation) * glm::vec3(1, 0, 0);

	return Core::createViewMatrixQuat(cameraPos, rotation);
}



void drawObjectColor(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color)
{
	GLuint program = programColor;

	glUseProgram(program);


	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	
	Core::DrawModel(model);


	glUseProgram(0);
}


void drawObjectTexture(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programTexture;

	glUseProgram(program);
	//float metallic = 0.5f;
	//GLint metallicLoc = glGetUniformLocation(program, "metallic");
	//float roughness = 0.5f;
	//GLint roughnessLoc = glGetUniformLocation(programTexture, "roughness");
	//glUniform1f(roughnessLoc, roughness);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	Core::SetActiveTexture(textureId, "Tcolor", program, 0);
	Core::SetActiveTexture(textureMetallic, "Tmetallic", program, 1); 
	Core::SetActiveTexture(textureRoughness, "Troughness", program, 2);
	Core::SetActiveTexture(textureAlbedo, "Talbedo", program, 3);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}
//void drawVAOTexture(RenderTarget& target) {
//	glUseProgram(programTexture);
//	glBindVertexArray(target.VertexArray);
//	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexIndexBuffer);
//	glUniform3f(glGetUniformLocation(programTexture, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
//	glUniform3f(glGetUniformLocation(programTexture, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
//	Core::SetActiveTexture(textureEarth, "Tcolor", programTexture, 0);
//	Core::SetActiveTexture(textureMetallic, "Tmetallic", programTexture, 1);
//	Core::SetActiveTexture(textureRoughness, "Troughness", programTexture, 2);
//	Core::SetActiveTexture(textureAlbedo, "Talbedo", programTexture, 3);
//	glm::mat4 modelMatrix(1.0f);
//	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
//	glUniformMatrix4fv(glGetUniformLocation(programTexture, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
//	glUniformMatrix4fv(glGetUniformLocation(programTexture, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
//	glDrawElements(
//		GL_TRIANGLES,      // mode
//		target.size,    // count
//		GL_UNSIGNED_SHORT,   // type
//		(void*)0           // element array buffer offset
//	);
//	glBindVertexArray(0);
//	glUseProgram(0);
//}
void renderScene()
{
	
	
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.10f, 0.10f, 0.10f);
	

	glm::mat4 planetModelMatrix(1.0f);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * planetModelMatrix;
	// TODO: draw 25 spheres with varying metallic and roughness parameter values
	//drawObjectTexture(&sphereModel, planetModelMatrix, textureEarth);
	sphere.render(transformation, lightDir, cameraPos);
	glutSwapBuffers();
}

void init()
{
	srand(time(0));
	glEnable(GL_DEPTH_TEST);
	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	
	sphere = RenderMaterial();
	sphere.loadModelFromFile("models/sphere.obj");
	sphere.setProgram(programTexture);
	sphere.loadMaterial("textures/rustediron2_roughness.png", "textures/rustediron2_metallic.png", "textures/earth.png");




	/*glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	loadObj("models/sphere.obj", sphereModel.vertices, sphereModel.normals, sphereModel.UVs);
	int size = sphereModel.vertices.size() * sizeof(glm::vec3) + sphereModel.normals.size() * sizeof(glm::vec3) + sphereModel.UVs.size() * sizeof(glm::vec2);
	glBufferData(GL_ARRAY_BUFFER, size, NULL,GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sphereModel.vertices.size() * sizeof(glm::vec3), &sphereModel.vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sphereModel.vertices.size() * sizeof(glm::vec3), sphereModel.normals.size() * sizeof(glm::vec3), &sphereModel.normals[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sphereModel.vertices.size() * sizeof(glm::vec3) + sphereModel.normals.size() * sizeof(glm::vec3), sphereModel.UVs.size() * sizeof(glm::vec2), &sphereModel.UVs[0]);
	GLuint spherePos = glGetAttribLocation(programTexture, "vertexPosition");
	glEnableVertexAttribArray(spherePos);
	glVertexAttribPointer(spherePos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint sphereNor = glGetAttribLocation(programTexture, "vertexNormal");
	glEnableVertexAttribArray(sphereNor);
	glVertexAttribPointer(sphereNor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sphereModel.vertices.size() * sizeof(glm::vec3)));
	
	GLuint sphereUV = glGetAttribLocation(programTexture, "vertexTexCoord");
	glEnableVertexAttribArray(sphereUV);
	glVertexAttribPointer(sphereUV, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sphereModel.vertices.size() * sizeof(glm::vec3) + sphereModel.normals.size() * sizeof(glm::vec3)));
	glBindVertexArray(0);*/

	//textureEarth = Core::LoadTexture("textures/earth.png");
	//textureRoughness = Core::LoadTexture("textures/rustediron2_roughness.png");
	//textureMetallic = Core::LoadTexture("textures/rustediron2_metallic.png");
	//textureAlbedo = Core::LoadTexture("textures/rustediron2_basecolor.png");
	
}

void shutdown()	
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(800, 800);
	glutCreateWindow("CGP");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(mouse);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}
