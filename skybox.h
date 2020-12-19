#pragma once
#include "wrapper_glfw.h"
#include <glm/glm.hpp>
#include <vector>

class skybox {
public:
	float skyboxVerts[108];
	GLuint skyviewID;
	GLuint skyprojectionID;
	GLuint skyboxTextureID;
	GLuint skyboxVBO;
	GLuint skyboxVAO;

	std::vector<std::string> faces;

	skybox();
	void sendUniforms(glm::mat4 view, glm::mat4 projection);
	void loadSkybox();
	void bindSkybox();
	void loadCubemap(GLuint& skytexture);
	void loadFaces(GLuint& program);

};
