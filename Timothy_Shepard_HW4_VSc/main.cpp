// 
//  Timothy Shepard
//  HW3
//  Teapot Transformations
//

#include <Windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

const int NUM_VERTICES = 2688;
const int NUM_INDICES = 2688;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
GLuint shaderProgramID;
GLuint vao = 0;
GLuint vbo;
GLuint positionID, colorID;
GLuint indexBufferID;

struct Vertex {
	GLfloat x, y, z;
	GLfloat nx, ny, nz;
	GLfloat r, g, b, a;
};

///////////////////////////////////////////////////////////////
// Read and Compile Shaders from tutorial
///////////////////////////////////////////////////////////////
static char* readFile(const char* filename) {
	FILE* fp = fopen(filename, "r");
	fseek(fp, 0, SEEK_END);
	long file_length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* contents = new char[file_length + 1];
	for (int i = 0; i < file_length + 1; i++) {
		contents[i] = 0;
	}
	fread(contents, 1, file_length, fp);
	contents[file_length + 1] = '\0';  // end of string in C
	fclose(fp);
	return contents;
}

bool compiledStatus(GLint shaderID) {
	GLint compiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
	if (compiled) {
		return true;
	}
	else {
		GLint logLength;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
		char* msgBuffer = new char[logLength];
		glGetShaderInfoLog(shaderID, logLength, NULL, msgBuffer);
		printf("%s\n", msgBuffer);
		delete (msgBuffer);
		return false;
	}
}

void printShaderSource(const char* shaderSource, string shaderName) {
	printf("--------------------------------------------------------------\n");
	printf("Printing %s shader:\n", shaderName.c_str());
	printf(shaderSource);
	printf("\n");
}

GLuint makeVertexShader(const char* shaderSource) {
	printShaderSource(shaderSource, "vertex");
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, (const GLchar**)&shaderSource, NULL);
	glCompileShader(vertexShaderID);
	bool compiledCorrectly = compiledStatus(vertexShaderID);
	if (compiledCorrectly) {
		return vertexShaderID;
	}
	return -1;
}

GLuint makeFragmentShader(const char* shaderSource) {
	printShaderSource(shaderSource, "fragment");
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, (const GLchar**)&shaderSource, NULL);
	glCompileShader(fragmentShaderID);
	bool compiledCorrectly = compiledStatus(fragmentShaderID);
	if (compiledCorrectly) {
		return fragmentShaderID;
	}
	return -1;
}

GLuint makeShaderProgram(GLuint vertexShaderID, GLuint fragmentShaderID) {
	GLuint shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShaderID);
	glAttachShader(shaderID, fragmentShaderID);
	glLinkProgram(shaderID);
	return shaderID;
}

///////////////////////////////////////////////////////////////
// Read File for Tris.txt
///////////////////////////////////////////////////////////////
int getNumTriangles(string filename) 
{
	ifstream inputFile;
	inputFile.open(filename);
	int numTriangles = 0;
	inputFile >> numTriangles;
	inputFile.close();
	return numTriangles;
}

Vertex* getAllVertices(string filename) 
{
	ifstream inputFile;
	inputFile.open(filename);

	int numTriangles = 0;
	inputFile >> numTriangles;

	Vertex* allVertices;
	allVertices = new Vertex[numTriangles*3];  // numVertices is numTriangles*3

	int currentIndex = 0;
	while (!inputFile.eof())
	{
		string currentLine;
		getline(inputFile, currentLine);
		if (currentLine == "") { continue;}
		stringstream ss;
		ss << currentLine;
		Vertex newVertex;
		ss >> newVertex.x >> newVertex.y >> newVertex.z;
		ss >> newVertex.nx >> newVertex.ny >> newVertex.nz;
		ss >> newVertex.r >> newVertex.g >> newVertex.b >> newVertex.a;
		allVertices[currentIndex] = newVertex;
		currentIndex++;
	}

	inputFile.close();
	return allVertices;
}

void printAllVertices(Vertex* allVertices, int numTriangles)
{
	int triangleCount = 0;
	std::cout << "Printing all vertices.\n";
	for (int i = 0; i < numTriangles * 3; i++)
	{
		if (i % 3 == 0) {std::cout << "Triangle " << ++triangleCount << endl;}
		Vertex v;
		v = allVertices[i];
		std::cout << "Vertex: " << i << " ";
		std::cout << v.x << " " << v.y << " " << v.z << " ";
		std::cout << v.nx << " " << v.ny << " " << v.nz << " ";
		std::cout << v.r << " " << v.b << " " << v.g << " " << v.a << endl;
	}
}

glm::vec3* buildPositionsVec3s(Vertex* allVertices, int numVertices)
{
	glm::vec3* allPositions;
	allPositions = new glm::vec3[numVertices];
	for (int i = 0; i < numVertices; i++)
	{
		Vertex v;
		v = allVertices[i];
		allPositions[i] = glm::vec3(v.x, v.y, v.z);
	}
	return allPositions;
}

void printAllPositions(glm::vec3* allPositions, int numVertices)
{
	std::cout << "Printing all position vec3:\n";
	for (int i = 0; i < numVertices; i++)
	{
		if (i % 3 == 0)
		{
			std::cout << "Triangle: " << int(i / 3 + 0.5) << "\n";
		}
		std::cout << "Position For Vertex " << i << ": ";
		std::cout << allPositions[i][0] << " " << allPositions[i][1];
		std::cout << " " << allPositions[i][2] << endl;
	}
}

glm::vec3* buildNormalsVec3s(Vertex* allVertices, int numVertices)
{
	glm::vec3* allNormals;
	allNormals = new glm::vec3[numVertices];
	for (int i = 0; i < numVertices; i++)
	{
		Vertex v;
		v = allVertices[i];
		allNormals[i] = glm::vec3(v.nx, v.ny, v.nz);
	}
	return allNormals;
}

void printAllNormals(glm::vec3* allNormals, int numVertices)
{
	std::cout << "Printing all normals vec3:\n";
	for (int i = 0; i < numVertices; i++)
	{
		if (i % 3 == 0)
		{
			std::cout << "Triangle: " << int(i / 3 + 0.5) << "\n";
		}
		std::cout << "Normal For Vertex " << i << ": ";
		std::cout << allNormals[i][0] << " " << allNormals[i][1];
		std::cout << " " << allNormals[i][2] << endl;
	}
}

void printAllColors(glm::vec4* allColors, int numVertices)
{
	std::cout << "Printing all color vec4:\n";
	for (int i = 0; i < numVertices; i++)
	{
		if (i % 3 == 0)
		{
			std::cout << "Triangle: " << int(i / 3 + 0.5) << "\n";
		}
		std::cout << "Color For Vertex " << i << ": ";
		std::cout << allColors[i][0] << " " << allColors[i][1] << " ";
		std::cout << allColors[i][2] << " " << allColors[i][3] << endl;
	}
}

glm::vec4* buildColorsVec4s(Vertex* allVertices, int numVertices)
{
	glm::vec4* allColors;
	allColors = new glm::vec4[numVertices];
	for (int i = 0; i < numVertices; i++)
	{
		Vertex v;
		v = allVertices[i];
		allColors[i] = glm::vec4(v.r, v.g, v.b, v.a);
	}
	return allColors;
}

GLuint* getTriangleIndicesArray(int numVertices) 
{
	GLuint* triangleIndicesList;
	triangleIndicesList = new GLuint[numVertices];
	for (int i = 0; i < numVertices; i++)
	{
		triangleIndicesList[i] = GLuint(i);
	}
	return triangleIndicesList;
}

void printTriangleIndices(GLuint* triangleIndices, int numVertices)
{
	std::cout << "Printing all indices.\n";
	for (int i = 0; i < numVertices; i++)
	{
		std::cout << triangleIndices[i] << "\n";
	}
}

///////////////////////////////////////////////////////////////
// Main Part of program
///////////////////////////////////////////////////////////////

// changeViewport
void changeViewport(int w, int h) {
	glViewport(0, 0, w, h);
}

// render
void render() {
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glDrawElements(GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, NULL);
	glDrawArrays(GL_TRIANGLES, 0, NUM_INDICES);
	glutSwapBuffers();
}

void switchMVP(unsigned char key, int xmouse, int ymouse)
{
	GLuint MatrixID = glGetUniformLocation(shaderProgramID, "MVP");

	glm::mat4 M = glm::mat4(1.0f);
	glm::mat4 P = glm::mat4(1.0f);
	glm::mat4 V = glm::mat4(1.0f);
	glm::mat4 MVP = P*V*M;

	switch (key) {
	case '1':
		//#1 Orthographic Projection, Camera Transformation
		P = glm::ortho(-8.0f, 8.0f, -6.0f, 6.0f, 1.0f, 100.0f);
		V = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		MVP = P*V*M;
		break;

	case '2':
		//#2 Orthographic Projection, Camera Transformation
		P = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 1.0f, 100.0f);
		V = glm::lookAt(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 2.0f));
		MVP = P*V*M;
		break;

	case '3':
		//New #3 Model Transformation, Orthographic Projection, Camera Transformation
		M = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		P = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 1.0f, 100.0f);
		V = glm::lookAt(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		MVP = P*V*M;
		break;

	case '4':
		//#4 Model Transformation, Perspective Projection, Camera Transformation
		M = glm::translate(M, glm::vec3(0.0f, -1.0f, 0.0f));
		P = glm::perspective(glm::radians(20.0f), 4.0f / 3.0f, 1.0f, 100.0f);
		V = glm::lookAt(glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f));
		MVP = P*V*M;
		break;

	case '5':
		//#5 Model Transformation, Perspective Projection, Camera Transformation
		M = glm::translate(M, glm::vec3(0.0f, -1.0f, 0.0f));
		P = glm::perspective(glm::radians(30.0f), 4.0f / 3.0f, 1.0f, 100.0f);
		V = glm::lookAt(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		MVP = P*V*M;
		break;

	case '6':
		//#6 Model Transformation, Perspective Projection, Camera Transformation
		M = glm::rotate(M, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		M = glm::translate(M, glm::vec3(0.0f, -1.0f, 0.0f));
		P = glm::perspective(glm::radians(70.0f), 4.0f / 3.0f, 1.0f, 100.0f);
		V = glm::lookAt(glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		MVP = P*V*M;
		break;

	default:
		//Default copy number 1
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		break;
	}

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glutPostRedisplay(); //request display() call ASAP
}

int main(int argc, char** argv) {

	// Get vertices from Tris.txt file
	string filename = "Tris1.txt";
	int numTriangles = getNumTriangles(filename);
	Vertex* allVertices;
	allVertices = getAllVertices(filename);
	int numVertices = numTriangles * 3;
	printAllVertices(allVertices, numTriangles);
	glm::vec3* vpositions;
	vpositions = buildPositionsVec3s(allVertices, numVertices);
	//printAllPositions(vpositions, numVertices);
	glm::vec3* vnormals;
	vnormals = buildNormalsVec3s(allVertices, numVertices);
	printAllNormals(vnormals, numVertices);
	glm::vec4* vcolors;
	vcolors = buildColorsVec4s(allVertices, numVertices);
	//printAllColors(vcolors, numVertices);

	int numIndices;
	numIndices = numVertices;
	GLuint* vindices;
	vindices = getTriangleIndicesArray(numVertices);
	//printTriangleIndices(vindices, numVertices);


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(700, 700);
	glutCreateWindow("Timothy_Shepard_HW3.zip");
	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);
	glewInit();  //glewInit() for Windows only
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;
	std::cout << "GL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n\n";

	// Make a shader
	char* vertexShaderSourceCode = readFile("vertexShader.vsh");
	char* fragmentShaderSourceCode = readFile("fragmentShader.fsh");
	GLuint vertShaderID = makeVertexShader(vertexShaderSourceCode);
	GLuint fragShaderID = makeFragmentShader(fragmentShaderSourceCode);
	shaderProgramID = makeShaderProgram(vertShaderID, fragShaderID);

	// Change for APPLE version
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//glGenVertexArraysAPPLE(1, &vao);
	//glBindVertexArrayAPPLE(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	GLsizeiptr bufferSize = NUM_VERTICES * sizeof(glm::vec3) + NUM_VERTICES * sizeof(glm::vec4);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_STATIC_DRAW); //Create buffer
	glBufferSubData(GL_ARRAY_BUFFER, 0, NUM_VERTICES * sizeof(glm::vec3), vpositions);  // Put data in buffer
	glBufferSubData(GL_ARRAY_BUFFER, NUM_VERTICES * sizeof(glm::vec3), NUM_VERTICES * sizeof(glm::vec4), vcolors);

	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_INDICES * sizeof(GLuint), vindices, GL_STATIC_DRAW); // Put indices in buffer as Gluint

	// Find the position of the variables in the shader
    positionID = glGetAttribLocation(shaderProgramID, "s_vPosition");
	colorID = glGetAttribLocation(shaderProgramID, "s_vColor");

	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), BUFFER_OFFSET(NUM_VERTICES * sizeof(glm::vec3)));
	glUseProgram(shaderProgramID);

	// Start by showing view #1
	glm::mat4 M = glm::mat4(1.0f);
	glm::mat4 P = glm::ortho(-8.0f, 8.0f, -6.0f, 6.0f, 1.0f, 100.0f);
	glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 MVP = P*V*M;

	GLuint MatrixID = glGetUniformLocation(shaderProgramID, "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	glutKeyboardFunc(switchMVP);

	glEnableVertexAttribArray(positionID);
	glEnableVertexAttribArray(colorID);

	glutMainLoop();

	delete[] allVertices;
	delete[] vindices;
	delete[] vpositions;
	delete[] vnormals;
	delete[] vcolors;

	return 0;
}