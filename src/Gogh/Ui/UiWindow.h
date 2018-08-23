#ifndef H_UIWINDOW
#define H_UIWINDOW

#include "UiBase.h"
#include "UiApp.h"

#include <GLFW/glfw3.h>

// Window dimensions
const GLuint WIDTH = 1200, HEIGHT = 600;

/**
 * This must be unique.
 * TODO: separate into an App class and this class, and put every init/free
 * that must run only once in it, so that several windows can be opened
 */
class UiWindow : public UiElement {
public:
	UiWindow(UiApp *app);
	~UiWindow();

	struct NVGcontext* DrawingContext() { return m_vg; }

	bool ShouldClose() {
		return glfwWindowShouldClose(m_window);
	}

	void Render() const;

	/**
	 * Check if any events have been activated (key pressed, mouse moved etc.)
	 * and call corresponding response functions.
	 */
	void Poll() const;

	int Width() const { return m_width; }
	int Height() const { return m_height; }

	UiElement *Content() const { return m_content; }
	void SetContent(UiElement *element);

public: // protected
	void OnMouseOver(int x, int y) override;
	void OnMouseClick(int button, int action, int mods) override;
	void OnKey(int key, int scancode, int action, int mode) override;
	void OnChar(unsigned int codepoint) override;
	void OnResize(int width, int height);

private:
	bool m_isValid;
	GLFWwindow* m_window;
	struct NVGcontext* m_vg;
	UiElement *m_content;
	mutable int m_width, m_height;
	int m_font;
};

#endif // H_UIWINDOW
