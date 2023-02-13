#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <math.h>

#include "SOIL/stb_image_aug.h"
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "SOIL/SOIL.h"
#include "Texture.h"

#include "Box.cpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include "PerlinNoise.hpp"

const float SHADOW_WIDTH = 3000, SHADOW_HEIGHT = 3000;
int WIDTH = 1920, HEIGHT = 1080;

namespace models {
	Core::RenderContext bedContext;
	Core::RenderContext chairContext;
	Core::RenderContext cozy1Context;
	Core::RenderContext cozy2Context;
	Core::RenderContext cozy2Drawer1Context;
	Core::RenderContext cozy2Drawer2Context;
	Core::RenderContext cozy2Drawer3Context;
	Core::RenderContext cozy2Drawer4Context;
	Core::RenderContext cremeContext;
	Core::RenderContext deskContext;
	Core::RenderContext deskDrawer1Context;
	Core::RenderContext deskDrawer2Context;
	Core::RenderContext deskDrawer3Context;
	Core::RenderContext deskDrawer4Context;
	Core::RenderContext door1Context;
	Core::RenderContext door2Context;
	Core::RenderContext floorContext;
	Core::RenderContext gamesDeskContext;
	Core::RenderContext games1Context;
	Core::RenderContext games2Context;
	Core::RenderContext games3Context;
	Core::RenderContext games4Context;
	Core::RenderContext games5Context;
	Core::RenderContext jarContext;
	Core::RenderContext keyboardContext;
	Core::RenderContext lampContext;
	Core::RenderContext lamp2Context;
	Core::RenderContext materacContext;
	Core::RenderContext monitorHolderContext;
	Core::RenderContext monitor1Context;
	Core::RenderContext monitor2Context;
	Core::RenderContext monitor3Context;
	Core::RenderContext mouseContext;
	Core::RenderContext paperContext;
	Core::RenderContext wallsContext;
	Core::RenderContext books1Context;
	Core::RenderContext books2Context;
	Core::RenderContext books3Context;
	Core::RenderContext playstationFigureContext;
	Core::RenderContext nightCozyContext;
	Core::RenderContext nightCozyDrawer1Context;
	Core::RenderContext nightCozyDrawer2Context;
	Core::RenderContext duvetContext;
	Core::RenderContext pillow1Context;
	Core::RenderContext pillow2Context;
	Core::RenderContext pillow3Context;
	Core::RenderContext pillow4Context;
	Core::RenderContext pillow5Context;
	Core::RenderContext windowContext;
	//Core::RenderContext testContext;
	Core::RenderContext roombaContext;
	Core::RenderContext meshContext;
	Core::RenderContext sphereContext;
}

namespace textures {
	GLuint floor;
	GLuint door;
	GLuint walls;
	GLuint materac;
	GLuint chair;
	GLuint pc;
	GLuint xbox;
	GLuint playstation;
	GLuint switchT;
	GLuint jar;
	GLuint white;
	GLuint black;
	GLuint yellow;
	GLuint books;
	GLuint playstationFigure;
	GLuint pillow;
	GLuint duvet;
}

GLuint depthMapFBO;
GLuint depthMap;

GLuint program;
GLuint programSun;
GLuint programTest;
GLuint programTex;
GLuint programDepth;
GLuint programSkybox;
GLuint cube_map_texture;
GLuint programMesh;

Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext sphereContext;
Core::RenderContext skyboxContext;

glm::vec3 sunPos = glm::vec3(-4.740971f, 2.149999f, 0.369280f);
glm::vec3 sunDir = glm::vec3(-0.93633f, 0.351106, 0.003226f);
glm::vec3 sunColor = glm::vec3(0.9f, 0.9f, 0.7f) * 5;

glm::vec3 cameraPos = glm::vec3(0.479490f, 1.250000f, -2.124680f);
glm::vec3 cameraDir = glm::vec3(-0.354510f, 0.000000f, 0.935054f);


glm::vec3 spaceshipPos = glm::vec3(0.065808f, 1.250000f, -2.189549f);
glm::vec3 spaceshipDir = glm::vec3(-0.490263f, 0.000000f, 0.871578f);

glm::mat4 roombaPosition;
glm::vec3 roombaDir = glm::vec3(0, 0, 0);
glm::mat4 roombaRotationMatrix =
{ cos(0),0,sin(0),0,
  0,1,0,0,
  -sin(0),0,cos(0),0,
  0,0,0,1 };

GLuint VAO, VBO;

glm::vec3 quadPos;

int mouseScroll = 0;
int mouseVertical = 0;
int mouseHorizontal = 0;
float mousePositionX = 0;
float mousePositionY = 0;
bool mouseLower = false;
bool mouseUpper = false;

float aspectRatio = 1.f;

float exposition = 10.f;
float exposition2 = 10.f;
float yCameraDir;

glm::vec3 pointlightPos = glm::vec3(0, 2, 0);
glm::vec3 pointlightColor = glm::vec3(0.9, 0.6, 0.6);


glm::vec3 pointlightPos2 = glm::vec3(1.5804f, 1.385f, 1.3198f);
glm::vec3 pointlightColor2 = glm::vec3(0.93, 0.93, 0.0);

glm::vec3 spotlightPos = glm::vec3(0, 0, 0);
glm::vec3 spotlightConeDir = glm::vec3(0, 0, 0);
glm::vec3 spotlightColor = glm::vec3(0.4, 0.4, 0.9) * 5;
float spotlightPhi = 3.14 / 4;

glm::mat4 lightVP = glm::ortho(-4.f, 2.5f, -2.f, 5.f, 1.0f, 30.0f) * glm::lookAt(sunPos, sunPos - sunDir, glm::vec3(0, 1, 0));

float lastTime = -1.f;
float deltaTime = 0.f;

// _____________ NOISE ____________

const siv::PerlinNoise::seed_type seed = (UINT)glfwGetTime;
const siv::PerlinNoise perlin{ seed };

// _____________ MESH ____________
glm::mat4 meshPos;

std::vector<glm::vec3> meshVertices, meshNormals;
std::vector<glm::uvec3> meshTriangles;

const int meshWidth = 10;
const int meshHeight = 10;

bool isDrawerOpen1 = false;
bool isDrawerOpen2 = false;
bool isDrawerOpen3 = false;
bool isDrawerOpen4 = false;


glm::mat4 drawerPosition;

void createTerrainMesh() {

	static GLuint terrainVBO = 0, terrainVBONormals = 0, terrainEBO = 0;

	const double d = 0.4f;

	int octaves = 100;

	int i = 0;

	glm::vec3 start = glm::vec3(2.35033, 0.0, 3.76054);
	glm::vec3 end = glm::vec3(-0.075839, 3.29564, 3.76054);

	double y_diff = (end.y - start.y) / meshHeight;
	double x_diff = (start.x - end.x) / meshWidth;

	//std::cout << y_diff << " " << x_diff << std::endl;

	// Iterate through each pixel of the heightmap image
	for (double y = start.y; y < end.y + y_diff; y += y_diff) {
		//std::cout << y << std::endl;
		for (double x = end.x; x < start.x + x_diff; x += x_diff) {
			//std::cout << perlin.octave2D_01((x), (y), octaves) << std::endl;
			// create vertex using height map
			glm::vec3 v0 = glm::vec3(x, y, (perlin.octave2D_01((x), (y), octaves) * d) + start.z - d);
			glm::vec3 v1 = glm::vec3(x, (y + y_diff), (perlin.octave2D_01((x), (y + y_diff), octaves) * d) + start.z - d);
			glm::vec3 v2 = glm::vec3((x + x_diff), (y + y_diff), (perlin.octave2D_01((x + x_diff), (y + y_diff), octaves) * d) + start.z - d);
			glm::vec3 v3 = glm::vec3((x + x_diff), y, (perlin.octave2D_01((x + x_diff), (y), octaves) * d) + start.z - d);

			// scaling vector z
			//v0.z *= d;
			//v1.z *= d;
			//v2.z *= d;
			//v3.z *= d;

			// add vector to vector array
			meshVertices.push_back(v0);
			meshVertices.push_back(v1);
			meshVertices.push_back(v2);
			meshVertices.push_back(v0);
			meshVertices.push_back(v2);
			meshVertices.push_back(v3);

			// normalne 3 dla pierwszego trójkąta i 3 dla drugiego

			glm::vec3 n1, n2;

			glm::vec3 Diff_1 = v0 - v1;
			glm::vec3 Diff_2 = v2 - v1;

			n1.x = Diff_1.y * Diff_2.z - Diff_1.z * Diff_2.y;
			n1.y = Diff_1.z * Diff_2.x - Diff_1.x * Diff_2.z;
			n1.z = Diff_1.x * Diff_2.y - Diff_1.y * Diff_2.x;

			Diff_1 = v1 - v2;
			Diff_2 = v3 - v2;

			n2.x = Diff_1.y * Diff_2.z - Diff_1.z * Diff_2.y;
			n2.y = Diff_1.z * Diff_2.x - Diff_1.x * Diff_2.z;
			n2.z = Diff_1.x * Diff_2.y - Diff_1.y * Diff_2.x;

			meshNormals.push_back(-n1);
			meshNormals.push_back(-n1);
			meshNormals.push_back(-n1);
			meshNormals.push_back(-n2);
			meshNormals.push_back(-n2);
			meshNormals.push_back(-n2);


			meshTriangles.push_back(glm::uvec3(6 * i, 6 * i + 1, 6 * i + 2));
			meshTriangles.push_back(glm::uvec3(6 * i + 3, 6 * i + 4, 6 * i + 5));
			i++;
		}
	}
}


void updateDeltaTime(float time) {
	if (lastTime < 0) {
		lastTime = time;
		return;
	}

	deltaTime = time - lastTime;
	if (deltaTime > 0.1) deltaTime = 0.1;
	lastTime = time;
}
glm::mat4 createCameraMatrix()
{
	glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide, cameraDir));
	glm::mat4 cameraRotrationMatrix = glm::mat4({
		cameraSide.x,cameraSide.y,cameraSide.z,0,
		cameraUp.x,cameraUp.y,cameraUp.z ,0,
		-cameraDir.x,-cameraDir.y,-cameraDir.z,0,
		0.,0.,0.,1.,
		});
	cameraRotrationMatrix = glm::transpose(cameraRotrationMatrix);
	glm::mat4 cameraMatrix = cameraRotrationMatrix * glm::translate(-cameraPos);

	return cameraMatrix;
}

glm::mat4 createPerspectiveMatrix()
{

	glm::mat4 perspectiveMatrix;
	float n = 0.05;
	float f = 20.;
	float a1 = glm::min(aspectRatio, 1.f);
	float a2 = glm::min(1 / aspectRatio, 1.f);
	perspectiveMatrix = glm::mat4({
		1,0.,0.,0.,
		0.,aspectRatio,0.,0.,
		0.,0.,(f + n) / (n - f),2 * f * n / (n - f),
		0.,0.,-1.,0.,
		});


	perspectiveMatrix = glm::transpose(perspectiveMatrix);

	return perspectiveMatrix;
}

//Metoda renderująca obiekty
//context - model obiektu
//modelMatrix - umiejscowienie obiektu
//color - kolor obiektu
//roughness - szorstkość
//metallic - metaliczność
void drawObjectPBR(Core::RenderContext& context, GLuint texture, glm::mat4 modelMatrix, glm::vec3 color, float roughness, float metallic) {
	glUseProgram(program);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	
	// przesyłanie wartości do shaderów
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	glUniform1f(glGetUniformLocation(program, "exposition"), exposition);

	glUniform1f(glGetUniformLocation(program, "exposition2"), exposition2);

	glUniform1f(glGetUniformLocation(program, "roughness"), roughness);
	glUniform1f(glGetUniformLocation(program, "metallic"), metallic);

	glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);

	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUniform3f(glGetUniformLocation(program, "sunDir"), sunDir.x, sunDir.y, sunDir.z);
	glUniform3f(glGetUniformLocation(program, "sunColor"), sunColor.x, sunColor.y, sunColor.z);

	glUniform3f(glGetUniformLocation(program, "lightPos"), pointlightPos.x, pointlightPos.y, pointlightPos.z);
	glUniform3f(glGetUniformLocation(program, "lightColor"), pointlightColor.x, pointlightColor.y, pointlightColor.z);

	glUniform3f(glGetUniformLocation(program, "lightPos2"), pointlightPos2.x, pointlightPos2.y, pointlightPos2.z);
	glUniform3f(glGetUniformLocation(program, "lightColor2"), pointlightColor2.x, pointlightColor2.y, pointlightColor2.z);

	glUniform3f(glGetUniformLocation(program, "spotlightConeDir"), spotlightConeDir.x, spotlightConeDir.y, spotlightConeDir.z);
	glUniform3f(glGetUniformLocation(program, "spotlightPos"), spotlightPos.x, spotlightPos.y, spotlightPos.z);
	glUniform3f(glGetUniformLocation(program, "spotlightColor"), spotlightColor.x, spotlightColor.y, spotlightColor.z);
	glUniform1f(glGetUniformLocation(program, "spotlightPhi"), spotlightPhi);
	glUniform1f(glGetUniformLocation(program, "shadowMapWidth"), SHADOW_WIDTH);
	glUniform1f(glGetUniformLocation(program, "shadowMapHeight"), SHADOW_HEIGHT);

	// przesyłanie źródeł światła do cieni
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, depthMap);
	Core::SetActiveTexture(depthMap, "shadowMap", program, 0);
	glUniformMatrix4fv(glGetUniformLocation(program, "lightVP"), 1, GL_FALSE, (float*)&lightVP);

	//textury
	Core::SetActiveTexture(texture, "colorTexture", program, 1);
	// renderowanie obiektu
	Core::DrawContext(context);

}


glm::mat4 drawerModelPosition(glm::vec3 A, float t, bool isDrawerOpen) {
	if (isDrawerOpen == true) {
		drawerPosition = glm::translate(lerp(A, glm::vec3(0.f, 0.f, 0.4f), 1.0f));
		return drawerPosition;
	}
	else {
		drawerPosition = glm::translate(A);
		return drawerPosition;
	}
}

void drawDrawer(Core::RenderContext& context, GLuint texture, glm::mat4 modelMatrix, glm::vec3 color, float roughness, float metallic, bool isDrawerOpen) {
	glUseProgram(program);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	float time = glfwGetTime();

	glm::mat4 transformation = viewProjectionMatrix * drawerModelPosition(glm::vec3(0.0f, 0.0f, 0.0f), time, isDrawerOpen);


	// przesyłanie wartości do shaderów
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	glUniform1f(glGetUniformLocation(program, "exposition"), exposition);

	glUniform1f(glGetUniformLocation(program, "exposition2"), exposition2);

	glUniform1f(glGetUniformLocation(program, "roughness"), roughness);
	glUniform1f(glGetUniformLocation(program, "metallic"), metallic);

	glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);

	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUniform3f(glGetUniformLocation(program, "sunDir"), sunDir.x, sunDir.y, sunDir.z);
	glUniform3f(glGetUniformLocation(program, "sunColor"), sunColor.x, sunColor.y, sunColor.z);

	glUniform3f(glGetUniformLocation(program, "lightPos"), pointlightPos.x, pointlightPos.y, pointlightPos.z);
	glUniform3f(glGetUniformLocation(program, "lightColor"), pointlightColor.x, pointlightColor.y, pointlightColor.z);

	glUniform3f(glGetUniformLocation(program, "lightPos2"), pointlightPos2.x, pointlightPos2.y, pointlightPos2.z);
	glUniform3f(glGetUniformLocation(program, "lightColor2"), pointlightColor2.x, pointlightColor2.y, pointlightColor2.z);

	glUniform3f(glGetUniformLocation(program, "spotlightConeDir"), spotlightConeDir.x, spotlightConeDir.y, spotlightConeDir.z);
	glUniform3f(glGetUniformLocation(program, "spotlightPos"), spotlightPos.x, spotlightPos.y, spotlightPos.z);
	glUniform3f(glGetUniformLocation(program, "spotlightColor"), spotlightColor.x, spotlightColor.y, spotlightColor.z);
	glUniform1f(glGetUniformLocation(program, "spotlightPhi"), spotlightPhi);
	glUniform1f(glGetUniformLocation(program, "shadowMapWidth"), SHADOW_WIDTH);
	glUniform1f(glGetUniformLocation(program, "shadowMapHeight"), SHADOW_HEIGHT);

	// przesyłanie źródeł światła do cieni
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, depthMap);
	Core::SetActiveTexture(depthMap, "shadowMap", program, 0);
	glUniformMatrix4fv(glGetUniformLocation(program, "lightVP"), 1, GL_FALSE, (float*)&lightVP);

	//textury
	Core::SetActiveTexture(texture, "colorTexture", program, 1);
	// renderowanie obiektu
	Core::DrawContext(context);

}

//Metoda renderująca mesh
//context - model obiektu
//modelMatrix - umiejscowienie obiektu
//color - kolor obiektu
//roughness - szorstkość
//metallic - metaliczność
void drawMesh(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color, float roughness, float metallic) {

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;

	// przesyłanie wartości do shaderów
	glUniformMatrix4fv(glGetUniformLocation(programMesh, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(programMesh, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	glUniform1f(glGetUniformLocation(programMesh, "exposition"), exposition);

	glUniform1f(glGetUniformLocation(programMesh, "roughness"), roughness);
	glUniform1f(glGetUniformLocation(programMesh, "metallic"), metallic);

	glUniform3f(glGetUniformLocation(programMesh, "color"), color.x, color.y, color.z);

	glUniform3f(glGetUniformLocation(programMesh, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUniform3f(glGetUniformLocation(programMesh, "sunDir"), sunDir.x, sunDir.y, sunDir.z);
	glUniform3f(glGetUniformLocation(programMesh, "sunColor"), sunColor.x, sunColor.y, sunColor.z);

	glUniform3f(glGetUniformLocation(programMesh, "lightPos"), pointlightPos.x, pointlightPos.y, pointlightPos.z);
	glUniform3f(glGetUniformLocation(programMesh, "lightColor"), pointlightColor.x, pointlightColor.y, pointlightColor.z);

	glUniform3f(glGetUniformLocation(program, "lightPos2"), pointlightPos2.x, pointlightPos2.y, pointlightPos2.z);
	glUniform3f(glGetUniformLocation(program, "lightColor2"), pointlightColor2.x, pointlightColor2.y, pointlightColor2.z);

	glUniform3f(glGetUniformLocation(programMesh, "spotlightConeDir"), spotlightConeDir.x, spotlightConeDir.y, spotlightConeDir.z);
	glUniform3f(glGetUniformLocation(programMesh, "spotlightPos"), spotlightPos.x, spotlightPos.y, spotlightPos.z);
	glUniform3f(glGetUniformLocation(programMesh, "spotlightColor"), spotlightColor.x, spotlightColor.y, spotlightColor.z);
	glUniform1f(glGetUniformLocation(programMesh, "spotlightPhi"), spotlightPhi);
	glUniform1f(glGetUniformLocation(programMesh, "shadowMapWidth"), SHADOW_WIDTH);
	glUniform1f(glGetUniformLocation(programMesh, "shadowMapHeight"), SHADOW_HEIGHT);

	// przesyłanie źródeł światła do cieni
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniformMatrix4fv(glGetUniformLocation(programMesh, "lightVP"), 1, GL_FALSE, (float*)&lightVP);


	// renderowanie obiektu
	Core::DrawContext(context);

}
glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t) {
	return x * (1.f - t) + y * t;
}

float rotationAngle = 0.0;

void roombaModelRotation()
{
	glm::quat MyQuaternion;
	MyQuaternion = glm::quat(cos(glm::radians(-90.0f * deltaTime / 2)), 0, sin(glm::radians(-90.0f * deltaTime / 2)) * 1, 0);
	glm::mat4 RotationMatrix = glm::mat4_cast(MyQuaternion);
	roombaRotationMatrix *= RotationMatrix;
}

glm::mat4 roombaModelPosition(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, float t) {


	if (fmod(t, 8) < 0.0) {
		roombaPosition = glm::translate(A);
		return roombaPosition;


	}
	else if (fmod(t, 8) < 1.0 && t > 1.0) {
		roombaModelRotation();
		return roombaPosition;
	}
	else if (fmod(t, 8) < 2.0) {
		roombaPosition = glm::translate(lerp(A, B, fmod(t, 8) - 1.0));
		return roombaPosition;
	}
	else if (fmod(t, 8) < 3.0) {
		roombaModelRotation();
		return roombaPosition;
	}
	else if (fmod(t, 8) < 4.0) {
		roombaPosition = glm::translate(lerp(B, C, fmod(t, 8) - 3.0));
		return roombaPosition;
	}
	else if (fmod(t, 8) < 5.0) {
		roombaModelRotation();
		return roombaPosition;
	}
	else if (fmod(t, 8) < 6.0) {
		roombaPosition = glm::translate(lerp(C, D, fmod(t, 8) - 5.0));
		return roombaPosition;
	}
	else if (fmod(t, 8) < 7.0) {
		roombaModelRotation();
		return roombaPosition;
	}
	else {
		roombaPosition = glm::translate(lerp(D, A, fmod(t, 8) - 7.0));
		return roombaPosition;
	}
}



//metoda renderująca ruszajaca sie roombe0,
void drawRoombaPBR(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color, float roughness, float metallic) {

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	float time = glfwGetTime();
	glm::mat4 transformation = viewProjectionMatrix *
		roombaModelPosition(
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(-1.0f, 0.0f, 0.0f),
			glm::vec3(-1.0f, 0.0f, -1.0f),
			glm::vec3(0.0f, 0.0f, -1.0f),
			time
		) * roombaRotationMatrix;


	// przesyłanie wartości do shaderów
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	glUniform1f(glGetUniformLocation(program, "exposition"), exposition);

	glUniform1f(glGetUniformLocation(program, "roughness"), roughness);
	glUniform1f(glGetUniformLocation(program, "metallic"), metallic);

	glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);

	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUniform3f(glGetUniformLocation(program, "sunDir"), sunDir.x, sunDir.y, sunDir.z);
	glUniform3f(glGetUniformLocation(program, "sunColor"), sunColor.x, sunColor.y, sunColor.z);

	glUniform3f(glGetUniformLocation(program, "lightPos"), pointlightPos.x, pointlightPos.y, pointlightPos.z);
	glUniform3f(glGetUniformLocation(program, "lightColor"), pointlightColor.x, pointlightColor.y, pointlightColor.z);

	glUniform3f(glGetUniformLocation(program, "spotlightConeDir"), spotlightConeDir.x, spotlightConeDir.y, spotlightConeDir.z);
	glUniform3f(glGetUniformLocation(program, "spotlightPos"), spotlightPos.x, spotlightPos.y, spotlightPos.z);
	glUniform3f(glGetUniformLocation(program, "spotlightColor"), spotlightColor.x, spotlightColor.y, spotlightColor.z);
	glUniform1f(glGetUniformLocation(program, "spotlightPhi"), spotlightPhi);

	// przesyłanie źródeł światła do cieni
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniformMatrix4fv(glGetUniformLocation(program, "lightVP"), 1, GL_FALSE, (float*)&lightVP);

	// renderowanie obiektu
	Core::DrawContext(context);

}

// Metoda inicjalizująca depth map
void initDepthMap() {
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// rednerowanie głębokości
void drawObjectDepth(Core::RenderContext& context, glm::mat4 modelMatrix, glm::mat4 viewProjectionMatrix) {
	glUniformMatrix4fv(glGetUniformLocation(programDepth, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(programDepth, "viewProjectionMatrix"), 1, GL_FALSE, (float*)&viewProjectionMatrix);
	Core::DrawContext(context);
}

void renderShadowMap(glm::mat4 lightViewPointMatrix) {
	float time = glfwGetTime();
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

	//ustawianie przestrzeni rysowania 
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	//bindowanie FBO
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	//czyszczenie mapy głębokości 
	glClear(GL_DEPTH_BUFFER_BIT);
	//ustawianie programu depth
	glUseProgram(programDepth);

	// renderowanie przedmiotów na depth map
	drawObjectDepth(models::bedContext, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::chairContext, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::cozy1Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::cozy2Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::cozy2Drawer1Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::cozy2Drawer2Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::cozy2Drawer3Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::cozy2Drawer4Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::cremeContext, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::deskContext, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::deskDrawer1Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::deskDrawer2Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::deskDrawer3Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::deskDrawer4Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::door1Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::door2Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::floorContext, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::gamesDeskContext, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::games1Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::games2Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::games3Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::games4Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::games5Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::jarContext, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::keyboardContext, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::lampContext, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::lamp2Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::materacContext, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::monitorHolderContext, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::monitor1Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::monitor2Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::monitor3Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::mouseContext, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::paperContext, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::wallsContext, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::windowContext, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::roombaContext, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::meshContext, meshPos, lightViewPointMatrix);
	drawObjectDepth(models::books1Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::books2Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::books3Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::playstationFigureContext, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::nightCozyContext, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::nightCozyDrawer1Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::nightCozyDrawer2Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::duvetContext, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::pillow1Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::pillow2Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::pillow3Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::pillow4Context, glm::mat4(), lightViewPointMatrix);
	drawObjectDepth(models::pillow5Context, glm::mat4(), lightViewPointMatrix);

	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
}

void testDepthBuffer() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programTest);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	//Core::DrawContext(models::testContext);
}

void loadCubemap(char* textures[]) {
	glGenTextures(1, &cube_map_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map_texture);

	int w, h;
	unsigned char* data;
	for (unsigned int i = 0; i < 6; i++)
	{
		data = SOIL_load_image(textures[i], &w, &h, 0, SOIL_LOAD_RGBA);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void drawSkyBox(glm::mat4 modelMatrix) {
	glDisable(GL_DEPTH_TEST);

	glUseProgram(programSkybox);

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "transformation"), 1, GL_FALSE, (float*)&transformation);
	Core::SetActiveTexture(cube_map_texture, "skybox", programSkybox, 0);
	Core::DrawContext(skyboxContext);
	glEnable(GL_DEPTH_TEST);
}
float angle = 0;
glm::vec3 rotate = glm::vec3(0, 0, 1);
void renderScene(GLFWwindow* window)
{
	renderShadowMap(lightVP);
	glClearColor(0.4f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float time = glfwGetTime();
	updateDeltaTime(time);

	drawSkyBox(glm::translate(cameraPos));

	glUseProgram(programMesh);
	drawMesh(models::meshContext, glm::mat4(), glm::vec3(0.0f, 1.0f, 0.0f), 1.f, 0.0f);


	// program do renderowania obiektów
	glUseProgram(program);


	drawObjectPBR(models::bedContext, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::chairContext, textures::chair, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::cozy1Context, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::cozy2Context, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawDrawer(models::cozy2Drawer1Context, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f, isDrawerOpen1);
	drawDrawer(models::cozy2Drawer2Context, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f, isDrawerOpen2);
	drawDrawer(models::cozy2Drawer3Context, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f, isDrawerOpen3);
	drawDrawer(models::cozy2Drawer4Context, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f, isDrawerOpen4);
	drawObjectPBR(models::cremeContext, textures::xbox, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::deskContext, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::deskDrawer1Context, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::deskDrawer2Context, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::deskDrawer3Context, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::deskDrawer4Context, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::door1Context, textures::door, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::door2Context, textures::door, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::floorContext, textures::floor, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::gamesDeskContext, textures::pc, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::games1Context, textures::pc, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::games2Context, textures::playstation, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::games3Context, textures::xbox, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::games4Context, textures::switchT, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::games5Context, textures::pc, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::jarContext, textures::jar, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::keyboardContext, textures::black, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::lampContext, textures::pc, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::lamp2Context, textures::pc, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::materacContext, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::monitorHolderContext, textures::black, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::monitor1Context, textures::black, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::monitor2Context, textures::black, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::monitor3Context, textures::black, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::mouseContext, textures::black, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::paperContext, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::wallsContext, textures::walls, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::books1Context, textures::books, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::books2Context, textures::books, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::books3Context, textures::books, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::playstationFigureContext, textures::playstationFigure, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::nightCozyContext, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::nightCozyDrawer1Context, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::nightCozyDrawer2Context, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::duvetContext, textures::duvet, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::pillow1Context, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::pillow2Context, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::pillow3Context, textures::pillow, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::pillow4Context, textures::pillow, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::pillow5Context, textures::pillow, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawObjectPBR(models::windowContext, textures::white, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	drawRoombaPBR(models::roombaContext, glm::mat4(), glm::vec3(0.9f, 0.9f, 0.9f), 0.2f, 0.0f);
	glUseProgram(0);
	//drawObjectPBR2(models::meshContext, meshPos, glm::vec3(0.9f, .9f, 0.9f), 1.f, 0.0f);

	if(exposition2 == 0.f)
		drawObjectPBR(models::sphereContext, textures::white, glm::scale(glm::translate(glm::mat4(), glm::vec3(1.5804f, 1.385f, 1.3198f)), glm::vec3(0.03f, 0.03f, 0.03f)), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);
	else
		drawObjectPBR(models::sphereContext, textures::yellow, glm::scale(glm::translate(glm::mat4(), glm::vec3(1.5804f, 1.385f, 1.3198f)), glm::vec3(0.03f, 0.03f, 0.03f)), glm::vec3(1.f, 1.f, 1.f), 0.2f, 0.0f);


	//test depth buffer
	//testDepthBuffer();

	glUseProgram(0);
	glfwSwapBuffers(window);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	aspectRatio = width / float(height);
	glViewport(0, 0, width, height);
	WIDTH = width;
	HEIGHT = height;
}
void loadModelToContext(std::string path, Core::RenderContext& context)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	context.initFromAssimpMesh(scene->mMeshes[0]);
}


void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if( abs(xpos - mousePositionX) > abs(ypos - mousePositionY))
		if (xpos > mousePositionX)
			mouseHorizontal = 1;
		else
			mouseHorizontal = -1;
	else
		if (ypos > mousePositionY)
			mouseVertical = 1;
		else
			mouseVertical = -1;

	mousePositionX = xpos;
	mousePositionY = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == 0)
		if (action == 1)
			mouseLower = 1;
		else
			mouseLower = 0;
	else
		if (action == 1)
			mouseUpper = 1;
		else
			mouseUpper = 0;

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	mouseScroll = yoffset;
}

void init(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

	// enable testing for depth map
	glEnable(GL_DEPTH_TEST);

	program = shaderLoader.CreateProgram("shaders/worldSpaceShader.vert", "shaders/worldSpaceShader.frag");
	programTest = shaderLoader.CreateProgram("shaders/test.vert", "shaders/test.frag");
	programSun = shaderLoader.CreateProgram("shaders/shader_sun.vert", "shaders/shader_sun.frag");
	programDepth = shaderLoader.CreateProgram("shaders/shader_depth.vert", "shaders/shader_depth.frag");
	programSkybox = shaderLoader.CreateProgram("shaders/skybox.vert", "shaders/skybox.frag");
	programTex = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	programMesh = shaderLoader.CreateProgram("shaders/meshShader.vert", "shaders/meshShader.frag");

	loadModelToContext("./models/bed.obj", models::bedContext);
	loadModelToContext("./models/chair.obj", models::chairContext);
	loadModelToContext("./models/cozy1.obj", models::cozy1Context);
	loadModelToContext("./models/cozy2.obj", models::cozy2Context);
	loadModelToContext("./models/cozy2_drawer1.obj", models::cozy2Drawer1Context);
	loadModelToContext("./models/cozy2_drawer2.obj", models::cozy2Drawer2Context);
	loadModelToContext("./models/cozy2_drawer3.obj", models::cozy2Drawer3Context);
	loadModelToContext("./models/cozy2_drawer4.obj", models::cozy2Drawer4Context);
	loadModelToContext("./models/creme.obj", models::cremeContext);
	loadModelToContext("./models/desk.obj", models::deskContext);
	loadModelToContext("./models/desk_drawer1.obj", models::deskDrawer1Context);
	loadModelToContext("./models/desk_drawer2.obj", models::deskDrawer2Context);
	loadModelToContext("./models/desk_drawer3.obj", models::deskDrawer3Context);
	loadModelToContext("./models/desk_drawer4.obj", models::deskDrawer4Context);
	loadModelToContext("./models/door1.obj", models::door1Context);
	loadModelToContext("./models/door2.obj", models::door2Context);
	loadModelToContext("./models/floor.obj", models::floorContext);
	loadModelToContext("./models/games_desk.obj", models::gamesDeskContext);
	loadModelToContext("./models/games1.obj", models::games1Context);
	loadModelToContext("./models/games2.obj", models::games2Context);
	loadModelToContext("./models/games3.obj", models::games3Context);
	loadModelToContext("./models/games4.obj", models::games4Context);
	loadModelToContext("./models/games5.obj", models::games5Context);
	loadModelToContext("./models/jar.obj", models::jarContext);
	loadModelToContext("./models/keyboard.obj", models::keyboardContext);
	loadModelToContext("./models/lamp.obj", models::lampContext);
	loadModelToContext("./models/lamp2.obj", models::lamp2Context);
	loadModelToContext("./models/materac.obj", models::materacContext);
	loadModelToContext("./models/monitor_holder.obj", models::monitorHolderContext);
	loadModelToContext("./models/monitor1.obj", models::monitor1Context);
	loadModelToContext("./models/monitor2.obj", models::monitor2Context);
	loadModelToContext("./models/monitor3.obj", models::monitor3Context);
	loadModelToContext("./models/mouse.obj", models::mouseContext);
	loadModelToContext("./models/paper.obj", models::paperContext);
	loadModelToContext("./models/walls.obj", models::wallsContext);
	loadModelToContext("./models/window.obj", models::windowContext);
	loadModelToContext("./models/books1.obj", models::books1Context);
	loadModelToContext("./models/books2.obj", models::books2Context);
	loadModelToContext("./models/books3.obj", models::books3Context);
	loadModelToContext("./models/playstation_figure.obj", models::playstationFigureContext);
	loadModelToContext("./models/night_cozy.obj", models::nightCozyContext);
	loadModelToContext("./models/night_cozy_drawer1.obj", models::nightCozyDrawer1Context);
	loadModelToContext("./models/night_cozy_drawer2.obj", models::nightCozyDrawer2Context);
	loadModelToContext("./models/duvet.obj", models::duvetContext);
	loadModelToContext("./models/pillow1.obj", models::pillow1Context);
	loadModelToContext("./models/pillow2.obj", models::pillow2Context);
	loadModelToContext("./models/pillow3.obj", models::pillow3Context);
	loadModelToContext("./models/pillow4.obj", models::pillow4Context);
	loadModelToContext("./models/pillow5.obj", models::pillow5Context);
	//loadModelToContext("./models/test.obj", models::testContext);
	loadModelToContext("./models/roomba.obj", models::roombaContext);
    loadModelToContext("./models/cube.obj", skyboxContext);

	loadModelToContext("./models/sphere.obj", models::sphereContext);

	char* textures[] = {
		"./images/right.png",
		"./images/left.png",
		"./images/top.png",
		"./images/bottom.png",
		"./images/front.png",
		"./images/back.png"
	};
	loadCubemap(textures);
	//loadModelToContext("./models/test.obj", models::testContext);

	textures::floor = Core::LoadTexture("./textures/laminate_floor_02_diff_4k.jpg");
	textures::door = Core::LoadTexture("./textures/drzwi.png");
	textures::walls = Core::LoadTexture("./textures/beige_wall_001_diff_4k.jpg");
	textures::materac = Core::LoadTexture("./textures/fabric_pattern_07_col_1_4k.png");
	textures::chair = Core::LoadTexture("./textures/chair.png");
	textures::pc = Core::LoadTexture("./textures/pc.png");
	textures::xbox = Core::LoadTexture("./textures/xbox.png");
	textures::playstation = Core::LoadTexture("./textures/playstation.png");
	textures::switchT = Core::LoadTexture("./textures/switch.png");
	textures::jar = Core::LoadTexture("./textures/sloik.png");
	textures::white = Core::LoadTexture("./textures/white.png");
	textures::black = Core::LoadTexture("./textures/black.png");
	textures::yellow = Core::LoadTexture("./textures/yellow.png");
	textures::books = Core::LoadTexture("./textures/books.png");
	textures::playstationFigure = Core::LoadTexture("./textures/playstation_figure.png");
	textures::duvet = Core::LoadTexture("./textures/duvet2.png");
	textures::pillow = Core::LoadTexture("./textures/piloow.png");

	// intialize depth map
	initDepthMap();

	createTerrainMesh();
	models::meshContext.initFromArray(meshVertices, meshTriangles, meshNormals);
}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(program);
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
	yCameraDir = cameraDir.y;
	glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 cameraUp = glm::vec3(0.f, 1.f, 0.f);
	float angleSpeed = 0.05f * deltaTime * 60;
	float moveSpeed = 0.05f * deltaTime * 80;
	int scrollSpeed = 4;
	float horizontalSpeed = 1.5;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraDir * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraDir * moveSpeed;

	switch (mouseScroll) {
		case -1:
			cameraPos -= cameraDir * moveSpeed * scrollSpeed;
			break;
		case 0:
			break;
		case 1:
			cameraPos += cameraDir * moveSpeed * scrollSpeed;
		break;
	}

	switch (mouseHorizontal) {
	case -1:
		cameraDir = glm::vec3(glm::eulerAngleY(angleSpeed * horizontalSpeed) * glm::vec4(cameraDir, 0));
		break;
	case 0:
		break;
	case 1:
		cameraDir = glm::vec3(glm::eulerAngleY(-angleSpeed * horizontalSpeed) * glm::vec4(cameraDir, 0));
		break;
	}


	switch (mouseVertical) {
	case -1:
		if (yCameraDir < 0.8)
			cameraDir = cameraDir + glm::vec3(0, 0.04, 0);
		break;
	case 0:
		break;
	case 1:
		if (yCameraDir > -0.8)
			cameraDir = cameraDir - glm::vec3(0, 0.04, 0);
		break;

	}

	if(mouseUpper == true)
		cameraPos += cameraUp * moveSpeed;

	if (mouseLower == true)
		cameraPos -= cameraUp * moveSpeed;

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		cameraPos += cameraSide * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		cameraPos -= cameraSide * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos += cameraUp * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos -= cameraUp * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraDir = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(cameraDir, 0));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraDir = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(cameraDir, 0));

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		exposition -= 0.5;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		exposition += 0.5;
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		exposition2 = 0.f;
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
		exposition2 = 10.f;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		isDrawerOpen1 = true;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		isDrawerOpen1 = false;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		isDrawerOpen2 = true;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		isDrawerOpen2 = false;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		isDrawerOpen3 = true;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		isDrawerOpen3 = false;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		isDrawerOpen4 = true;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		isDrawerOpen4 = false;


	mouseScroll = 0;
	mouseHorizontal = 0;
	mouseVertical = 0;
}

// funkcja jest glowna petla
void renderLoop(GLFWwindow* window) {
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		renderScene(window);
		glfwPollEvents();
	}
}
//}