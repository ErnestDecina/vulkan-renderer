#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>

#ifdef NDBUG
	#define DEBUG false
#else
	#define DEBUG true
#endif


// Symbolic Names
#define VULKAN_APP_NAME "Hello Triangle"
#define VULKAN_ENGINE_NAME "No Engine"

class VulkanAPI
{
public:
	VulkanAPI();
	~VulkanAPI();

private:
	// Release
	VkInstance vulkan_instance;

	void initVulkan();
	void createInstance();
	void extensionSupport();

	// DEBUG
	const std::vector<const char*> validation_layers = { "VK_LAYER_KHRONOS_validation" };
	const bool enable_validation_layers = false;
	VkDebugUtilsMessengerEXT debug_messenger;

	void setupDebugMessenger();
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT*, void*);
	VkResult createDebugUtilsMessengerEXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, const VkAllocationCallbacks*, VkDebugUtilsMessengerEXT*);
	void destroyDebugUtilsMessengerEXT(VkInstance, VkDebugUtilsMessengerEXT, const VkAllocationCallbacks*);

}; // End class VulkanAPI