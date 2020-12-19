#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

camera::camera(){
	//set default view values which can be changed later on
	worldView = glm::vec3(24, 5, 0);
	originView = glm::vec3(0, 0, 0);
	headView = glm::vec3(0, 1, 0);



	setView(worldView, originView, headView);
}

camera::~camera() {

}

void camera::defineProjection(GLfloat aspect_ratio) {
	projection = glm::perspective(glm::radians(30.0f), aspect_ratio, 0.1f, 500.0f);
}


void camera::setView(glm::vec3 world, glm::vec3 origin, glm::vec3 head) {
	view = glm::lookAt(
		world,
		origin,
		head
	);
}

void camera::setWorldView(glm::vec3 world){
	view = glm::lookAt(
		world,
		originView,
		headView
	);
}

void camera::setOriginView(glm::vec3 origin) {
	view = glm::lookAt(
		worldView,
		origin,
		headView
	);
}

void camera::setHeadView(glm::vec3 head) {
	view = glm::lookAt(
		worldView,
		originView,
		head
	);
}

void camera::sendUniforms() {
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projection[0][0]);
}