#version 450

in vec3 s_vPosition;
in vec3 s_vNormal;
in vec4 s_vColor;
in vec4 s_lvColor;


uniform mat4 MVP;   // MVP Matrix
uniform mat4 MV;    // Model View Matrix
uniform mat4 M;     // Model Matrix (World Coordinates)
uniform mat4 V;

uniform vec3 light1direction;
uniform vec3 light2direction;
uniform vec3 light3direction;
uniform vec3 eyePosition;

out vec3 n;
out vec3 v;
out vec3 e;

out vec3 ld1;
out vec3 ld2;
out vec3 ld3;

out vec4 objectColor;
out vec4 lightedColor;


void main() {

	//pass on to fragment shader
	ld1 = (MV * vec4(light1direction, 1.0f)).xyz;
	ld1 = normalize(ld1);

	ld2 = (MV * vec4(light2direction, 1.0f)).xyz;
	ld2 = normalize(ld2);

	ld3 = (MV * vec4(light3direction, 1.0f)).xyz;
	ld3 = normalize(ld3);

	v = (MV * vec4(s_vPosition, 1.0f)).xyz;
	
	n = mat3(transpose(inverse(MV))) * s_vNormal;
	n = s_vNormal;

	e = v - vec3(0.0f, 0.0f, 0.0f);
	e = normalize(e);

	objectColor = s_vColor;
	lightedColor = s_lvColor;

	//set vertex position for display in openGL
	gl_Position = MVP * vec4(s_vPosition, 1.0f);
}