#version 450

in vec4 s_vPosition;
in vec4 s_vNormal;
in vec4 s_vColor;

uniform mat4 MVP;   // MVP Matrix
uniform mat4 MV;    // Model View Matrix
uniform vec4 light1direction;

out vec4 objectColor;
out vec4 normalMV;
out vec4 vertexMV; 
out vec4 light1MV;

void main() {

	//pass on to fragment shader
	normalMV = MV * s_vNormal;
	objectColor = s_vColor;
	vertexMV = MV * s_vPosition;
	light1MV = MV * light1direction;


	//set vertex position for display in openGL
	gl_Position = MVP * s_vPosition;
}