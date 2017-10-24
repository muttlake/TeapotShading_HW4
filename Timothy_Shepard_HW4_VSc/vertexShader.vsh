#version 450

in vec3 s_vPosition;
in vec4 s_vColor;
in vec3 s_vNormal;

uniform mat4 MVP;   // MVP Matrix
uniform mat4 MV;    // Model View Matrix

uniform vec3 light1direction;
uniform vec3 light2direction;
uniform vec3 light3direction;

out vec3 n;
out vec3 v;

out vec3 ld1;
out vec3 ld2;
out vec3 ld3;

out vec4 objectColor;


void main() {

	//pass on to fragment shader
	n = (MV * vec4(s_vNormal, 0.0f)).xyz;
	n = normalize(n);
	//n = s_vPosition;

	ld1 = -1 * (MV * vec4(light1direction, 0.0f)).xyz;
	ld1 = normalize(ld1);

	ld2 = -1 * (MV * vec4(light2direction, 0.0f)).xyz;
	ld2 = normalize(ld2);

	ld3 = -1 * (MV * vec4(light3direction, 0.0f)).xyz;
	ld3 = normalize(ld3);

	v = (MV * vec4(s_vPosition, 0.0f)).xyz;

	objectColor = s_vColor;

	//set vertex position for display in openGL
	gl_Position = MVP * vec4(s_vPosition, 1.0f);
}