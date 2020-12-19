/* tiny_loader_texture.h
Example class to demonstrate the use of TinyObjectLoader to load an obj (WaveFront)
object file with normals and texture coordinates, and copy the data into vertex, normal texture coordinate buffers.

Iain Martin November 2018
*/

#pragma once

#include "wrapper_glfw.h"
#include <vector>
#include <glm/glm.hpp>

class TinyObjLoader
{
public:
	TinyObjLoader();
	~TinyObjLoader();

	void load_obj(std::string inputfile, bool debugPrint = false);
	void drawObject(int drawmode);

private:
	// Define vertex buffer object names (e.g as globals)
	GLuint positionBufferObject;
	GLuint normalBufferObject;
	GLuint texCoordsObject;

	GLuint attribute_v_coord;
	GLuint attribute_v_normal;
	GLuint attribute_v_texcoord;

	int drawmode;
	GLuint numVertices;
	GLuint numNormals;
	GLint  numTexCoords;
	GLuint numPIndexes;
};
