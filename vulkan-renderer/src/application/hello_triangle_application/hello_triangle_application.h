#pragma once

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
	VulkanAPI vulkan_api = VulkanAPI();

	// Methods
	void mainLoop();
	void cleanup();

}; // End class HelloTriangleApplication