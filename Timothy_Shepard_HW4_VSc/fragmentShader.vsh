#version 450

in vec3 n;
in vec3 v;

in vec3 ld1;
in vec3 ld2;
in vec3 ld3;

in vec4 objectColor;

uniform vec3 light1color;
uniform vec3 light2color;
uniform vec3 light3color;

out vec4 fColor;

void main () {

	//get shorter names for light colors
	vec3 lc1 = light1color; // (1.0f, 0.1f, 0.1f)
	vec3 lc2 = light2color; // (0.1f, 1.0f, 0.1f)
	vec3 lc3 = light3color; // (0.1f, 0.1f, 1.0f)

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

	vec3 spec1 = lc1 * pow( max( dot( r1, e ), 0.0 ), s );
	vec3 spec2 = lc2 * pow( max( dot( r2, e ), 0.0 ), s );
	vec3 spec3 = lc3 * pow( max( dot( r3, e ), 0.0 ), s );

	vec3 specular = Ks * (spec1 + spec2 + spec3);
	//vec3 specular = Ks * spec3;

	//Diffuse Color
	float Kd = 0.3f;

	vec3 diff1 = lc1 * max( dot( n, ld1 ), 0.0);
	vec3 diff2 = lc2 * max( dot( n, ld2 ), 0.0);
	vec3 diff3 = lc3 * max( dot( n, ld3 ), 0.0);

	vec3 diffuse = Kd * (diff1 + diff2 + diff3);
	//vec3 diffuse = Kd * diff1;

	//Ambient Color
	float Ka = 0.1f;

	vec3 amb1 = lc1;
	vec3 amb2 = lc2;
	vec3 amb3 = lc3;

	vec3 ambient = Ka * (amb1 + amb2 + amb3);
	//vec3 ambient = Ka * amb3;

	//calculate output color
	vec4 c = vec4( (specular + diffuse + ambient), 1.0f);
	vec4 outputColor = c * objectColor;

	//output color
	//fColor = vec4(n, 1.0f);
	fColor = outputColor;
}