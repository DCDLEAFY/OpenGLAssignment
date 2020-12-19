#pragma once
#include "wrapper_glfw.h"
#include <glm/glm.hpp>
#include "terrain_object.h"

class terrain_loader {
public:
	terrain_object *heightfield;
	int octaves;
	GLfloat perlin_scale, perlin_freq, land_size, sealevel;

	terrain_loader();
};