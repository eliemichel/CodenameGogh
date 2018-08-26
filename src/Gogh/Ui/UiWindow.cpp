#include "UiWindow.h"
#include "Logger.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nanovg.h>
#define NANOVG_GLES3_IMPLEMENTATION
#include <nanovg_gl.h>

UiWindow::UiWindow(int width, int height, std::string title, UiWindowType type)
	: m_isValid(false)
	, m_content(nullptr)
{
	switch (type) {
	case MainWindow:
		glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
		glfwWindowHint(GLFW_FLOATING, GLFW_FALSE);
		glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
		break;
	case MenuWindow:
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
		glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
		glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
		break;
	}

	// Create a GLFWwindow object that we can use for GLFW's functions
	m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
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
	glfwSetKeyCallback(m_window, [](GLFWwindow* glfwWindow, int key, int scancode, int action, int mode) {
		static_cast<UiWindow*>(glfwGetWindowUserPointer(glfwWindow))->OnKey(key, scancode, action, mode);
	});
	glfwSetCharCallback(m_window, [](GLFWwindow* glfwWindow, unsigned int codepoint) {
		static_cast<UiWindow*>(glfwGetWindowUserPointer(glfwWindow))->OnChar(codepoint);
	});
	glfwSetCursorPosCallback(m_window, [](GLFWwindow* glfwWindow, double xpos, double ypos) {
		static_cast<UiWindow*>(glfwGetWindowUserPointer(glfwWindow))->OnMouseOver(static_cast<int>(xpos), static_cast<int>(ypos));
	});
	glfwSetMouseButtonCallback(m_window, [](GLFWwindow* glfwWindow, int button, int action, int mods) {
		static_cast<UiWindow*>(glfwGetWindowUserPointer(glfwWindow))->OnMouseClick(button, action, mods);
	});
	glfwSetWindowSizeCallback(m_window, [](GLFWwindow* glfwWindow, int width, int height) {
		static_cast<UiWindow*>(glfwGetWindowUserPointer(glfwWindow))->OnResize(width, height);
	});

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

	glfwDestroyWindow(m_window);

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

	nvgBeginFrame(m_vg, static_cast<float>(m_width), static_cast<float>(m_height), pxRatio);

	// UI Objects
	if (m_content) {
		m_content->OnTick(static_cast<float>(glfwGetTime()));
		m_content->Paint(m_vg);
	}

	nvgEndFrame(m_vg);

	// Swap the screen buffers
	glfwSwapBuffers(m_window);
}

void UiWindow::Poll() const {
	glfwPollEvents();
}

void UiWindow::SetContent(UiElement *element) {
	if (element->Parent()) {
		return;
	}
	element->SetParent(this);
	m_content = element;
}

void UiWindow::OnMouseOver(int x, int y) {
	if (Content()) {
		Content()->ResetDebug();
		Content()->ResetMouse();
		Content()->OnMouseOver(x, y);
	}
}

void UiWindow::OnMouseClick(int button, int action, int mods) {
	if (Content()) {
		Content()->OnMouseClick(button, action, mods);
	}
}

void UiWindow::OnKey(int key, int scancode, int action, int mods) {
	/*
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(m_window, GL_TRUE);
	}
	*/

	if (Content()) {
		Content()->OnKey(key, scancode, action, mods);
	}
}

void UiWindow::OnChar(unsigned int codepoint) {
	if (Content()) {
		Content()->OnChar(codepoint);
	}
}

void UiWindow::OnResize(int width, int height) {
	if (Content()) {
		Content()->SetRect(0, 0, width, height);
	}
}
