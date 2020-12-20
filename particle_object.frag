// Adaped from the particle animation tutorial from:
// http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/

#version 400 

// Interpolated values from the vertex shaders
in vec2 UV;
in vec4 particlecolor;

// Ouput data
out vec4 color;

uniform sampler2D myTextureSampler;

void main(){
	// Output color = color of the texture at the specified UV
	color = texture2D( myTextureSampler, UV ) * vec4(0, 0, 0.2, 1);

}