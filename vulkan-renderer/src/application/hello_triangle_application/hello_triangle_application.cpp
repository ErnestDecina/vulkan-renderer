#include "./hello_triangle_application.h"


HelloTriangleApplication::HelloTriangleApplication()
{
} // End HelloTriabgleApplication Constructor

HelloTriangleApplication::~HelloTriangleApplication()
{
	this->cleanup();
} // End HelloTriabgleApplication Deconstructor

void HelloTriangleApplication::run()
{
	this->mainLoop();

} // End run()

void HelloTriangleApplication::mainLoop()
{
	while (!glfwWindowShouldClose(this->glfw_window.getWindow()))
	{
		glfwPollEvents();
	} // End while
} // End mainLoop()

void HelloTriangleApplication::cleanup()
{
	this->vulkan_api.~VulkanAPI();
	this->glfw_window.~Window();
} // End cleanup();
