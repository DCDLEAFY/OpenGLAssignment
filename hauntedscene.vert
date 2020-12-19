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

//Opacity Value
//uniform uint opacitymode; //------------------------------------

// Output the vertex colour - to be rasterized into pixel fragments
out vec4 fcolour;
vec3 light_dir = vec3(0.0, 0.0, 10.0);

// Output a texture coordinate as a vertex attribute
out vec2 ftexcoord;
out vec3 funitdir, flightdir, fnormal;

void main()
{
	vec4 specular_colour = vec4(1.0,1.0,1.0,1.0);
	vec4 diffuse_colour = vec4(1.0,1.0,0,1.0);
	vec4 position_h = vec4(position, 1.0);
	float shininess = 8.0;
	
	vec4 ambient = diffuse_colour * 0.8;

	mat4 mv_matrix = view * model;
	mat3 normalmatrix = mat3(mv_matrix);
	vec3 N = mat3(mv_matrix) * normal;
	N = normalize(N);
	light_dir = normalize(light_dir);

	vec3 diffuse = max(dot(N, light_dir), 0.0) * diffuse_colour.xyz;

	vec4 P = position_h * mv_matrix;
	vec3 half_vec = normalize(light_dir + P.xyz);
	vec4 specular = pow(max(dot(N, half_vec), 0.0), shininess) * specular_colour;

	//___________________________


	mat4 modelview = view * model;
	mat3 normal_matrix = transpose(inverse(mat3(modelview)));
	vec3 normal_vector = normal_matrix*normal;
	vec3 light_dir_normalised = normalize(light_dir);
	vec3 normal_vector_normalised = normalize(normal_vector);

	float shiny_val = 0.8f;

	vec3 specular_col = vec3(1.0, 1.0, 1.0);
	
	vec4 positioned_transformed = modelview * position_h;
	vec3 light_point_vert = light_dir.xyz - positioned_transformed.xyz;

	vec3 unit_length_dir = normalize(-positioned_transformed.xyz);

	fnormal = normal_vector_normalised;
	flightdir = normalize(light_point_vert);
	funitdir = normalize(-positioned_transformed.xyz);



	// Define the vertex colour
	fcolour = vec4(diffuse, 1.0) + ambient + specular;
	//fcolour.a = 0.5f;

	// Define the vertex position
	gl_Position = projection * view * model * position_h;

	// Output the texture coordinates with no modifications
	
	ftexcoord = texcoord;
}

