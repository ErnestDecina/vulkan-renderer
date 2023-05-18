#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

// Symbolic Names
#define VULKAN_APP_NAME "Hello Triangle"
#define VULKAN_ENGINE_NAME "No Engine"

class VulkanAPI
{
public:
	VulkanAPI();
	~VulkanAPI();

private:
	VkInstance vulkan_instance;
	void initVulkan();
	void createInstance();
}; // End class VulkanAPI