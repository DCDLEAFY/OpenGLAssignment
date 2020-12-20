// Basic fragment shader to demosntrate combining texturing with 
// vertex lighting
// Iain Martin 2018

#version 400

in vec4 fcolour;
in vec2 ftexcoord;
out vec4 outputColor;

in vec3 fV, flightdir, fnormal;
in float fattenuation;


uniform sampler2D tex1;

void main()
{
	vec3 emissive = vec3(0);
	float shininess = 8.0;
	vec4 specular_colour = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 global_ambient = vec3(0.1, 0.07, 0.05);

	vec4 diffuse_colour = vec4(0.5, 0.5, 0.5, 1.0);

	float NL = max(dot(fnormal, flightdir), 0);
	vec4 diffuse_lighting = NL * diffuse_colour;

	vec3 R = reflect(-flightdir, fnormal);
	vec3 specular_lighting = pow(max(dot(R, fV), 0.0), shininess) * specular_colour.xyz;

	vec4 texcolour = texture(tex1, ftexcoord);
	vec4 shadedcolour = texcolour*(fcolour + diffuse_lighting) + vec4(specular_lighting, 1.0);

	//outputColor = (fattenuation)*(shadedcolour) + vec4(emissive, 1.0) + vec4(global_ambient, 1.0);
	outputColor = fcolour * texcolour;

	outputColor = fattenuation*shadedcolour + vec4(global_ambient, 1);
}