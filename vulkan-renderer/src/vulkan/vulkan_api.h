#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
	#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
	#include <GLFW/glfw3native.h>

#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <map>
#include <optional>

// DEBUG
#define DEBUG_STATE true

// Symbolic Names
#define VULKAN_APP_NAME "Hello Triangle"
#define VULKAN_ENGINE_NAME "No Engine"

class VulkanAPI
{
public:
	VulkanAPI(GLFWwindow*);
	~VulkanAPI();

	VkInstance getVulkanInstance();
	VkSurfaceKHR* getVulkanWindowSurface();

private:
	/**
	*	Vulkan Initialization
	*	desc:
	*		Used to create a Vulkan Instance
	*/
	VkInstance vulkan_instance;

	void initVulkan();
	void createInstance();
	void extensionSupport();

	/**
	*	Validation Layers
	*	desc:
	*		Used for DEBUG output messages
	*/
	const std::vector<const char*> validation_layers = { "VK_LAYER_KHRONOS_validation" };
	const bool enable_validation_layers = DEBUG_STATE;
	VkDebugUtilsMessengerEXT debug_messenger;

	void setupDebugMessenger();
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT*, void*);
	VkResult createDebugUtilsMessengerEXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, const VkAllocationCallbacks*, VkDebugUtilsMessengerEXT*);
	void destroyDebugUtilsMessengerEXT(VkInstance, VkDebugUtilsMessengerEXT, const VkAllocationCallbacks*);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT&);

	/**
	*	Physical devices
	*/
	VkPhysicalDevice vulkan_physical_device = VK_NULL_HANDLE;

	void pickPhysicalDevice();
	void printPhysicalDevices();
	void printSelectedVulkanDevice();
	bool isVulkanDevicePropertiesFeaturesSuitable(VkPhysicalDevice);
	int	rateVulkanDeviceSuitability(VkPhysicalDevice);

	/**
	*	Queue Families 
	*/
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphics_family;
		bool isComplete();
	};

	VulkanAPI::QueueFamilyIndices findQueueFamilies(VkPhysicalDevice);
	bool isVulkanDeviceQueueFamilySuitable(VkPhysicalDevice);

	/**
	*	Logical device and queues
	*/
	VkDevice vulkan_logical_device;
	VkQueue graphics_queue;

	void createLogicalDevice();

	/**
	*	Window Surface
	*/
	VkSurfaceKHR vulkan_window_surface;
	GLFWwindow* glfw_window = nullptr;

	void createVulkanWindowSurface();

}; // End class VulkanAPI