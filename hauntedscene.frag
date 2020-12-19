// Basic fragment shader to demosntrate combining texturing with 
// vertex lighting
// Iain Martin 2018

#version 400

in vec4 fcolour;
in vec2 ftexcoord;
out vec4 outputColor;

in vec3 funitdir, flightdir, fnormal;


uniform sampler2D tex1;

void main()
{
	vec3 global_ambient = vec3(0.5, 0.5, 0.5);
	vec4 spec_colour = vec4(0.5,0.5,1.0,1.0);
	float shiny_val = 10.0f;


	vec4 ambient = fcolour * 0.2;

	vec3 reflected_light_beam = reflect(-flightdir, fnormal);

	vec3 specular = pow(max(dot(reflected_light_beam, funitdir), 0.0), shiny_val) * vec3(1,1,1);

	vec4 spec4 = vec4(specular, 1.0);

	float diffuse_component = max(dot(fnormal, flightdir), 0);

	vec4 diffuse_lighting = diffuse_component * fcolour;


	vec4 texcolour = texture(tex1, ftexcoord);
	outputColor = fcolour * texcolour;

	//outputColor = (spec4) + vec4(global_ambient,1) + texcolour;

	outputColor = texcolour + spec4;
}