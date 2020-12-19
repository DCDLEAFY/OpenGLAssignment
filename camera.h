#pragma once
#include "wrapper_glfw.h"
#include <vector>
#include <glm/glm.hpp>



class camera {
public:
	GLuint viewID;
	GLuint projectionID;

	glm::mat4 view;
	glm::mat4 projection;

	glm::vec3 worldView;
	glm::vec3 originView;
	glm::vec3 headView;

	camera();
	~camera();

	void setView(glm::vec3 world, glm::vec3 origin, glm::vec3 head);
	void setWorldView(glm::vec3 world);
	void setOriginView(glm::vec3 origin);
	void setHeadView(glm::vec3 head);

	void defineProjection(GLfloat aspect_ratio);
	
	void sendUniforms();

};