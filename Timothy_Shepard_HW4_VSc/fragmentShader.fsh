#version 450

in vec4 color;
out vec4 fColor;

//struct Light {
//   vec3 position;
//   vec3 color;
//}

uniform vec3 light1direction;
uniform vec3 light1color;
uniform vec3 light2direction;
uniform vec3 light2color;
uniform vec3 light3direction;
uniform vec3 light3color;

void main () {
	fColor = color;
}