#include <iostream>
#include "Window.h"
#include <glm/common.hpp>

Window::Window(uint32_t winWidth, uint32_t winHeight, const char* winName, int sampleCount)
{
	GLFWwindow* window = nullptr;

	/* Initialize the library */
	if (!glfwInit())
		std::cerr << "Failed to initialize GLFW!" << std::endl;

	glfwWindowHint(GLFW_SAMPLES, sampleCount);
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(winWidth, winHeight, winName, NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		std::cerr << "Failed to create window!" << std::endl;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetWindowUserPointer(window, this);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetWindowUserPointer(window, &resized);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
	}

	this->windowHandle = window;

	onUpdate();
}

Window::~Window()
{
	glfwTerminate();
}

void Window::onUpdate()
{
	resized = false;

	// Cursor update
	lastX = cursorX;
	lastY = cursorY;
	glfwGetCursorPos(windowHandle, &cursorX, &cursorY);

	for (auto& pair : keyStates)
	{
		pair.second = isKeyPressed(pair.first);
	}

	// Updating keys
	glfwPollEvents();
	glfwSwapBuffers(windowHandle);

	// Updating time
	float currentTime = (float)glfwGetTime();
	t.lastDeltaTime = t.deltaTime;
	t.deltaTime = currentTime - t.runningTime;
	t.runningTime = currentTime;
	t.frameRate = 1.f / t.deltaTime;

	if (glm::abs(t.deltaTime - t.lastDeltaTime) > 0.2f)
		std::cout << "Frame rate: " << t.frameRate << std::endl;

}

bool Window::isKeyPressed(int key) const
{
	return bool(glfwGetKey(windowHandle, key));
}

bool Window::isKeyTapped(int key) const
{
	if (keyStates.find(key) != keyStates.end())
	{
		return (keyStates[key] == false && isKeyPressed(key) == true);
	}
	else
	{
		keyStates.insert({ key, false });
	}

	return false;
}

bool Window::isMouseButtonPressed(int key) const
{
	return bool(glfwGetMouseButton(windowHandle, key));
}

glm::ivec2 Window::getWindowSize() const
{
	glm::ivec2 winSize;
	glfwGetWindowSize(windowHandle, &winSize.x, &winSize.y);
	return winSize;
}

const glm::vec2 Window::getCursorPositions() const
{
	return glm::vec2(cursorX, cursorY);
}

glm::vec2 Window::getCursorOffset() const
{
	glm::vec2 cursorOff;
	cursorOff.x = (float)cursorX - (float)lastX;
	cursorOff.y = (float)lastY - (float)cursorY;
	return cursorOff;
}

void Window::windowResizeCallback(GLFWwindow* window, int w, int h)
{
	*(static_cast<bool*>(glfwGetWindowUserPointer(window))) = true;
}
