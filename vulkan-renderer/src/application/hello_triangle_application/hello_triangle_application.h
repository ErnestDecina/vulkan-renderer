#pragma once

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "../../window/glfw_window.h"
#include "../../vulkan/vulkan_api.h"


class HelloTriangleApplication
{
public:
	HelloTriangleApplication();
	~HelloTriangleApplication();
	void run();

private:
	// Variables
	Window glfw_window = Window(500, 500, "Vulkan");
	VulkanAPI vulkan_api = VulkanAPI();

	// Methods
	void initVulkan();
	void mainLoop();
	void cleanup();

}; // End class HelloTriangleApplication