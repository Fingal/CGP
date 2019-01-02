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
#include "RenderSkyBox.h"
#include "RenderWater.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
double clipPlaneUp[] = { 0.0,1.0,0.0,-40000.0 };
double clipPlaneDown[] = { 0.0,-1.0,0.0,0.0 };

RenderMaterial sphere;
RenderMaterial boat;
RenderSkyBox skyBox;
RenderWater water;


GLuint programTexture;
GLuint programSky;

GLuint reflectTexture;
GLuint reflectDepthTexture;
GLuint reflectionFramebufferObject;

GLuint refractTexture;
GLuint refractDepthTexture;
GLuint refractionFramebufferObject;

Core::Shader_Loader shaderLoader;

glm::vec3 cameraPos = glm::vec3(0, 2, 3);
glm::vec3 cameraDir;
glm::vec3 cameraSide;

glm::mat4 cameraMatrix, perspectiveMatrix;

//glm::vec3 lightPos = (glm::vec3(12.5, 29, -16));
glm::vec3 lightPos = (glm::vec3(12.5, 29, -16));

glm::quat rotation = glm::quat(1, 0, 0, 0);
glm::quat rotationReflected = glm::quat(1, 0, 0, 0);

glm::vec3 rotationChangeXYZ = glm::vec3(0, 0, 0);


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

float sensitivity = 0.02;
float flatten = 0.0;
void keyboard(unsigned char key, int x, int y)
{

	float angleSpeed = 0.1f;
	float moveSpeed = 0.1f;
	switch (key)
	{
	//case 'z': rotationChangeXYZ.z += 0.1f; break;
	//case 'x': rotationChangeXYZ.z -= 0.1f; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += cameraSide * moveSpeed; break;
	case 'a': cameraPos -= cameraSide * moveSpeed; break;
	case '+': sensitivity = (sensitivity < 0.2) ? sensitivity + 0.2 : 0.02; ; break;
	case 'f': flatten = (flatten < 1.0) ? flatten + 0.1 : 0.00; ; break;
	}
}

void mouse(int x, int y)
{
	static int lastMouseX = x;
	static int lastMouseY = y;
	rotationChangeXYZ.y = sensitivity *(x - lastMouseX);
	rotationChangeXYZ.x = sensitivity *(y - lastMouseY);
	lastMouseX = x;
	lastMouseY = y;
}
glm::mat4 createReflectedCameraMatrix()
{
	glm::vec3 reflectionRotation(-rotationChangeXYZ.x, rotationChangeXYZ.y, rotationChangeXYZ.z);
	glm::vec3 reflectionPos(cameraPos.x, -cameraPos.y, cameraPos.z);
	glm::vec3 x = rotation * glm::vec3(0,0,-1);

	glm::quat rotationChange = glm::quat(reflectionRotation);
	rotationReflected = rotationChange * rotationReflected;
	rotationReflected = glm::normalize(rotationReflected);
	reflectionRotation = glm::vec3(0);

	return Core::createViewMatrixQuat(reflectionPos, rotationReflected);
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




void renderObjects(glm::mat4 prespectiveCameraMatrix, glm::vec4 clipPlane) {
	skyBox.render(prespectiveCameraMatrix, clipPlane);
	glm::mat4 modelMatrix = glm::translate(glm::vec3(1, -0.3, -4))*glm::scale(glm::vec3(0.2));
	boat.render(prespectiveCameraMatrix, modelMatrix, lightPos, cameraPos, clipPlane);


	modelMatrix = glm::translate(glm::vec3(19, 0, 5));
	sphere.render(prespectiveCameraMatrix, modelMatrix, lightPos, cameraPos, clipPlane);
	modelMatrix = glm::translate(glm::vec3(8, -4, 0));
	sphere.render(prespectiveCameraMatrix, modelMatrix, lightPos, cameraPos, clipPlane);

}
void renderReflection()
{
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFramebufferObject);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_CLIP_PLANE0);

	//std::cout << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << "\n";

	glm::vec3 reflectionPos(cameraPos.x, -cameraPos.y, cameraPos.z);
	glm::mat4 cameraMatrix = createReflectedCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();
	glm::mat4 prespectiveCameraMatrix = perspectiveMatrix * cameraMatrix;

	renderObjects(prespectiveCameraMatrix, glm::vec4(0, 1, 0, -0.1));


	glDisable(GL_CLIP_PLANE0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
void renderRefraction()
{
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFramebufferObject);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_CLIP_PLANE0);

	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();
	glm::mat4 prespectiveCameraMatrix = perspectiveMatrix * cameraMatrix;
	if (cameraPos.y < 0) {
		renderObjects(prespectiveCameraMatrix, glm::vec4(0, 1, 0, 0.0));
	}
	else {
		renderObjects(prespectiveCameraMatrix, glm::vec4(0, -1, 0, -0.0));
	}
	
	

	glDisable(GL_CLIP_PLANE0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
void initFBO(GLuint& FBO, GLuint& colorTexture, GLuint& depthTexture) {
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//Generate depth texture
	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_FLOAT, NULL);

	//Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Attach color texture to frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);


	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	//Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Attach depth texture to frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderScene()
{
	
	renderReflection();
	renderRefraction();
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();
	glm::mat4 prespectiveCameraMatrix = perspectiveMatrix * cameraMatrix;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 0.10f, 0.10f, 0.10f);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	renderObjects(prespectiveCameraMatrix, glm::vec4(0, 0, 0, 0));
	water.render(perspectiveMatrix, cameraMatrix, lightPos, cameraPos);
	glDisable(GL_BLEND);
	glutSwapBuffers();
}

void init()
{
	srand(time(0));
	glEnable(GL_DEPTH_TEST);
	programSky = shaderLoader.CreateProgram("shaders/sky.vert", "shaders/sky.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");

	boat = RenderMaterial();
	boat.loadModelFromFile("models/OldBoat.obj");
	boat.setProgram(programTexture);
	boat.loadMaterial("textures/boat_roughness.png", "textures/boat_metallic.png", "textures/boat_tex.png");


	skyBox = RenderSkyBox();
	skyBox.init();
	skyBox.setProgram(programSky);


	sphere = RenderMaterial();
	sphere.loadModelFromFile("models/sphere.obj");
	sphere.setProgram(programTexture);
	sphere.loadMaterial("textures/rustediron2_roughness.png", "textures/rustediron2_metallic.png", "textures/rustediron2_basecolor.png");



	initFBO(reflectionFramebufferObject, reflectTexture, reflectDepthTexture);
	initFBO(refractionFramebufferObject, refractTexture, refractDepthTexture);


	water = RenderWater();
	water.init(reflectTexture, refractTexture, refractDepthTexture);
}

void shutdown()	
{
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
	glutInitWindowSize(1024, 1024);
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
