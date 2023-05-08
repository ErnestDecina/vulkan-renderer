#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <string>

class Window
{
public:
	Window(int window_width, int window_height, std::string window_name);
	~Window();

	// Getter functions
	GLFWwindow* getWindow();
private:
	int window_width;
	int window_height;
	std::string window_name;
	GLFWwindow* window;

	void initWindow();
}; // End class Window
