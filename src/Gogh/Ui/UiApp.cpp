#include "UiApp.h"
#include "Logger.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

UiApp::UiApp() {
	LOG << "Starting GLFW context, OpenGL ES 3.0";
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_OPENGL_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

}

UiApp::~UiApp() {
	// Terminates GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
}
