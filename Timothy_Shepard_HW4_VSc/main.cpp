// 
//  Timothy Shepard
//  HW4
//  Teapot Lighting
//

#include <Windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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
GLuint vno;
GLuint positionID, colorID, normalID, lcID;
GLuint indexBufferID;
GLuint positionBuffer, colorBuffer, normalBuffer, lcbuffer;

struct Vertex {
	GLfloat x, y, z;
	GLfloat nx, ny, nz;
	GLfloat r, g, b, a;
};

glm::vec3 light1direction = glm::vec3(0.0f, -1.0f, 0.0f);
glm::vec3 light1color = glm::vec3(1.0f, 0.1f, 0.1f);
glm::vec3 light2direction = glm::vec3(-1.0f, 0.0f, 0.0f);
glm::vec3 light2color = glm::vec3(0.1f, 1.0f, 0.1f);
glm::vec3 light3direction = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 light3color = glm::vec3(0.1f, 0.1f, 1.0f);

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
	allVertices = new Vertex[numTriangles * 3];  // numVertices is numTriangles*3

	int currentIndex = 0;
	while (!inputFile.eof())
	{
		string currentLine;
		getline(inputFile, currentLine);
		if (currentLine == "") { continue; }
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
		if (i % 3 == 0) { std::cout << "Triangle " << ++triangleCount << endl; }
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
//// Test making lighting here in glm

glm::vec4* buildLightedColorsVec4s(Vertex* allVertices, int numVertices, glm::mat4 M)
{
	glm::vec4* lightedColors;
	lightedColors = new glm::vec4[numVertices];

	/*
	std::cout << "Making lighted colors in glm\n";
	std::cout << "Using matrix transform:\n";
	std::cout << M[0][0] << " " << M[1][0] << " " << M[2][0] << " " << M[3][0] << endl;
	std::cout << M[0][1] << " " << M[1][1] << " " << M[2][1] << " " << M[3][1] << endl;
	std::cout << M[0][2] << " " << M[1][2] << " " << M[2][2] << " " << M[3][2] << endl;
	std::cout << M[0][3] << " " << M[1][3] << " " << M[2][3] << " " << M[3][3] << endl;
	*/

	glm::vec3 ld1 = light1direction;
	glm::vec3 ld2 = light2direction;
	glm::vec3 ld3 = light3direction;
	glm::vec3 lc1 = light1color;
	glm::vec3 lc2 = light2color;
	glm::vec3 lc3 = light3color;
	glm::vec3 ld1T = glm::vec3(M * glm::vec4(ld1, 1.0f));
	glm::vec3 ld2T = glm::vec3(M * glm::vec4(ld2, 1.0f));
	glm::vec3 ld3T = glm::vec3(M * glm::vec4(ld3, 1.0f));
	glm::vec3 ld1Tn = glm::normalize(ld1T);
	glm::vec3 ld2Tn = glm::normalize(ld2T);
	glm::vec3 ld3Tn = glm::normalize(ld3T);

	GLfloat Ka = 0.1f;
	GLfloat Kd = 0.3f;
	GLfloat Ks = 0.6f;
	GLfloat s = 3.0f;


	for (int i = 0; i < numVertices; i++)
	{
		Vertex vertex;
		vertex = allVertices[i];
		glm::vec3 v = glm::vec3(vertex.x, vertex.y, vertex.z);
		glm::vec4 objectColor = glm::vec4(vertex.r, vertex.g, vertex.b, vertex.a);
		glm::vec3 n = glm::vec3(vertex.nx, vertex.ny, vertex.nz);


		glm::vec3 vT = glm::vec3(M * glm::vec4(v, 1.0f));
		glm::vec3 nT = glm::vec3(glm::transpose(glm::inverse(M)) * glm::vec4(n, 1.0f));

		glm::vec3 e = glm::vec3(0.0f, 0.0f, 0.0f) - vT;

		glm::vec3 r1 = glm::reflect(-ld1T, nT);
		glm::vec3 r2 = glm::reflect(-ld2T, nT);
		glm::vec3 r3 = glm::reflect(-ld3T, nT);

		glm::vec3 nTn = glm::normalize(nT);
		glm::vec3 en = glm::normalize(e);
		glm::vec3 r1n = glm::normalize(r1);
		glm::vec3 r2n = glm::normalize(r2);
		glm::vec3 r3n = glm::normalize(r3);

		//specular
		glm::vec3 spec1 = lc1 * glm::pow(glm::max(glm::dot(r1n, en), 0.0f), s);
		glm::vec3 spec2 = lc2 * glm::pow(glm::max(glm::dot(r2n, en), 0.0f), s);
		glm::vec3 spec3 = lc3 * glm::pow(glm::max(glm::dot(r3n, en), 0.0f), s);
		glm::vec3 specular = Ks * (spec1 + spec2 + spec3);

		//diffuse
		glm::vec3 diff1 = lc1 * glm::max(glm::dot(nTn, ld1Tn), 0.0f);
		glm::vec3 diff2 = lc2 * glm::max(glm::dot(nTn, ld2Tn), 0.0f);
		glm::vec3 diff3 = lc3 * glm::max(glm::dot(nTn, ld3Tn), 0.0f);
		glm::vec3 diffuse = Kd * (diff1 + diff2 + diff3);

		//ambient
		glm::vec3 amb1 = lc1;
		glm::vec3 amb2 = lc2;
		glm::vec3 amb3 = lc3;
		glm::vec3 ambient = Ka * (amb1 + amb2 + amb3);

		glm::vec3 color = specular + diffuse + ambient;
		glm::vec4 outputColor = objectColor * glm::vec4(color, 1.0f);

		/*
		if (i % 1000 == 0) {
			std::cout << "----------------------------------------------------------------------\n";
			std::cout << endl << "v = " << v[0] << ", " << v[1] << ", " << v[2] << endl;
			std::cout << "vT = " << vT[0] << ", " << vT[1] << ", " << vT[2] << endl;
			std::cout << "n = " << n[0] << ", " << n[1] << ", " << n[2] << endl;
			std::cout << "nT = " << nT[0] << ", " << nT[1] << ", " << nT[2] << endl;
			std::cout << "nTn = " << nTn[0] << ", " << nTn[1] << ", " << nTn[2] << endl << endl;
			std::cout << "ld1 = " << ld1[0] << ", " << ld1[1] << ", " << ld1[2] << endl;
			std::cout << "ld1T = " << ld1T[0] << ", " << ld1T[1] << ", " << ld1T[2] << endl;
			std::cout << "ld1Tn = " << ld1Tn[0] << ", " << ld1Tn[1] << ", " << ld1Tn[2] << endl;
			std::cout << "ld2 = " << ld2[0] << ", " << ld2[1] << ", " << ld2[2] << endl;
			std::cout << "ld2T = " << ld2T[0] << ", " << ld2T[1] << ", " << ld2T[2] << endl;
			std::cout << "ld2Tn = " << ld2Tn[0] << ", " << ld2Tn[1] << ", " << ld2Tn[2] << endl;
			std::cout << "ld3 = " << ld3[0] << ", " << ld3[1] << ", " << ld3[2] << endl;
			std::cout << "ld3T = " << ld3T[0] << ", " << ld3T[1] << ", " << ld3T[2] << endl;
			std::cout << "ld3Tn = " << ld3Tn[0] << ", " << ld3Tn[1] << ", " << ld3Tn[2] << endl << endl;
			//std::cout << "eyePosition = " << eyePosition[0] << ", " << eyePosition[1] << ", " << eyePosition[2] << endl;
			std::cout << "e = " << e[0] << ", " << e[1] << ", " << e[2] << endl;
			std::cout << "en = " << en[0] << ", " << en[1] << ", " << en[2] << endl << endl;
			std::cout << "r1 = " << r1[0] << ", " << r1[1] << ", " << r1[2] << endl;
			std::cout << "r1n = " << r1n[0] << ", " << r1n[1] << ", " << r1n[2] << endl;
			std::cout << "r2 = " << r2[0] << ", " << r2[1] << ", " << r2[2] << endl;
			std::cout << "r2n = " << r2n[0] << ", " << r2n[1] << ", " << r2n[2] << endl;
			std::cout << "r3 = " << r3[0] << ", " << r3[1] << ", " << r3[2] << endl;
			std::cout << "r3n = " << r3n[0] << ", " << r3n[1] << ", " << r3n[2] << endl << endl;
			std::cout << "lc1 = " << lc1[0] << ", " << lc1[1] << ", " << lc1[2] << endl;
			std::cout << "lc2 = " << lc2[0] << ", " << lc2[1] << ", " << lc2[2] << endl;
			std::cout << "lc3 = " << lc3[0] << ", " << lc3[1] << ", " << lc3[2] << endl << endl;
			std::cout << "Ks = " << Ks << " , s = " << s << endl;
			std::cout << "spec1 = " << spec1[0] << ", " << spec1[1] << ", " << spec1[2] << endl;
			std::cout << "spec2 = " << spec2[0] << ", " << spec2[1] << ", " << spec2[2] << endl;
			std::cout << "spec3 = " << spec3[0] << ", " << spec3[1] << ", " << spec3[2] << endl;
			std::cout << "specular = " << specular[0] << ", " << specular[1] << ", " << specular[2] << endl << endl;
			std::cout << "Kd = " << Kd << endl;
			std::cout << "diff1 = " << diff1[0] << ", " << diff1[1] << ", " << diff1[2] << endl;
			std::cout << "diff2 = " << diff2[0] << ", " << diff2[1] << ", " << diff2[2] << endl;
			std::cout << "diff3 = " << diff3[0] << ", " << diff3[1] << ", " << diff3[2] << endl;
			std::cout << "diffuse = " << diffuse[0] << ", " << diffuse[1] << ", " << diffuse[2] << endl << endl;
			std::cout << "Ka = " << Ka << endl;
			std::cout << "amb1 = " << amb1[0] << ", " << amb1[1] << ", " << amb1[2] << endl;
			std::cout << "amb2 = " << amb2[0] << ", " << amb2[1] << ", " << amb2[2] << endl;
			std::cout << "amb3 = " << amb3[0] << ", " << amb3[1] << ", " << amb3[2] << endl;
			std::cout << "ambient = " << ambient[0] << ", " << ambient[1] << ", " << ambient[2] << endl << endl;
			std::cout << "color = " << color[0] << ", " << color[1] << ", " << color[2] << endl;
			std::cout << "objectColor = " << objectColor[0] << ", " << objectColor[1] << ", " << objectColor[2] << ", " << objectColor[3] << endl;
			std::cout << "outputColor = " << outputColor[0] << ", " << outputColor[1] << ", " << outputColor[2] << ", " << outputColor[3] << endl;
		}
		*/

		lightedColors[i] = outputColor;
	}
	return lightedColors;
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
	GLuint MVPID = glGetUniformLocation(shaderProgramID, "MVP");
	GLuint MVID = glGetUniformLocation(shaderProgramID, "MV");


	glm::mat4 M = glm::mat4(1.0f);
	glm::mat4 P = glm::mat4(1.0f);
	glm::mat4 V = glm::mat4(1.0f);
	glm::mat4 MV = V*M;
	glm::mat4 MVP = P*V*M;

	switch (key) {
	case '1':
		//#1 Model Transformation, Orthographic Projection, Camera Transformation
		M = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		P = glm::ortho(-2.4f, 2.4f, -1.8f, 1.8f, 1.0f, 50.0f);
		V = glm::lookAt(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		MV = V*M;
		MVP = P*V*M;
		break;

	case '2':
		//#2 Model Transformation, Perspective Projection, Camera Transformation
		M = glm::rotate(M, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		M = glm::translate(M, glm::vec3(0.0f, -1.0f, 0.0f));
		P = glm::perspective(glm::radians(50.0f), 4.0f / 3.0f, 1.0f, 50.0f);
		V = glm::lookAt(glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		MV = V*M;
		MVP = P*V*M;
		break;

	case '3':
		//#3 Model Transformation, Orthographic Projection, Camera Transformation, Same as #1
		M = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		P = glm::ortho(-2.4f, 2.4f, -1.8f, 1.8f, 1.0f, 50.0f);
		V = glm::lookAt(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		MV = V*M;
		MVP = P*V*M;
		break;

	case '4':
		//#4 Model Transformation, Perspective Projection, Camera Transformation, Same as #2
		M = glm::rotate(M, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		M = glm::translate(M, glm::vec3(0.0f, -1.0f, 0.0f));
		P = glm::perspective(glm::radians(50.0f), 4.0f / 3.0f, 1.0f, 50.0f);
		V = glm::lookAt(glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		MV = V*M;
		MVP = P*V*M;
		break;

	default:
		//Default copy number 1
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		break;
	}

	glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(MVID, 1, GL_FALSE, &MV[0][0]);
	glutPostRedisplay(); //request display() call ASAP
}

//struct Light {
//	glm::vec3 direction;
//	glm::vec3 color;
//};

int main(int argc, char** argv) {

	// Get vertices from Tris.txt file
	string filename = "Tris1.txt";
	int numTriangles = getNumTriangles(filename);

	Vertex* allVertices;
	allVertices = getAllVertices(filename);
	int numVertices = numTriangles * 3;
	//printAllVertices(allVertices, numTriangles);

	glm::vec3* vpositions;
	vpositions = buildPositionsVec3s(allVertices, numVertices);
	//printAllPositions(vpositions, numVertices);

	glm::vec3* vnormals;
	vnormals = buildNormalsVec3s(allVertices, numVertices);
	//printAllNormals(vnormals, numVertices);

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
	glutCreateWindow("Timothy_Shepard_HW4.zip");
	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);
	glewInit();  //glewInit() for Windows only
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;
	std::cout << "GL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n\n";

	// Make a shader
	char* vertexShaderSourceCode = readFile("vertexShader.vsh");
	char* fragmentShaderSourceCode = readFile("fragmentShader.vsh");
	GLuint vertShaderID = makeVertexShader(vertexShaderSourceCode);
	GLuint fragShaderID = makeFragmentShader(fragmentShaderSourceCode);
	shaderProgramID = makeShaderProgram(vertShaderID, fragShaderID);

	// Start by showing view #1
	glm::mat4 M = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	glm::mat4 P = glm::ortho(-2.4f, 2.4f, -1.8f, 1.8f, 1.0f, 50.0f);
	glm::mat4 V = glm::lookAt(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 MV = V*M;
	glm::mat4 MVP = P*V*M;

	//test lighting in glm
	glm::vec4* lightedColors;
	lightedColors = new glm::vec4[numVertices];
	lightedColors = buildLightedColorsVec4s(allVertices, numVertices, MV);


	// Change for APPLE version
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//glGenVertexArraysAPPLE(1, &vao);
	//glBindVertexArrayAPPLE(vao);

	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, NUM_VERTICES * sizeof(glm::vec3), vpositions, GL_STATIC_DRAW);
	
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, NUM_VERTICES * sizeof(glm::vec4), vcolors, GL_STATIC_DRAW);
	
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, NUM_VERTICES * sizeof(glm::vec3), vnormals, GL_STATIC_DRAW);

	glGenBuffers(1, &lcbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, lcbuffer);
	glBufferData(GL_ARRAY_BUFFER, NUM_VERTICES * sizeof(glm::vec4), lightedColors, GL_STATIC_DRAW);

	positionID = glGetAttribLocation(shaderProgramID, "s_vPosition");
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	colorID = glGetAttribLocation(shaderProgramID, "s_vColor");
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0);

	normalID = glGetAttribLocation(shaderProgramID, "s_vNormal");
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer(normalID, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	lcID = glGetAttribLocation(shaderProgramID, "s_lvColor");
	glBindBuffer(GL_ARRAY_BUFFER, lcbuffer);
	glVertexAttribPointer(lcID, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0);

	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_INDICES * sizeof(GLuint), vindices, GL_STATIC_DRAW); // Put indices in buffer as Gluint

	//use shader program
	glUseProgram(shaderProgramID);



	GLuint MVPID = glGetUniformLocation(shaderProgramID, "MVP");
	glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);

	GLuint MVID = glGetUniformLocation(shaderProgramID, "MV");
	glUniformMatrix4fv(MVID, 1, GL_FALSE, &MV[0][0]);

	//Directional Lights
	glm::vec3 light1direction = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 light1color = glm::vec3(1.0f, 0.1f, 0.1f);
	GLuint light1d = glGetUniformLocation(shaderProgramID, "light1direction");
	glUniform3fv(light1d, 1, glm::value_ptr(light1direction));
	GLuint light1c = glGetUniformLocation(shaderProgramID, "light1color");
	glUniform3fv(light1c, 1, glm::value_ptr(light1color));

	glm::vec3 light2direction = glm::vec3(-1.0f, 0.0f, 0.0f);
	glm::vec3 light2color = glm::vec3(0.1f, 1.0f, 0.1f);
	GLuint light2d = glGetUniformLocation(shaderProgramID, "light2direction");
	glUniform3fv(light2d, 1, glm::value_ptr(light2direction));
	GLuint light2c = glGetUniformLocation(shaderProgramID, "light2color");
	glUniform3fv(light2c, 1, glm::value_ptr(light2color));

	glm::vec3 light3direction = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 light3color = glm::vec3(0.1f, 0.1f, 1.0f);
	GLuint light3d = glGetUniformLocation(shaderProgramID, "light3direction");
	glUniform3fv(light3d, 1, glm::value_ptr(light3direction));
	GLuint light3c = glGetUniformLocation(shaderProgramID, "light3color");
	glUniform3fv(light3c, 1, glm::value_ptr(light3color));

	glutKeyboardFunc(switchMVP);

	glEnableVertexAttribArray(positionID);
	glEnableVertexAttribArray(colorID);
	glEnableVertexAttribArray(normalID);
	glEnableVertexAttribArray(lcID);

	glutMainLoop();

	delete[] allVertices;
	delete[] vindices;
	delete[] vpositions;
	delete[] vnormals;
	delete[] vcolors;
	delete[] lightedColors;

	return 0;
}

//glGenBuffers(1, &vbo);
//glBindBuffer(GL_ARRAY_BUFFER, vbo);
//GLsizeiptr bufferSize = NUM_VERTICES * sizeof(glm::vec3) + NUM_VERTICES * sizeof(glm::vec4);
//glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_STATIC_DRAW); //Create buffer
//glBufferSubData(GL_ARRAY_BUFFER, 0, NUM_VERTICES * sizeof(glm::vec3), vpositions);  // Put data in buffer
//glBufferSubData(GL_ARRAY_BUFFER, NUM_VERTICES * sizeof(glm::vec3), NUM_VERTICES * sizeof(glm::vec4), vcolors);

//// Find the position of the variables in the shader
//positionID = glGetAttribLocation(shaderProgramID, "s_vPosition");
//colorID = glGetAttribLocation(shaderProgramID, "s_vColor");

//glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
//glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), BUFFER_OFFSET(NUM_VERTICES * sizeof(glm::vec3)));

//glGenBuffers(1, &vno);
//glBindBuffer(GL_NORMAL_ARRAY_BUFFER_BINDING, vno);
//glBufferData(GL_ARRAY_BUFFER, NUM_VERTICES * sizeof(glm::vec3), vnormals, GL_STATIC_DRAW);
//normalID = glGetAttribLocation(shaderProgramID, "s_vNormal");