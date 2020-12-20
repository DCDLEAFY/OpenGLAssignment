// Vertex shader with Gouraud shading lighting (lighting calculated per vertex)
// Designed to texture an object with lighting
// No colour data as colour data assumed to be taked from texture

#version 400

// These are the vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

// Uniform variables are passed in from the application
uniform mat4 model, view, projection;
uniform uint colourmode;

vec4 lightpos = vec4(2, 2, 2, 1);

//Opacity Value
//uniform uint opacitymode; //------------------------------------

// Output the vertex colour - to be rasterized into pixel fragments
out vec4 fcolour;


// Output a texture coordinate as a vertex attribute
out vec2 ftexcoord;
out vec3 fV, flightdir, fnormal;
out float fattenuation;

void main()
{
	vec4 specular_colour = vec4(1.0,1.0,0,1.0);
	vec4 diffuse_colour = vec4(1.0,1.0,0,1.0);
	vec4 position_h = vec4(position, 1.0);
	float shininess = 8.0;
	
	

	vec4 ambient = diffuse_colour;
	fcolour = vec4(1);

	mat4 modelview = view * model;

	vec4 P = modelview * position_h;
	vec3 L = lightpos.xyz - P.xyz;

	mat3 normal_transformation = transpose(inverse(mat3(modelview)));

	vec3 N = normal_transformation * normal;

	fnormal = normalize(N);
	flightdir = normalize(L);
	fV = normalize(-P.xyz);

	float attenuation;
	float distance_to_light = length(L);

	float k1 = 0.1;
	float k2 = 0.1;
	float k3 = 0.1;

	attenuation = 1.0/(k1 + k2*distance_to_light + k3*pow(distance_to_light, 2));
	fattenuation = 40*attenuation;
	
	ftexcoord = texcoord;

	gl_Position = projection * view * model * position_h;

	// Define the vertex colour
	//fcolour = vec4(diffuse, 1.0) + ambient + specular;
	//fcolour.a = 0.5f;

	// Define the vertex position
	//gl_Position = projection * view * model * position_h;

	// Output the texture coordinates with no modifications
	
	//ftexcoord = texcoord;
}

