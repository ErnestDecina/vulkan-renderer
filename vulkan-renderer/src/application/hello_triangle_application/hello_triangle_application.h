#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
	#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
	#include <GLFW/glfw3native.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "../../window/glfw_window.h"
#include "../../vulkan/vulkan_api.h"

// Symbolic Names
#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 720
#define WINDOW_NAME "Vulkan Tutorial"


class HelloTriangleApplication
{
public:
	HelloTriangleApplication();
	~HelloTriangleApplication();
	void run();

private:
	// Variables
	Window glfw_window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME);
	VulkanAPI vulkan_api = VulkanAPI(glfw_window.getWindow());

	// Methods
	void mainLoop();
	void cleanup();
}; // End class HelloTriangleApplication