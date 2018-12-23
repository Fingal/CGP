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

GLuint programTexture;

Core::Shader_Loader shaderLoader;

glm::vec3 cameraPos = glm::vec3(0, 0, 3);
glm::vec3 cameraDir;
glm::vec3 cameraSide;

glm::mat4 cameraMatrix, perspectiveMatrix;

glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f));

glm::quat rotation = glm::quat(1, 0, 0, 0);
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
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	
	sphere = RenderMaterial();
	sphere.loadModelFromFile("models/sphere.obj");
	sphere.setProgram(programTexture);
	sphere.loadMaterial("textures/rustediron2_roughness.png", "textures/rustediron2_metallic.png", "textures/rustediron2_basecolor.png");
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
