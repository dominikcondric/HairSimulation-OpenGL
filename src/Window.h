#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <unordered_map>

struct Time {
	float deltaTime = 0.f;
	float lastDeltaTime = 0.f;
	float runningTime = 0.f;
	float frameRate = 0.f;
};

class Window {
public:
	Window(uint32_t winWidth = 1024, uint32_t winHeight = 768, const char* winName = "MyApplication", int sampleCount = 1);
	~Window();
	void onUpdate();
	glm::vec2 getCursorOffset() const;
	const glm::vec2 getCursorPositions() const;
	glm::ivec2 getWindowSize() const;
	bool isKeyPressed(int key) const;
	bool isKeyTapped(int key) const;
	bool isMouseButtonPressed(int key) const;
	bool shouldClose() const { return glfwWindowShouldClose(windowHandle); }
	const Time& getTime() const { return t; }
	const bool isResized() const { return resized; }

private:
	Time t;
	GLFWwindow* windowHandle = nullptr;
	double cursorX{ 512.f }, cursorY{ 384.f };
	double lastX{ 0.f }, lastY{ 0.f };
	static void windowResizeCallback(GLFWwindow* window, int w, int h);
	bool resized = false;
	mutable std::unordered_map<int, bool> keyStates;
};