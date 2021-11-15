#include <windows.h> 
#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <GL/glew.h> 
#include <GL/freeglut.h> 

#include "loadShaders.h"

#include "glm/glm/glm.hpp"  
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtx/transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "SOIL.h"

#include <vector>
using namespace std;

//////////////////////////////////////

GLuint
VaoId,
VboId,
EboId,
ColorBufferId,
TextureId,
ProgramId,
myMatrixLocation,
texture;

const int PlayerVCount = 4;
const int N = 360;
const int R = 50;
const float PI = 3.141516;
glm::mat4 myMatrix, resizeMatrix, scaleMatrix, translMatrix, rotateMatrix, 
	translMoonMatrix1, translMoonMatrix2, rotateMoonMatrix,
	translPlayerMatrix, rotatePlayerMatrix, rotateVerticalPlayerMatrix, launchPlayerMatrix;

glm::mat4 view, projection;

float Obsx = 0.0f, Obsy = 0.0f, Obsz = 800.f;
float Refx = 0.0f, Refy = 0.0f;
float width = 1200, height = 600, fov = 90, znear = 1, zfar = 1000;

bool PLAYER_LAUNCHED = false;
bool PLAYER_SHOULD_INIT_MATRIX = false;

float planetAngle = 0.0;
float moonAngle = 0.0;
float planetScaleRaport[] = { 0.25, 0.5, 0.55, 0.3, 0.75, 0.70, 0.60, 0.60 };


vector<GLfloat> getCirclePoints(float r, int numberOfPoints)
{
	vector<GLfloat> circlePoints;
	GLfloat x;
	GLfloat y;
	for (int i = 0; i < numberOfPoints; i++)
	{
		x = (GLfloat)r * cos((2 * i * PI) / numberOfPoints);
		y = (GLfloat)r * sin((2 * i * PI) / numberOfPoints);
		circlePoints.insert(circlePoints.end(), { x, y, 0.0f, 1.0f });
	}
	return circlePoints;
}

void debugMatrix(glm::mat4 matrix)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			cout << matrix[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}

void CreateVBO(void)
{
	static vector<GLfloat> Vertices = {
		-25.f, -25.0f, 0.0f, 1.0f,
		 25.f, -25.0f, 0.0f, 1.0f,
		 25.f, 25.0f, 0.0f, 1.0f,
		-25.f, 25.0f, 0.0f, 1.0f,
	};
	
	vector<GLfloat> circleVertices = getCirclePoints(R, N);
	Vertices.insert(Vertices.end(), circleVertices.begin(), circleVertices.end());
	

	static const GLfloat Colors[] =
	{
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
	};

	static const GLfloat Textures[] =
	{
		0.0f, 0.0f, // st jos 
		1.0f, 0.0f, // dr jos
		1.0f, 1.0f, // dr sus
		0.0f, 1.0f, // st sus
	};

	glGenBuffers(1, &VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(GLfloat), Vertices.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &ColorBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &TextureId);
	glBindBuffer(GL_ARRAY_BUFFER, TextureId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Textures), Textures, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void DestroyVBO(void)
{
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &TextureId);
	glDeleteBuffers(1, &ColorBufferId);
	glDeleteBuffers(1, &VboId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}

void LoadTexture(void)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	int width, height;
	unsigned char* image = SOIL_load_image("rocket-2.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CreateShaders(void)
{
	ProgramId = LoadShaders("SolarSystemShader.vert", "SolarSystemShader.frag");
	glUseProgram(ProgramId);
}

void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

void Initialize(void)
{
	glClearColor(0.0f, 0.0f, 0.8f, 0.0f);
	CreateShaders();

	LoadTexture();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	Obsx = 0;
	Obsy = 0;
	Obsz = 200.f;

	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	resizeMatrix = glm::mat4(1.0f);
}

void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}

void setMyMatrix(glm::mat4 matrix)
{
	myMatrix = matrix;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
}

void setTexCode(int cod) 
{
	glUniform1i(glGetUniformLocation(ProgramId, "tex_code"), cod);
}


void rotatePlanetsAroundSun(int n)
{
	moonAngle = moonAngle +0.35;
	planetAngle = planetAngle + 0.1;
	glutPostRedisplay();
	glutTimerFunc(50, rotatePlanetsAroundSun, 0);
}

void mouse(int button, int state, int x, int y)
{
	switch (button) {
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
			glutTimerFunc(100, rotatePlanetsAroundSun, 0);
		break;
	default:
		break;
	}
}

void processNormalKeys(unsigned char key, int x, int y) {

	switch (key) {
	case ' ':
		if (!PLAYER_LAUNCHED) {
			PLAYER_LAUNCHED = true;
			PLAYER_SHOULD_INIT_MATRIX = true;
		}
		break;
	default:
		break;
	}
}

void processSpecialKeys(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_LEFT:
		Obsx -= 10;
		launchPlayerMatrix = glm::translate(launchPlayerMatrix, glm::vec3(-10, 0, 0));
		break;
	case GLUT_KEY_RIGHT:
		Obsx += 10;
		launchPlayerMatrix = glm::translate(launchPlayerMatrix, glm::vec3(10, 0, 0));
		break;
	case GLUT_KEY_UP:
		Obsy += 10;
		launchPlayerMatrix = glm::translate(launchPlayerMatrix, glm::vec3(0, 10, 0));
		break;
	case GLUT_KEY_DOWN:
		Obsy -= 10;
		launchPlayerMatrix = glm::translate(launchPlayerMatrix, glm::vec3(0, -10, 0));
		break;
	default:
		break;
	}
}

void drawOrbit(int p) {
	scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.f * p, 2.f * p, 1.0));
	setMyMatrix(scaleMatrix);
	glDrawArrays(GL_LINE_LOOP, PlayerVCount, N);
}

void drawMoon() {
	rotateMoonMatrix = glm::rotate(glm::mat4(1.0f), moonAngle, glm::vec3(0.0, 0.0, 1.0));
	scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.1, 0.1, 1.0));
	translMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(7 * R, 0.0, 0.0));
	translMoonMatrix1 = glm::translate(glm::mat4(1.0f), glm::vec3(-6 * R, 0.0, 0.0));
	translMoonMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(6* R, 0.0, 0.0));

	setMyMatrix(rotateMatrix * translMoonMatrix2 * rotateMoonMatrix * translMoonMatrix1 * translMatrix * scaleMatrix);
	glDrawArrays(GL_TRIANGLE_FAN, PlayerVCount, N);
}

void drawPlanet(int p, float scaleRaport) {
	scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleRaport, scaleRaport, 1.0));
	translMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(2 * p * R, 0.0, 0.0));
	setMyMatrix(rotateMatrix * translMatrix * scaleMatrix);
	glDrawArrays(GL_TRIANGLE_FAN, PlayerVCount, N);
	if (p == 3) {
		if (!PLAYER_LAUNCHED) {
			translPlayerMatrix = translMatrix;
			Obsx = (rotateMatrix * translMatrix)[3][0];
			Obsy = (rotateMatrix * translMatrix)[3][1];
		}
		drawMoon();
	}
}

void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	CreateVBO();

	// se schimba pozitia observatorului
	glm::vec3 Obs = glm::vec3(Obsx, Obsy, Obsz);

	// pozitia punctului de referinta
	Refx = Obsx; Refy = Obsy;
	glm::vec3 PctRef = glm::vec3(Refx, Refy, -1.0f);

	// verticala din planul de vizualizare 
	glm::vec3 Vert = glm::vec3(0.0f, 1.0f, 0.0f);
	
	
	view = glm::lookAt(Obs, PctRef, Vert);
	glUniformMatrix4fv(glGetUniformLocation(ProgramId, "view"), 1, GL_FALSE, &view[0][0]);
	
	projection = glm::perspective(fov, GLfloat(width) / GLfloat(height), znear, zfar);
	glUniformMatrix4fv(glGetUniformLocation(ProgramId, "projection"), 1, GL_FALSE, &projection[0][0]);

	setTexCode(0);
	setMyMatrix(resizeMatrix);
	glDrawArrays(GL_TRIANGLE_FAN, PlayerVCount, N);

	for (int p = 1; p <= 8; p++) {
		drawOrbit(p);
		rotateMatrix = glm::rotate(glm::mat4(1.0f), planetAngle/p, glm::vec3(0.0, 0.0, 1.0));
		if (p == 3 && PLAYER_SHOULD_INIT_MATRIX) {
			rotateVerticalPlayerMatrix = glm::rotate(glm::mat4(1.0f), -planetAngle / p, glm::vec3(0.0, 0.0, 1.0));
			rotatePlayerMatrix = rotateMatrix;
		}
		drawPlanet(p, planetScaleRaport[p - 1]);
	}

	if (PLAYER_LAUNCHED)
	{
		if (PLAYER_SHOULD_INIT_MATRIX) {
			launchPlayerMatrix = rotatePlayerMatrix * translPlayerMatrix * rotateVerticalPlayerMatrix;
			PLAYER_SHOULD_INIT_MATRIX = false;
		}
		setMyMatrix(launchPlayerMatrix);
		setTexCode(1);
		glDrawArrays(GL_TRIANGLE_FAN, 0, PlayerVCount);
	}
	
	glFlush();
}


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("Solar System");
	glewInit();
	Initialize();
	glutDisplayFunc(RenderFunction);
	glutMouseFunc(mouse);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutCloseFunc(Cleanup);
	glutMainLoop();
}

