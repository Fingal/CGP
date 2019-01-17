#include "glew.h"
#include <ctime>
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
#include "RenderNormalMap.h"
#include "RenderTexture.h"
#include "RenderBundle.cpp"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
double clipPlaneUp[] = { 0.0,1.0,0.0,-40000.0 };
double clipPlaneDown[] = { 0.0,-1.0,0.0,0.0 };

struct particle {
	RenderTexture* model;
	glm::vec3 location;
	glm::vec3 speed;
	glm::mat4 rotation;
	glm::vec3 rotation_axis;
};
std::vector<particle> fishes;

RenderTexture fish;
RenderMaterial sphere;
RenderNormalMap sphere2;
RenderNormalMap boat;
RenderSkyBox skyBox;
RenderWater water;


GLuint programTexture;
GLuint programMaterial;
GLuint programSky;
GLuint programNormal;
GLuint programDepth;

GLuint reflectTexture;
GLuint reflectDepthTexture;
GLuint reflectionFramebufferObject;

GLuint refractTexture;
GLuint refractDepthTexture;
GLuint refractionFramebufferObject;

GLuint depthTexture;
GLuint depthFramebufferObject;

Core::Shader_Loader shaderLoader;

glm::vec3 cameraPos = glm::vec3(0, 2, 3);
glm::vec3 cameraDir;
glm::vec3 cameraSide;

glm::mat4 cameraMatrix, perspectiveMatrix;

//glm::vec3 lightPos = (glm::vec3(12.5, 29, -16));
glm::vec3 lightPos = (glm::vec3(12.5, 29, -16));
glm::vec3 lightPoses[] = { (glm::vec3(12.5, 29, -16)) , (glm::vec3(1, 10, -4)) , (glm::vec3(1, -10, -4))  , (glm::vec3(10, 0, -4)) };
int lightIndex = 1;
glm::quat rotation = glm::quat(1, 0, 0, 0);
glm::quat rotationReflected = glm::quat(1, 0, 0, 0);

glm::vec3 rotationChangeXYZ = glm::vec3(0, 0, 0);


GLuint cubemapTexture;

std::vector<glm::vec4> planets;
std::vector<float> tangent(1203);


float frame_time;

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
float coef1 = 0.0;
float coef2 = 0.0;
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
	case '1': coef1 = (coef1 < 1.0) ? coef1 + 0.1 : 0.00; ; break;
	case '2': coef2 = (coef2 < 1.0) ? coef1 + 0.1 : 0.00; ; break;
	case 'f': flatten = flatten + 0.1; ; break;
	case 'l': lightPos = lightPoses[(lightIndex++)%4]; ; break;
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


float old_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
void countFishes()
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f ;
	if ((time - old_time) > 1 || (time - old_time) < 0) {
		old_time = time - 0.1;
	}

	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.50f, 0.50f, 0.50f, 1.0f);

	glm::mat4 FishModelMatrix;
	glm::vec3 attractor(60 * sin(time / 2),-120, 60 * cos(time / 2));
	//glm::vec3 attractor(0,0, 10);
	float cohesion_distance = 50;
	float aligment_distance = 40;
	float separation_distance = 30;
	float dt = 0.05;
	float view_angle = 0.2f;
	std::vector<particle> newParticles;
	for (auto &adam : fishes) {
		particle eva;
		eva.model = adam.model;
		eva.speed = adam.speed;
		glm::vec3 cohesion(0.0, 0.0, 0.0);
		glm::vec3 aligment(0.0, 0.0, 0.0);
		float cohesion_size = 0;
		float aligment_size = 0;
		for (auto &lilit : fishes) {
			float distance = glm::length(adam.location - lilit.location);
			if (distance < cohesion_distance && distance > 0.01f && glm::dot(glm::normalize(adam.speed), glm::normalize(lilit.location - adam.location)) > view_angle) {
				if (distance < aligment_distance) {
					aligment += (lilit.speed - adam.speed);
					aligment_size += 1;
				}
				cohesion_size += 1;
				cohesion += glm::normalize(lilit.location - adam.location);
			}
		}
		cohesion_size = (cohesion_size > 1) ? cohesion_size : 1;
		aligment_size = (aligment_size > 1) ? aligment_size : 1;
		glm::vec3 separation(0.0, 0.0, 0.0);
		float separation_size = 0;
		for (auto &lilit : fishes) {
			float distance = glm::length(adam.location - lilit.location);
			if (glm::length(adam.location - lilit.location) < separation_distance && distance > 0.001f) {
				separation_size += 1;
				separation -= (lilit.location - adam.location) / (distance*distance);
			}
		}
		separation_size = (separation_size > 1) ? separation_size : 1;
		glm::vec3 xxx(0);
		if (glm::length(attractor - adam.location) > 0) {
			xxx = (attractor - adam.location)*glm::inversesqrt(glm::length(attractor - adam.location));
		}
		//std::cout << adam.location.x << "\n";
		//std::cout << glm::length(attractor - adam.location) << "\n";
		//std::cout << time-old_time<< std::endl;

		//myfile << separation.x << " " << separation.y << " " << separation.z << ' '<< separation_size << "\n";
		//myfile << aligment.x << " " << aligment.y << " " << aligment.z << ' ' << aligment_size << "\n\n";
		//myfile << time << " " << old_time << "\n" ;
		//myfile << separation.x << " " << separation.y << " " << separation.z << "\n";
		if (eva.location.y > 0) {
			eva.speed -= glm::vec3(0, 100 * (time - old_time), 0);
		}
		else {
			eva.speed = eva.speed + (40.0f * separation + 0.20f * cohesion / cohesion_size + 0.300f * aligment / aligment_size + 10 * xxx)*(time - old_time);
		}
		eva.location = adam.location + eva.speed*(time - old_time);

		glm::vec3 rotation_axis = glm::normalize(glm::cross(glm::vec3(0.0, 0.0, 1.0), glm::normalize(eva.speed)));
		float angle = glm::acos(glm::dot(rotation_axis, glm::normalize(eva.speed)));
		//myfile << angle << '\n';
		//myfile << rotation_axis.x << ' ' << rotation_axis.y << ' ' << rotation_axis.z << ' ' << '\n';

		/*if (glm::length(glm::cross(glm::vec3(0.0, 0.0, 1.0), glm::normalize(eva.speed))) < 0.00001) {
			rotation_axis = glm::vec3(0.0, 0.0, 1.0);
			angle = 0.0;
			eva.rotation = adam.rotation;
		}
		else {
			glm::mat4 rotation = glm::rotate(angle, rotation_axis);
			eva.rotation = glm::toMat4(glm::normalize(glm::quat(adam.rotation) + glm::quat(rotation)*0.5f));
		}
		*/
		glm::mat4 rotation = glm::rotate(angle, rotation_axis);
		eva.rotation = glm::toMat4(glm::normalize(glm::quat(adam.rotation) + glm::quat(rotation)*10.5f));
		newParticles.push_back(eva);
	}
	fishes = newParticles;
	old_time = time;
}

glm::mat4 lightMVP() {

	glm::mat4 perspectiveMatrix = glm::ortho<float>(-61, 61, -61, 61, 20, 91);
	glm::mat4 cameraMatrix = glm::lookAt<float>(lightPos, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
	return (perspectiveMatrix * cameraMatrix);
}
glm::mat4 fish_model_matrix(particle fish) {
	return glm::translate(fish.location / 10.0)*fish.rotation*glm::scale(glm::vec3(0.25))*glm::translate(glm::vec3(2.5, 0, -1));
}


void renderShadow(glm::vec3 lightPos, glm::mat4 translations[]) {

	glBindFramebuffer(GL_FRAMEBUFFER, depthFramebufferObject);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glm::mat4 trans = lightMVP();
	glm::mat4 lightMVP = trans;

	lightMVP = trans * translations[0];
	boat.renderShadow(lightMVP);


	lightMVP = trans * translations[1];
	sphere2.renderShadow(lightMVP);

	lightMVP = trans * translations[2];
	sphere.renderShadow(lightMVP);

	lightMVP = trans * translations[3];
	sphere.renderShadow(lightMVP);
	/*for (int i = -10; i <=10; i++) {
		for (int j = -10; j <= 10; j++) {
			lightMVP = trans * glm::translate(glm::vec3(j,1,i)*5);
			sphere.renderShadow(lightMVP);
		}
	}*/

	for (auto fish : fishes) {

		lightMVP = trans * fish_model_matrix(fish);
		//lightMVP = trans * translate(glm::vec3(1, -2.3, -4));
		fish.model->renderShadow(lightMVP);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void renderObjects(glm::mat4 perspectiveCameraMatrix, glm::mat4 translations[], glm::vec4 clipPlane) {
	RenderBundle bundle;
	bundle.cameraPos = cameraPos;
	bundle.lightPos = lightPos;
	bundle.clipPlane = clipPlane;
	bundle.perspectiveCameraMatrix = perspectiveCameraMatrix;
	bundle.lightMVP = lightMVP();
	bundle.modelMatrix = translations[0];

	skyBox.render(bundle);

	glm::mat4 modelMatrix = glm::translate(glm::vec3(1, -0.3, -4))*glm::eulerAngleY(flatten)*glm::scale(glm::vec3(0.2));
	boat.render(bundle);

	
	bundle.modelMatrix = translations[1];
	modelMatrix = glm::translate(glm::vec3(3, 0, 2))*glm::translate(glm::vec3(1, -0.3, -4));
	sphere2.render(bundle);

	modelMatrix = glm::translate(glm::vec3(3, 2, 2))*glm::translate(glm::vec3(1, -0.3, -4));

	bundle.modelMatrix = translations[2];
	sphere.render(bundle);

	modelMatrix = glm::translate(glm::vec3(8, -4, 0));
	bundle.modelMatrix = translations[3];

	//for (int i = -10; i <= 10; i++) {
	//	for (int j = -10; j <= 10; j++) {
	//		bundle.modelMatrix = glm::translate(glm::vec3(j, 1, i) * 5);
	//		sphere.render(bundle);
	//	}
	//}
	for (auto fish : fishes) {
		/// *glm::translate(glm::vec3(2.5,0,-1)) to center model
		bundle.modelMatrix = fish_model_matrix(fish);
		
		//bundle.modelMatrix = glm::rotate( glutGet(GLUT_ELAPSED_TIME) / 1000.0f, glm::vec3(0, 1, 0))*glm::translate(glm::vec3(2.5,0,-1));
		//bundle.modelMatrix = translate(glm::vec3(1, -2.3, -4));
		fish.model->render(bundle);
		glm::vec3 xxxx = fish.location / 20.0;
		//std::cout << " " << xxxx.x << " " << xxxx.y << " "<< xxxx.z << std::endl;
	}

}
void renderReflection(glm::mat4 translations[])
{
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFramebufferObject);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_CLIP_PLANE0);

	//std::cout << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << "\n";

	glm::vec3 reflectionPos(cameraPos.x, -cameraPos.y, cameraPos.z);
	glm::mat4 cameraMatrix = createReflectedCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix(0.1,200);
	glm::mat4 perspectiveCameraMatrix = perspectiveMatrix * cameraMatrix;

	renderObjects(perspectiveCameraMatrix, translations, glm::vec4(0, 1, 0, -0.0));


	glDisable(GL_CLIP_PLANE0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
void renderRefraction(glm::mat4 translations[])
{
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFramebufferObject);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_CLIP_PLANE0);

	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();
	glm::mat4 perspectiveCameraMatrix = perspectiveMatrix * cameraMatrix;
	if (cameraPos.y < 0) {
		renderObjects(perspectiveCameraMatrix, translations, glm::vec4(0, 1, 0, 0.0));
	}
	else {
		renderObjects(perspectiveCameraMatrix, translations, glm::vec4(0, -1, 0, -0.0));
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
	countFishes();
	glm::mat4 translations[] = {	glm::translate(glm::vec3(1, -0.3, -4))*glm::eulerAngleY(flatten)*glm::scale(glm::vec3(0.2)),
									glm::translate(glm::vec3(5, 2, 2))*glm::translate(glm::vec3(1, -0.3, -4)),
									glm::translate(glm::vec3(5, 4, 2))*glm::translate(glm::vec3(1, -0.3, -4)),
									glm::translate(glm::vec3(8, -4, 0)) };

	
	frame_time = time(0);
	renderShadow(lightPos, translations);
	renderReflection(translations);
	renderRefraction(translations);
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix(0.1,200);
	glm::mat4 perspectiveCameraMatrix = perspectiveMatrix * cameraMatrix;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 0.10f, 0.10f, 0.10f);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	renderObjects(perspectiveCameraMatrix, translations, glm::vec4(0, 0, 0, 0));
	water.render(perspectiveMatrix, cameraMatrix, lightMVP(), lightPos, cameraPos);
	glDisable(GL_BLEND);

	glutSwapBuffers();
}

void RS() {
	countFishes();
	glm::mat4 translations[] = { glm::translate(glm::vec3(1, -0.05, -4))*glm::eulerAngleY(flatten)*glm::scale(glm::vec3(0.2)),
		glm::translate(glm::vec3(5, 2, 2))*glm::translate(glm::vec3(1, -0.3, -4)),
		glm::translate(glm::vec3(5, 4, 2))*glm::translate(glm::vec3(1, -0.3, -4)),
		glm::translate(glm::vec3(8, -4, 0)) };


	frame_time = time(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.00f, 1.0f);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glm::mat4 trans = lightMVP();
	glm::mat4 lightMVP = trans;

	lightMVP = trans * translations[0];
	boat.renderShadow(lightMVP);


	lightMVP = trans * translations[1];
	sphere2.renderShadow(lightMVP);

	lightMVP = trans * translations[2];
	sphere.renderShadow(lightMVP);

	lightMVP = trans * translations[3];
	sphere.renderShadow(lightMVP);
	/*for (int i = -10; i <=10; i++) {
	for (int j = -10; j <= 10; j++) {
	lightMVP = trans * glm::translate(glm::vec3(j,1,i)*5);
	sphere.renderShadow(lightMVP);
	}
	}*/

	for (auto fish : fishes) {

		lightMVP = trans * fish_model_matrix(fish);
		//lightMVP = trans * translate(glm::vec3(1, -2.3, -4));
		fish.model->renderShadow(lightMVP);
	}
	glDisable(GL_BLEND);

	glutSwapBuffers();

}
void init_fish() {



	fish = RenderTexture();
	fish.loadModelFromFile("models/fish.obj");
	fish.setProgram(programTexture);
	fish.loadTexture("textures/fish_texture.png");
	fish.setShadowProgram(programDepth, depthTexture);
	//create fish particles
	//...
	int i = 100;
	while (i--) {
		particle p;
		p.model = &fish;
		p.location = glm::vec3(glm::linearRand(-10, 10), glm::linearRand(-220, -185), glm::linearRand(-10, 10));
		p.speed = glm::vec3(0.0, 0.0, 30.0);
		p.rotation_axis = glm::vec3(0.0, 0.0, 1.0);
		p.rotation = glm::mat4(1);
		fishes.push_back(p);
	}

}


void init()
{
	srand(time(0));
	glEnable(GL_DEPTH_TEST);
	programSky = shaderLoader.CreateProgram("shaders/sky.vert", "shaders/sky.frag");
	programMaterial = shaderLoader.CreateProgram("shaders/shader_material.vert", "shaders/shader_material.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	programNormal = shaderLoader.CreateProgram("shaders/shader_material_normal.vert", "shaders/shader_material_normal.frag");
	programDepth = shaderLoader.CreateProgram("shaders/shader_depth.vert", "shaders/shader_depth.frag");

	//Generate Framebuffer
	depthFramebufferObject = 0;
	glGenFramebuffers(1, &depthFramebufferObject);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFramebufferObject);

	//Generate depth texture
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	//Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Attach depth texture to frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	boat = RenderNormalMap();
	boat.loadModelFromFile("models/OldBoat.obj");
	boat.setProgram(programNormal);
	boat.loadMaterial("textures/boat_roughness.png", "textures/boat_metallic.png", "textures/boat_tex.png");
	boat.loadNormal("textures/boatt_nm.png");
	boat.setShadowProgram(programDepth, depthTexture);

	skyBox = RenderSkyBox();
	skyBox.init();
	skyBox.setProgram(programSky);
	skyBox.setShadowProgram(programDepth, depthTexture);


	sphere = RenderMaterial();
	sphere.loadModelFromFile("models/sphere.obj");
	sphere.setProgram(programMaterial);
	sphere.loadMaterial("textures/rustediron2_roughness.png", "textures/rustediron2_metallic.png", "textures/rustediron2_basecolor.png");
	sphere.setShadowProgram(programDepth, depthTexture);


	sphere2 = RenderNormalMap();
	sphere2.loadModelFromFile("models/sphere.obj");
	sphere2.setProgram(programNormal);
	sphere2.loadMaterial("textures/rustediron2_roughness.png", "textures/rustediron2_metallic.png", "textures/rustediron2_basecolor.png");
	sphere2.loadNormal("textures/rustediron2_normal1.png");
	sphere2.setShadowProgram(programDepth, depthTexture);



	initFBO(reflectionFramebufferObject, reflectTexture, reflectDepthTexture);
	initFBO(refractionFramebufferObject, refractTexture, refractDepthTexture);


	water = RenderWater();
	water.init(reflectTexture, refractTexture, refractDepthTexture);
	water.setShadowProgram(programDepth, depthTexture);

	init_fish();
}
void init_no_texture()
{
	srand(time(0));
	glEnable(GL_DEPTH_TEST);
	programSky = shaderLoader.CreateProgram("shaders/sky.vert", "shaders/sky.frag");
	programMaterial = shaderLoader.CreateProgram("shaders/shader_material.vert", "shaders/shader_material.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	programNormal = shaderLoader.CreateProgram("shaders/shader_material_normal.vert", "shaders/shader_material_normal.frag");
	programDepth = shaderLoader.CreateProgram("shaders/shader_depth.vert", "shaders/shader_depth.frag");

	//Generate Framebuffer
	depthFramebufferObject = 0;
	glGenFramebuffers(1, &depthFramebufferObject);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFramebufferObject);

	//Generate depth texture
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	//Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Attach depth texture to frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	boat = RenderNormalMap();
	boat.loadModelFromFile("models/OldBoat.obj");
	boat.setProgram(programNormal);
	boat.setShadowProgram(programDepth, depthTexture);

	skyBox = RenderSkyBox();
	skyBox.init();
	skyBox.setProgram(programSky);
	skyBox.setShadowProgram(programDepth, depthTexture);


	sphere = RenderMaterial();
	sphere.loadModelFromFile("models/sphere.obj");
	sphere.setProgram(programMaterial);
	sphere.setShadowProgram(programDepth, depthTexture);


	sphere2 = RenderNormalMap();
	sphere2.loadModelFromFile("models/sphere.obj");
	sphere2.setProgram(programNormal);
	sphere2.setShadowProgram(programDepth, depthTexture);



	initFBO(reflectionFramebufferObject, reflectTexture, reflectDepthTexture);
	initFBO(refractionFramebufferObject, refractTexture, refractDepthTexture);


	water = RenderWater();
	water.init(reflectTexture, refractTexture, refractDepthTexture);
	water.setShadowProgram(programDepth, depthTexture);

	init_fish();
}

void shutdown()	
{
	shaderLoader.DeleteProgram(programSky);
	shaderLoader.DeleteProgram(programMaterial);
	shaderLoader.DeleteProgram(programTexture);
	shaderLoader.DeleteProgram(programNormal);
	shaderLoader.DeleteProgram(programDepth);
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
	//init_no_texture();
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(mouse);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}
