#version 450

in vec4 s_vPosition;
in vec4 s_vNormal;

//uniform mat4 mM;  // Model Matrix or Global Coordinate System Matrix
//uniform mat4 mV;  // Camera Matrix
//uniform mat4 mP;  // Projection Matrix

uniform mat4 MVP;   // MVP Matrix

in vec4 s_vColor;
out vec4 color;

void main() {
	color = s_vColor;
	gl_Position = MVP * s_vPosition;
}