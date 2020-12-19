/* terrain_object.cpp (enhanced)
   Includes functions to get floating point grid position from world coordinates
   and get the height from floating point grid position.
   Also includes sea level and height-based colours.

   vec2 terrain_object::getGridPos(GLfloat x, GLfloat z)
   float terrain_object::heightAtPosition(GLfloat x, GLfloat z)

   Iain Martin November 2020
*/

#include "terrain_object.h"
#include <glm/gtc/noise.hpp>
#include "glm/gtc/random.hpp"
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace glm;

/* Define the vertex attributes for vertex positions and normals. 
   Make these match your application and vertex shader
   You might also want to add texture coordinates */
terrain_object::terrain_object(int octaves, GLfloat freq, GLfloat scale)
{
	attribute_v_coord = 0;
	attribute_v_colour = 2;
	attribute_v_normal = 1;
	xsize = 0;	// Set to zero because we haven't created the heightfield array yet
	zsize = 0;	
	perlin_octaves = octaves;
	perlin_freq = freq;
	perlin_scale = scale;
	height_scale = 1.f;
}


terrain_object::~terrain_object()
{
	/* tidy up */
	if (vertices) delete[] vertices;
	if (normals) delete[] normals;
	if (colours) delete[] colours;
}


/* Copy the vertices, normals and element indices into vertex buffers */
void terrain_object::createObject()
{
	/* Generate the vertex buffer object */
	glGenBuffers(1, &vbo_mesh_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_vertices);
	glBufferData(GL_ARRAY_BUFFER, xsize * zsize  * sizeof(vec3), &(vertices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Store the colours in a buffer object */
	glGenBuffers(1, &vbo_mesh_colours);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_colours);
	glBufferData(GL_ARRAY_BUFFER, xsize * zsize * sizeof(vec3), &(colours[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Store the normals in a buffer object */
	glGenBuffers(1, &vbo_mesh_normals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_normals);
	glBufferData(GL_ARRAY_BUFFER, xsize * zsize * sizeof(vec3), &(normals[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Generate a buffer for the indices
	glGenBuffers(1, &ibo_mesh_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_mesh_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size()* sizeof(GLuint), &(elements[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

/* Enable vertex attributes and draw object
Could improve efficiency by moving the vertex attribute pointer functions to the
create object but this method is more general 
*/
void terrain_object::drawObject(int drawmode)
{
	int size;	// Used to get the byte size of the element (vertex index) array

	// Describe our vertices array to OpenGL (it can't guess its format automatically)
	glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_vertices);
	glVertexAttribPointer(
		attribute_v_coord,  // attribute index
		3,                  // number of elements per vertex, here (x,y,z)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                  // no extra data between each position
		0                   // offset of first element
		);
	glEnableVertexAttribArray(attribute_v_coord);

	// Describe our colours array to OpenGL (it can't guess its format automatically)
	glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_colours);
	glVertexAttribPointer(
		attribute_v_colour,  // attribute index
		3,                  // number of elements per vertex, here (x,y,z)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                  // no extra data between each position
		0                   // offset of first element
		);
	glEnableVertexAttribArray(attribute_v_colour);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_normals);
	glVertexAttribPointer(
		attribute_v_normal, // attribute
		3,                  // number of elements per vertex, here (x,y,z)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                  // no extra data between each position
		0                   // offset of first element
		);
	glEnableVertexAttribArray(attribute_v_normal);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_mesh_elements); 
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	// Enable this line to show model in wireframe
	if (drawmode == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	/* Draw the triangle strips */
	for (GLuint i = 0; i < xsize - 1; i++)
	{
		GLuint location = sizeof(GLuint) * (i * zsize * 2);
		glDrawElements(GL_TRIANGLE_STRIP, zsize * 2, GL_UNSIGNED_INT, (GLvoid*)(location));
	}
}


/* Define the terrian heights */
/* Uses code adapted from OpenGL Shading Language Cookbook: Chapter 8 */
void terrain_object::calculateNoise()
{
	/* Create the array to store the noise values */
	/* The size is the number of vertices * number of octaves */
	noise = new GLfloat[xsize * zsize * perlin_octaves];
	for (int i = 0; i < (xsize*zsize*perlin_octaves); i++) noise[i] = 0;

	GLfloat xfactor = 1.f / (xsize - 1);
	GLfloat zfactor = 1.f / (zsize - 1);
	GLfloat freq = perlin_freq;
	GLfloat scale = perlin_scale;

	for (GLuint row = 0; row < zsize; row++)
	{
		for (GLuint col = 0; col < xsize; col++)
		{
			GLfloat x = xfactor * col;
			GLfloat z = zfactor * row;
			GLfloat sum = 0;
			GLfloat curent_scale = scale;
			GLfloat current_freq = freq;

			// Compute the sum for each octave
			for (GLuint oct = 0; oct < perlin_octaves; oct++)
			{
				vec2 p(x*current_freq, z*current_freq);
				GLfloat val = perlin(p) / curent_scale;
				sum += val;
				GLfloat result = (sum + 1.f) / 2.f;

				// Store the noise value in our noise array
				noise[(row * xsize + col) * perlin_octaves + oct] = result;
				
				// Move to the next frequency and scale
				current_freq *= 2.f;
				curent_scale *= scale;
			}
			
		}
	}
}

/* Define the vertex array that specifies the terrain
   (xp, zp) specifies the pixel dimensions of the heightfield (x * y) vertices
   (xs, ys) specifies the size of the heightfield region in world coords
   */
void terrain_object::createTerrain(GLuint xp, GLuint zp, GLfloat xs, GLfloat zs, GLfloat sealevel)
{
	xsize = xp;
	zsize = zp;
	width = xs;
	height = zs;

	/* Scale heights in relation to the terrain size */
	height_scale = xs*4.f;

	/* Create array of vertices */
	GLuint numvertices = xsize * zsize;
	vertices = new vec3[numvertices];
	normals  = new vec3[numvertices];
	colours = new vec3[numvertices];

	/* First calculate the noise array which we'll use for our vertex height values */
	calculateNoise();

	/* Define starting (x,z) positions and the step changes */
	GLfloat xpos = -width / 2.f;
	GLfloat xpos_step = width / GLfloat(xp);
	GLfloat zpos_step = height / GLfloat(zp);
	GLfloat zpos_start = -height / 2.f;

	/* Define the vertex positions for a flat surface */
	/* Set the normals to zero */
	/* Note, for a flat surface, set the normals to (0, 1, 0) but we don't do that because
	   that will affect the true normal calculation in the next step */
	for (GLuint row = 0; row < zsize; row++)
	{
		GLfloat zpos = zpos_start;
		for (GLuint col = 0; col < zsize; col++)
		{
			GLfloat height = noise[(row * xsize + col) * perlin_octaves + perlin_octaves - 1];
			vertices[row * xsize + col] = vec3(xpos, (height - 0.5f) * height_scale, zpos);

			// Zero the normal, it gets calculated at the end of this method after all the vertex positions
			// have been set.
			normals[row * xsize + col] = vec3(0, 0.0f, 0);
			zpos += zpos_step;
		}
		xpos += xpos_step;
	}


	/* Define vertices for triangle strips */
	for (GLuint x = 0; x < xsize - 1; x++)
	{
		GLuint top    = x * zsize;
		GLuint bottom = top + zsize;
		for (GLuint z = 0; z < zsize; z++)
		{
			elements.push_back(top++);
			elements.push_back(bottom++);
		}
	}

	// Define the range of terrina heights
	height_max = xs / 8.f;
	height_min = -height_max;

	// Stretch the height values to a defined height range 
	stretchToRange(height_min, height_max);

	defineSeaLevel(sealevel);

	// Calculate the normals by averaging cross products for all triangles 
	calculateNormals();
}

/* Calculate normals by using cross products along the triangle strips
   and averaging the normals for each vertex */
void terrain_object::calculateNormals()
{
	GLuint element_pos = 0;
	vec3 AB, AC, cross_product;

	// Loop through each triangle strip  
	for (GLuint x = 0; x < xsize - 1; x++)
	{
		// Loop along the strip
		for (GLuint tri = 0; tri < zsize * 2 - 2; tri++)
		{
			// Extract the vertex indices from the element array 
			GLuint v1 = elements[element_pos];
			GLuint v2 = elements[element_pos+1];
			GLuint v3 = elements[element_pos+2];
			
			// Define the two vectors for the triangle
			AB = vertices[v2] - vertices[v1];
			AC = vertices[v3] - vertices[v1];

			// Calculate the cross product
			// We have to change the winding for every second triangle to ensure the normals
			// are the correct diections. See what happens if we don't do this by using the
			// commented out line of code below instead?
//			cross_product = normalize(cross(AC, AB));		// Incorrect winding
			if (tri % 2 == 0) 
				cross_product = normalize(cross(AC, AB));
			else 
				cross_product = normalize(cross(AB, AC));

			// Add this normal to the vertex normal for all three vertices in the triangle
			normals[v1] += cross_product;
			normals[v2] += cross_product;
			normals[v3] += cross_product;

			// Move on to the next vertex along the strip
			element_pos++;
		}

		// Jump past the last two element positions to reach the start of the strip
		element_pos += 2;	
	}

	// Normalise the normals (this gives us averaged, vertex normals)
	for (GLuint v = 0; v < xsize * zsize; v++)
	{
		normals[v] = normalize(normals[v]);
	}
}

/* Stretch the height values to the range min to max */
void terrain_object::stretchToRange(GLfloat min, GLfloat max)
{
	/* Calculate min and max values */
	GLfloat cmin, cmax;
	cmin = cmax = vertices[0].y;
	for (GLuint v = 1; v < xsize*zsize; v++)
	{
		if (vertices[v].y < cmin) cmin = vertices[v].y;
		if (vertices[v].y > cmax) cmax = vertices[v].y;
	}

	// Calculate stretch factor
	GLfloat stretch_factor = (max - min) / (cmax - cmin);
	GLfloat stretch_diff = cmin - min;

	/* Rescale the vertices */
	for (GLuint v = 0; v < xsize*zsize; v++)
	{
		vertices[v].y = (vertices[v].y - stretch_diff) * stretch_factor;
	}
}


/* Calculate terrian colours */
void terrain_object::setColour(vec3 c)
{
	GLuint numVertices = xsize * zsize;

	// Loop through all vertices, set colours
	// You could set height relayed colout here if you want to or add in some random variationd
	for (GLuint i = 0; i < numVertices; i++)
	{
		vec3 height_normal = normalize(vertices[i]);
		float height = vertices[i].y;

		// Define a brown terrain colour
		colours[i] = c;

		// You could add code here to set the terrain colour based on vertex height or any other conditions

	}

}


/* Define a sea level in the terrain */
void terrain_object::defineSeaLevel(GLfloat s)
{
	sealevel = s;
	for (int v = 0; v < xsize*zsize; v++)
	{
		if (vertices[v].y < sealevel)
		{
			vertices[v].y = sealevel;
		}
	}
}


/* Calculate terrian colours based on height with small random variations */
void terrain_object::setColourBasedOnHeight()
{
	GLuint numVertices = xsize * zsize;

	// Loop through all vertices, set colour based on height
	for (GLuint i = 0; i < numVertices; i++)
	{
		glm::vec3 height_normal = glm::normalize(vertices[i]);
		float height = vertices[i].y;

		// Scale height to range 0 to 1 to use to define colours
		height = (height - height_min) / (height_max - height_min);
		float sea_norm = (sealevel - height_min) / (height_max - height_min);

		// Some random values to use for colour selection
		float rand = glm::linearRand(0.0, 0.1);
		float rand2 = glm::linearRand(0.0, 0.05);
		float rand3 = glm::linearRand(0.0, 0.02);

		// Define colour based on normalised height (0 to 1)
		// with some random variations
		if (height <= sea_norm)
			colours[i] = glm::vec3(0.3 + rand2, 0.3 + rand2, 0.9);
		else if (height <= 0.52 + rand3)
			colours[i] = glm::vec3(0.7 + rand2, 0.7, 0.2);
		else if (height <= 0.6 + rand3)
			colours[i] = glm::vec3(0.2, 0.7 + rand, 0.2);
		else if (height <= 0.93 + rand3)
			colours[i] = glm::vec3(0.6 + rand2, 0.4, 0.3);
		else
			colours[i] = glm::vec3(0.9 + rand2, 0.9 + rand2, 0.9 + rand2);

	}

}

// Get height on terrain from world coordinates
// Rounds the floating point grid values to get the nearest (int) grid point
// Note that a more accurate algorithm would be a bilinear interpolation
// of the four nearest grid points
float terrain_object::heightAtPosition(GLfloat x, GLfloat z)
{
	// Get grid position in floating point
	vec2 grid_pos = getGridPos(x, z);

	// Get integer grid position to
	int gx = round(grid_pos.x);
	int gz = round(grid_pos.y);

	// Get vertex number from integer grid position
	int vnum = gx * xsize + gz;

	// Check that the vertex number is in range
	// before getting the height value
	float grid_height = 0;
	if (vnum > 0 && vnum < xsize*zsize)
	{
		grid_height = vertices[vnum].y;
	}

	return grid_height;
}

// Get a terrain height array gtid position from a world coordinate
// Note that this will only work if you DON'T scale and shift the terrain object
vec2 terrain_object::getGridPos(GLfloat x, GLfloat z)
{
	GLfloat Xgrid = ((x + (width / 2.f)) / width) * float(xsize);
	GLfloat Zgrid = ((z + (height / 2.f)) / height) * float(zsize);

	return vec2(Xgrid, Zgrid);
}




