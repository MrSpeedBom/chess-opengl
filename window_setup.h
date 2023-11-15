#ifndef WINDOW_SETUP
#define WINDOW_SETUP
#include "main_memory.h"
unsigned int window_width=800,window_height=600;
GLFWwindow* window;

GLFWwindow* initWindow(const GLuint WIDTH=800,const GLuint HEIGHT=600)
{
	// Init GLFW
	window_width=WIDTH;
	window_height=HEIGHT;
	glfwInit();

	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE,true);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window=
		glfwCreateWindow(WIDTH,HEIGHT,"LearnOpenGL",nullptr,nullptr);

	int screenWidth,screenHeight;
	glfwGetFramebufferSize(window,&screenWidth,&screenHeight);

	if(nullptr==window)
	{
		std::cout<<"Failed to create GLFW window"<<std::endl;
		glfwTerminate();
		return window;
	}

	glfwMakeContextCurrent(window);

	// Set this to true so GLEW knows to use a modern approach to retrieving
	// function pointers and extensions
	glewExperimental=GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if(GLEW_OK!=glewInit())
	{
		std::cout<<"Failed to initialize GLEW"<<std::endl;
		return window;
	}

	// Define the viewport dimensions
	glViewport(0,0,screenWidth,screenHeight);
	cout<<"window success!\n";
	return window;
}

#endif