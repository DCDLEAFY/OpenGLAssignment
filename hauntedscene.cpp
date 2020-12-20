/**
 * Object Loader: Example class to demonstratre the use of the tiny_object_loader
 * to import an .obj file and create and display the object in OpenGL
 * This example uses the following:
 *	object_loader.cpp (this file)
 *	tiny_loader.cpp (uses tiny_obj_loader.cc/h to load and parse the .obj file)
 *				    (then creates the OpenGL VBOs from the tiny_obj_loader attributes)
 *	sphere_tex.cpp (to display a seperate object in this example)
 */


 /* Link to static libraries, could define these as linker inputs in the project settings instead
 if you prefer */
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glloadD.lib")
#pragma comment(lib, "opengl32.lib")
 //#pragma comment(lib, "soil.lib")



 /* Include the header to the GLFW wrapper class which
	also includes the OpenGL extension initialisation*/
#include "wrapper_glfw.h"
#include <iostream>

#include <stack>
#include <vector>

	/* Include GLM core and matrix extensions*/
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

// Include our sphere and object loader classes
#include "tiny_loader_texture.h"
#include "terrain_loader.h"
#include "camera.h"
#include "skybox.h"
#include "particle_object.h";



/* Include the image loader */
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


/* I don't like using namespaces in header files but have less issues with them in
   seperate cpp files */
using namespace std;
using namespace glm;

GLuint skyprogram;		/* Identifier for the shader prgoram */
GLuint program;
GLuint particleprogram;

GLfloat px, py, pz;
particle_object *particleObject;

GLuint vao;			/* Vertex array (Containor) object. This is the index of the VAO that will be the container for
					   our buffer objects */

					   //GLuint skyboxVAO;
GLuint colourmode;	/* Index of a uniform to switch the colour mode in the vertex shader
					  I've included this to show you how to pass in an unsigned integer into
					  your vertex shader. */

					  /* Position and view globals */
GLfloat angle_x, angle_inc_x, x, scaler, z, y;
GLfloat angle_y, angle_inc_y, angle_z, angle_inc_z;
GLuint drawmode;			// Defines drawing mode of sphere as points, lines or filled polygons

/* Uniforms*/
GLuint modelID;
GLuint colourmodeID;

GLfloat aspect_ratio;		/* Aspect ratio of the window defined in the reshape callback*/

TinyObjLoader tiny_obj[2];		// This is an instance of our tiny object loader with texture

/* Define textureID*/
GLuint texID;
GLuint texIDTwo;


//camera views that will interact with the setters of the camera class
vec3 worldview;
vec3 originview;
vec3 headview;


float distancefromground;
float increment;

//terrain 
terrain_loader* terrain;

//camera
camera scenecamera;

//skybox
skybox *sky;


//void loadCubemap(vector<std::string> faces, GLuint& textureID);
void loadtexture(const char* texpath, GLuint& texID);








/*
This function is called before entering the main rendering loop.
Use it for all your initialisation stuff
*/
void init(GLWrapper* glw)
{
	distancefromground = 4.f;
	increment = 0.5f;

	px, py, pz = 20;
	py = 50;

	particleObject = new particle_object();

	//terrain
	terrain = new terrain_loader();

	//camera
	scenecamera = camera();
	worldview = vec3(24, terrain->heightfield->heightAtPosition(24,0) + distancefromground, 0);
	scenecamera.setWorldView(worldview);
	originview = scenecamera.originView;
	headview = scenecamera.headView;
	//skybox
	sky = new skybox();

	

	//loadCubemap(sky.faces, sky.skyboxTextureID);
	/* Set the object transformation controls to their initial values */

	x = 0.05f;
	y = 0;
	z = 0;
	angle_x = angle_y = angle_z = 0;
	angle_inc_x = angle_inc_y = angle_inc_z = 0;
	scaler = 1.f;
	aspect_ratio = 1.3333f;
	colourmode = 0;


	// Generate index (name) for one vertex array object
	glGenVertexArrays(1, &vao);

	// Create the vertex array object and make it current
	glBindVertexArray(vao);



	/* Load and create our object*/
	tiny_obj[0].load_obj("..\\..\\objects\\house\\hatka_local_.obj");

	tiny_obj[1].load_obj("..\\..\\objects\\ghost\\source\\test.obj");

	//tiny_obj[2].load_obj("..\\..\\objects\\house\\hatka_local_.obj");





	//skybox
	//sky.bindSkybox();

	/* Load and build the vertex and fragment shaders */
	try
	{
		program = glw->LoadShader("hauntedscene.vert", "hauntedscene.frag");
		skyprogram = glw->LoadShader("skybox.vert", "skybox.frag");
		particleprogram = glw->LoadShader("particle_object.vert", "particle_object.frag");
	}
	catch (exception& e)
	{
		cout << "Caught exception: " << e.what() << endl;
		cin.ignore();
		exit(0);
	}

	sky->loadFaces(skyprogram);

	// Image parameters


	// load an image using the stb_image.h library 
	// Create an OpenGL texture from the image we have just loaded
	// Be careful with the GL_RGB parameter as this may need to be changed if your image is not 24-bit
	// If unsure then look at the nrChannels returned 



	// check for an error during the load process of textures
	loadtexture("..\\..\\images\\darkwood.jpg", texID);
	loadtexture("..\\..\\images\\ghosttwo.jpg", texIDTwo);





	/* Define uniforms to send to vertex shader */
	modelID = glGetUniformLocation(program, "model");
	colourmodeID = glGetUniformLocation(program, "colourmode");


	//camera
	scenecamera.viewID = glGetUniformLocation(program, "view");
	scenecamera.projectionID = glGetUniformLocation(program, "projection");
	scenecamera.sendUniforms();


	//skybox
	sky->skyviewID = glGetUniformLocation(skyprogram, "view");
	sky->skyprojectionID = glGetUniformLocation(skyprogram, "projection");


	int loc2 = glGetUniformLocation(program, "tex1");
	if (loc2 > 0) glUniform1i(loc2, 0);

	/*int loc = glGetUniformLocation(skyprogram, "skybox");
	if (loc > 0) glUniform1i(loc, 0);*/




	particleObject->create(particleprogram);
}

/* Called to update the display. Note that this function is called in the event loop in the wrapper
   class because we registered display as a callback function */
void display()
{

	/* Define the background colour */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	/* Clear the colour and frame buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Enable depth test  */
	glEnable(GL_DEPTH_TEST);

	glUseProgram(program);


	scenecamera.defineProjection(aspect_ratio);

	glUniform1ui(colourmodeID, colourmode);

	//camera
	scenecamera.sendUniforms();


	//cout << "x: " << worldview.x << ", y: " << worldview.y << ", z: " << worldview.z << '\n' << endl;

	//Can use this later in order to get first person effect
	//cout << worldview.x << ", "<< worldview.y << ", " << worldview.z << "\n" << endl;


	stack<mat4> modelscene;
	modelscene.push(mat4(1.0));


	modelscene.push(modelscene.top()); {
		modelscene.top() = translate(modelscene.top(), vec3(5, terrain->heightfield->heightAtPosition(2, 0), 0));
		modelscene.top() = scale(modelscene.top(), vec3(0.01, 0.01, 0.01));


		glUniformMatrix4fv(modelID, 1, GL_FALSE, &modelscene.top()[0][0]);

		glBindTexture(GL_TEXTURE_2D, texID);
		tiny_obj[0].drawObject(drawmode);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	modelscene.pop();

	modelscene.push(modelscene.top()); {


		modelscene.top() = translate(modelscene.top(), vec3(-10, terrain->heightfield->heightAtPosition(0, 0) + 6, 10));
		modelscene.top() = scale(modelscene.top(), vec3(1, 1, 1));


		glUniformMatrix4fv(modelID, 1, GL_FALSE, &modelscene.top()[0][0]);

		glBindTexture(GL_TEXTURE_2D, texIDTwo);
		tiny_obj[1].drawObject(drawmode);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	modelscene.pop();



	modelscene.push(modelscene.top()); {


		glUniformMatrix4fv(modelID, 1, GL_FALSE, &modelscene.top()[0][0]);
		glBindTexture(GL_TEXTURE_2D, texID);
		terrain->heightfield->drawObject(drawmode);
		glBindTexture(GL_TEXTURE_2D, 0);

	}
	modelscene.pop();





	glUseProgram(0);

	glDepthFunc(GL_LEQUAL);

	/* Make the compiled shader program current */
	glUseProgram(skyprogram);

	mat4 skyview = scenecamera.view;
	mat4 skyproj = scenecamera.projection;

	glUniformMatrix4fv(sky->skyviewID, 1, GL_FALSE, &skyview[0][0]);
	glUniformMatrix4fv(sky->skyprojectionID, 1, GL_FALSE, &skyproj[0][0]);

	sky->loadSkybox();

	glDisableVertexAttribArray(0);
	glUseProgram(0);

	glDepthFunc(GL_LESS);

	mat4 ParticleView = lookAt(
		scenecamera.worldView,
		vec3(-px, -py, -pz),
		scenecamera.headView
	);

	particleObject->drawParticles(scenecamera.projection, ParticleView);


	/* Modify our animation variables */
	angle_x += angle_inc_x;
	angle_y += angle_inc_y;
	angle_z += angle_inc_z;
}

void loadtexture(const char* texpath, GLuint& texID) {

	glGenTextures(1, &texID);

	int width, height, nrChannels;

	unsigned char* datatexture = stbi_load(texpath, &width, &height, &nrChannels, 0);

	if (datatexture)
	{
		// Note: this is not a full check of all pixel format types, just the most common two!
		int pixel_format = 0;
		if (nrChannels == 3)
			pixel_format = GL_RGB;
		else
			pixel_format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, texID);

		// Create the texture, passing in the pointer to the loaded image pixel data
		glTexImage2D(GL_TEXTURE_2D, 0, pixel_format, width, height, 0, pixel_format, GL_UNSIGNED_BYTE, datatexture);

		// Generate Mip Maps
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		printf("stb_image  loading error:");
		exit(0);
	}
	stbi_image_free(datatexture);
}



/* Called whenever the window is resized. The new window size is given, in pixels. */
static void reshape(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	aspect_ratio = ((float)w / 640.f * 4.f) / ((float)h / 480.f * 3.f);
}


void updateYWorldViewTerrain() {
	worldview.y = terrain->heightfield->heightAtPosition(worldview.x, worldview.z) + distancefromground;
	scenecamera.setView(worldview, originview, headview);
}


void updateWorldViewx(float incrementVal) {
	worldview.x += incrementVal;
	updateYWorldViewTerrain();
}

void updateWorldViewz(float incrementVal) {
	worldview.z += incrementVal;
	updateYWorldViewTerrain();
}

void updateOriginViewx(float incrementVal) {
	originview.x += incrementVal;
	updateYWorldViewTerrain();
}

void updateOriginViewz(float incrementVal) {
	originview.z += incrementVal;
	updateYWorldViewTerrain();
}

void updateOriginViewy(float incrementVal) {
	originview.y += incrementVal;
	updateYWorldViewTerrain();
}


/* change view angle, exit upon ESC */
static void keyCallback(GLFWwindow* window, int key, int s, int action, int mods)
{
	/* Enable this call if you want to disable key responses to a held down key*/
	//if (action != GLFW_PRESS) return;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);



	//World view camera controls
	if (key == 'U') {
		updateWorldViewx(increment);
	}
	if (key == 'I') {
		updateWorldViewx(-increment);
	}
	if (key == 'O') {
		updateWorldViewz(increment);
	}
	if (key == 'P') {
		updateWorldViewz(-increment);
	}
	if (key == 'W') {
		updateOriginViewy(increment);
	}
	if (key == 'A') {
		updateOriginViewz(-increment);
	}
	if (key == 'S') {
		updateOriginViewy(-increment);
	}
	if (key == 'D') {
		updateOriginViewz(increment);
	}
	if (key == 'E') {
		updateOriginViewx(-increment);
	}
	if (key == 'Q') {
		updateOriginViewx(increment);
	}


	if (key == 'A') scaler -= 0.02f;
	if (key == 'S') scaler += 0.02f;
	if (key == 'Z') x -= 0.05f;
	if (key == 'X') x += 0.05f;
	if (key == 'C') y -= 0.05f;
	if (key == 'V') y += 0.05f;
	if (key == 'B') z -= 0.05f;
	if (key == 'N') z += 0.05f;

	if (key == 'M' && action != GLFW_PRESS)
	{
		colourmode = !colourmode;
		cout << "colourmode=" << colourmode << endl;
	}

	/* Cycle between drawing vertices, mesh and filled polygons */
	if (key == 'N' && action != GLFW_PRESS)
	{
		drawmode++;
		if (drawmode > 2) drawmode = 0;
	}

}




/* Entry point of program */
int main(int argc, char* argv[])
{
	GLWrapper* glw = new GLWrapper(1024, 768, "Lab5: Fun with texture");;

	if (!ogl_LoadFunctions())
	{
		fprintf(stderr, "ogl_LoadFunctions() failed. Exiting\n");
		return 0;
	}

	glw->setRenderer(display);
	glw->setKeyCallback(keyCallback);
	glw->setReshapeCallback(reshape);

	init(glw);

	glw->eventLoop();

	delete(glw);
	return 0;
}
