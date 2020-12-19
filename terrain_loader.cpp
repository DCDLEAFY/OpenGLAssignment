#include "terrain_loader.h"

terrain_loader::terrain_loader() {
	sealevel = 0;
	octaves = 4;
	perlin_scale = 4.f;
	perlin_freq = 2.f;
	land_size = 80.0f;
	heightfield = new terrain_object(octaves, perlin_freq, perlin_scale);
	heightfield->createTerrain(200, 200, land_size, land_size);
	heightfield->setColourBasedOnHeight();
	heightfield->createObject();
}