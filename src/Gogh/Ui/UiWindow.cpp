#include "UiWindow.h"
#include "Logger.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nanovg.h>
#define NANOVG_GLES3_IMPLEMENTATION
#include <nanovg_gl.h>

UiWindow::UiWindow(UiApp *app)
	: m_isValid(false)
	, m_content(nullptr)
{
	if (!app)
	{
		ERR_LOG << "A window requires to be launched in an initialized app";
		return;
	}

	// Create a GLFWwindow object that we can use for GLFW's functions
	m_window = glfwCreateWindow(WIDTH, HEIGHT, "Gogh", nullptr, nullptr);
	glfwMakeContextCurrent(m_window);
	if (!m_window)
	{
		ERR_LOG << "Failed to create GLFW window";
		glfwTerminate();
		m_isValid = false;
		return;
	}

	// Set the required callback functions
	glfwSetWindowUserPointer(m_window, static_cast<void*>(this));
	glfwSetKeyCallback(m_window, key_callback);
	glfwSetCursorPosCallback(m_window, cursor_pos_callback);
	glfwSetMouseButtonCallback(m_window, mouse_button_callback);
	glfwSetWindowSizeCallback(m_window, window_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		ERR_LOG << "Failed to initialize OpenGL context";
		m_isValid = false;
		return;
	}

	// Init NanoVG
	glfwMakeContextCurrent(m_window);
	std::cout << "Starting NanoVG" << std::endl;
	m_vg = nvgCreateGLES3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG); // TODO: try w/o NVG_STENCIL_STROKES
	if (!m_vg) {
		ERR_LOG << "Failed to initialize NanoVG context";
		glfwTerminate();
		m_isValid = false;
		return;
	}

	int font = nvgCreateFont(m_vg, "SegeoUI", SHARE_DIR"\\fonts\\segoeui.ttf");

	m_isValid = true;
}

UiWindow::~UiWindow() {
	glfwSetWindowUserPointer(m_window, nullptr);

	// Must be before nvgDelete
	if (m_content) {
		delete m_content;
	}

	// Destroy NanoVG ctxw
	nvgDeleteGLES3(m_vg);
}

void UiWindow::Render() const {
	int fbWidth, fbHeight;
	float pxRatio;

	glfwMakeContextCurrent(m_window);

	glfwGetWindowSize(m_window, &m_width, &m_height);
	glfwGetFramebufferSize(m_window, &fbWidth, &fbHeight);
	// Calculate pixel ration for hi-dpi devices.
	pxRatio = (float)fbWidth / (float)m_width;

	glViewport(0, 0, fbWidth, fbHeight);

	// Render
	// Clear the colorbuffer
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	nvgBeginFrame(m_vg, m_width, m_height, pxRatio);

	// UI Objects
	if (m_content) {
		m_content->OnTick();
		m_content->Paint(m_vg);
	}

	nvgEndFrame(m_vg);

	// Swap the screen buffers
	glfwSwapBuffers(m_window);
}

void UiWindow::Poll() const {
	glfwPollEvents();
}


// Callbacks

// Is called whenever a key is pressed/released via GLFW
void UiWindow::key_callback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mode)
{
	UiWindow* window = static_cast<UiWindow*>(glfwGetWindowUserPointer(glfwWindow));
	if (!window) {
		return;
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(glfwWindow, GL_TRUE);
}

void UiWindow::cursor_pos_callback(GLFWwindow* glfwWindow, double xpos, double ypos)
{
	UiWindow* window = static_cast<UiWindow*>(glfwGetWindowUserPointer(glfwWindow));
	if (!window || !window->Content()) {
		return;
	}

	window->Content()->ResetDebug();
	window->Content()->ResetMouse();
	window->Content()->OnMouseOver(static_cast<int>(xpos), static_cast<int>(ypos));
}


void UiWindow::mouse_button_callback(GLFWwindow* glfwWindow, int button, int action, int mods)
{
	UiWindow* window = static_cast<UiWindow*>(glfwGetWindowUserPointer(glfwWindow));
	if (!window || !window->Content()) {
		return;
	}

	window->Content()->OnMouseClick(button, action, mods);
}

void UiWindow::window_size_callback(GLFWwindow* glfwWindow, int width, int height) {
	UiWindow* window = static_cast<UiWindow*>(glfwGetWindowUserPointer(glfwWindow));
	if (!window || !window->Content()) {
		return;
	}

	window->Content()->SetRect(0, 0, width, height);
}
