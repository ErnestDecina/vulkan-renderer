#include "glfw_window.h"

/**
* Window instance to create a GLFW window
* 
* @param `window_width` the width of the window
* @param `window_height` the height of the window
* @param `window_name` the name of the window
*/
Window::Window(int window_width, int window_height, std::string window_name)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window_name = window_name;

	initWindow();
}

Window::~Window()
{
	glfwDestroyWindow(this->window);
	glfwTerminate();
}

GLFWwindow* Window::getWindow()
{
	return this->window;
}

void Window::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	this->window = glfwCreateWindow(this->window_width, this->window_height, this->window_name.c_str(), nullptr, nullptr);
}
