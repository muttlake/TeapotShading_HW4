#version 450

in vec3 s_vPosition;
in vec4 s_vColor;
in vec3 s_vNormal;

uniform mat4 MVP;   // MVP Matrix
uniform mat4 MV;    // Model View Matrix

uniform vec3 light1direction;
uniform vec3 light2direction;
uniform vec3 light3direction;

uniform vec3 light1color;
uniform vec3 light2color;
uniform vec3 light3color;

uniform int s_vShadingType;

out vec3 n;
out vec3 v;

out vec3 ld1;
out vec3 ld2;
out vec3 ld3;

out vec3 lc1;
out vec3 lc2;
out vec3 lc3;

out vec4 objectColor;
out vec4 gouraudColor;

out int shadingType;


void main() {

	shadingType = s_vShadingType;

	//For both phong and gouraud shading

	//but pass on to fragment shader for phong shading
	n = (transpose(inverse(MV))* vec4(s_vNormal, 0.0f)).xyz;
	n = normalize(n);

	ld1 = -1 * (MV * vec4(light1direction, 0.0f)).xyz;
	ld1 = normalize(ld1);

	ld2 = -1 * (MV * vec4(light2direction, 0.0f)).xyz;
	ld2 = normalize(ld2);

	ld3 = -1 * (MV * vec4(light3direction, 0.0f)).xyz;
	ld3 = normalize(ld3);

	v = (MV * vec4(s_vPosition, 1.0f)).xyz;

	objectColor = s_vColor;
	gouraudColor = objectColor;

	lc1 = light1color;
	lc2 = light2color;
	lc3 = light3color;

	if (shadingType == 1) //gouraud shading
	{
		//eye vector
		vec3 e = vec3(0, 0, 0) - v;
		e = normalize(e);

		//light reflection vectors
		vec3 r1 = reflect(-ld1, n);
		r1 = normalize(r1);
		vec3 r2 = reflect(-ld2, n);
		r2 = normalize(r2);
		vec3 r3 = reflect(-ld3, n);
		r3 = normalize(r3);

		//Specular Color
		float Ks = 0.6f;
		float s = 3.0f; //specular power

		vec3 spec1 = lc1 * pow(max(dot(r1, e), 0.0), s);
		vec3 spec2 = lc2 * pow(max(dot(r2, e), 0.0), s);
		vec3 spec3 = lc3 * pow(max(dot(r3, e), 0.0), s);

		vec3 specular = Ks * (spec1 + spec2 + spec3);

		//Diffuse Color
		float Kd = 0.3f;

		vec3 diff1 = lc1 * max(dot(n, ld1), 0.0);
		vec3 diff2 = lc2 * max(dot(n, ld2), 0.0);
		vec3 diff3 = lc3 * max(dot(n, ld3), 0.0);

		vec3 diffuse = Kd * (diff1 + diff2 + diff3);

		//Ambient Color
		float Ka = 0.1f;

		vec3 amb1 = lc1;
		vec3 amb2 = lc2;
		vec3 amb3 = lc3;

		vec3 ambient = Ka * (amb1 + amb2 + amb3);

		//calculate output color
		vec4 c = vec4((specular + diffuse + ambient), 1.0f);
		gouraudColor = c * objectColor;
	}

	//set vertex position for display in openGL
	gl_Position = MVP * vec4(s_vPosition, 1.0f);
}