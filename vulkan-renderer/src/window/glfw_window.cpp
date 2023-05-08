#include "glfw_window.h"

Window::Window(int window_width, int window_height, std::string window_name)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window_name = window_name;

	initWindow();
} // End Window::Window(int window_width, int window_height, std::string window_name)

Window::~Window()
{
	glfwDestroyWindow(this->window);
	glfwTerminate();
} // End Window::~Window()

void Window::run()
{
	checkCloseWindow();
} // End Window::run()

GLFWwindow* Window::getWindow()
{
	return this->window;
} // End Window::getWindow()

void Window::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	this->window = glfwCreateWindow(this->window_width, this->window_height, this->window_name.c_str(), nullptr, nullptr);
} // Window::initWindow()

void Window::checkCloseWindow()
{
	while (!glfwWindowShouldClose(this->window)) 
	{
		glfwPollEvents();
	} // End while
} // End Window::checkCloseWindow()
