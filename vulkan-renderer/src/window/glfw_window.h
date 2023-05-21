#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <string>

class Window
{
public:
	// Constructor && Destructor
	/**
	* Window instance to create a GLFW window
	*
	* @param `window_width` the width of the window
	* @param `window_height` the height of the window
	* @param `window_name` the name of the window
	*/
	Window(int window_width, int window_height, std::string window_name);

	/**
	* Clears Memory linked to GFLW window
	*/
	~Window();

	/**
	* Keeps running the window
	*/
	void run();

	// Getter functions
	/**
	* Returns w GLFWwindow pointer
	* @returns GLFWwindow* 
	*/
	GLFWwindow* getWindow();
private:
	int window_width;
	int window_height;
	std::string window_name;
	GLFWwindow* glfw_window;

	/**
	* Initializes the GLFW window
	*/
	void initWindow();

	/**
	* Checks if user has closed the window
	*/
	void checkCloseWindow();
}; // End class Window
