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

#include <vector>
using namespace std;

//////////////////////////////////////

GLuint
VaoId,
VboId,
EboId,
ColorBufferId,
ProgramId,
myMatrixLocation;

const int PlayerVCount = 4;
const int N = 360;
const int R = 50;
const float PI = 3.141516;
glm::mat4 myMatrix, resizeMatrix, scaleMatrix, translMatrix, rotateMatrix, 
translPlayerMatrix, rotatePlayerMatrix, rotateVerticalPlayerMatrix;

float i = 0.0;
float angle = 0.0;

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
}

void DestroyVBO(void)
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &ColorBufferId);
	glDeleteBuffers(1, &VboId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
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

	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	resizeMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.f / 600, 1.f / 300, 1.0));
	
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


void rotatePlanetsAroundSun(int n)
{
	angle = angle + 0.1;
	glutPostRedisplay();
	glutTimerFunc(100, rotatePlanetsAroundSun, 0);
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
		cout << "Space" << endl;
		break;
	default:
		break;
	}
}

void processSpecialKeys(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_LEFT:
		cout << "Stanga" << endl;
		break;
	case GLUT_KEY_RIGHT:
		cout << "Dreapta" << endl;
		break;
	case GLUT_KEY_UP:
		cout << "Sus" << endl;
		break;
	case GLUT_KEY_DOWN:
		cout << "Jos" << endl;
		break;
	default:
		break;
	}
}



void drawOrbit(int p) {
	scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.f * p, 2.f * p, 1.0));
	setMyMatrix(resizeMatrix * scaleMatrix);
	glDrawArrays(GL_LINE_LOOP, PlayerVCount, N);
}

void drawPlanet(int p, float scaleRaport) {
	scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleRaport, scaleRaport, 1.0));
	translMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(2 * p * R, 0.0, 0.0));
	if (p == 3) {
		translPlayerMatrix = translMatrix;
	}
	setMyMatrix(resizeMatrix * rotateMatrix * translMatrix * scaleMatrix);
	glDrawArrays(GL_TRIANGLE_FAN, PlayerVCount, N);
}

void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	CreateVBO();
	

	setMyMatrix(resizeMatrix);
	glDrawArrays(GL_TRIANGLE_FAN, PlayerVCount, N);

	
	
	for (int p = 1; p <= 8; p++) {
		drawOrbit(p);
		rotateMatrix = glm::rotate(glm::mat4(1.0f), angle/p, glm::vec3(0.0, 0.0, 1.0));
		if (p == 3) {
			rotateVerticalPlayerMatrix = glm::rotate(glm::mat4(1.0f), -angle / p, glm::vec3(0.0, 0.0, 1.0));
			rotatePlayerMatrix = rotateMatrix;
		}
		drawPlanet(p, planetScaleRaport[p - 1]);
	}

	setMyMatrix(resizeMatrix * rotatePlayerMatrix * translPlayerMatrix * rotateVerticalPlayerMatrix);
	glDrawArrays(GL_TRIANGLE_FAN, 0, PlayerVCount);

	glFlush();
}



int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1200, 600);
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

