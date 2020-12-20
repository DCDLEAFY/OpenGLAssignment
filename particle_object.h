/*
 particle_object.cpp
 Component from a basic example of a particle animation
 Adaped from the particle animation tutorial from:
 http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/

  Iain Martin November 2014
*/

#pragma once

#include "wrapper_glfw.h"
#include <glm/glm.hpp>


// CPU representation of a particle
struct Particle{
	glm::vec3 pos, speed;
	unsigned char r, g, b, a; // Color
	float size, angle, weight;
	float life; // Remaining life of the particle. if < 0 : dead and unused.

	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}

};

class particle_object
{
public:
	particle_object();
	~particle_object();

	void create(GLuint program);
	int FindUnusedParticle();
	void SortParticles();
	void drawParticles(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix);
	void defineUniforms();
		
	GLuint billboard_vertex_buffer;
	GLuint particles_position_buffer;
	GLuint particles_color_buffer;
	const int MaxParticles = 10000;
	Particle ParticlesContainer[10000];
	int LastUsedParticle;
	GLfloat* g_particule_position_size_data;
	GLubyte* g_particule_color_data;
	int ParticlesCount;
	double lastTime;

	GLuint VertexArrayID;
	GLuint programID;
	GLuint Texture;
	GLuint TextureID;
	GLuint CameraRight_worldspace_ID;
	GLuint CameraUp_worldspace_ID;
	GLuint ViewProjMatrixID;
};

