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
#include <set>
#include <optional>

#include <cstdint> // Necessary for uint32_t
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp

// DEBUG
#define DEBUG_STATE true

// Symbolic Names
#define VULKAN_APP_NAME "Hello Triangle"
#define VULKAN_ENGINE_NAME "No Engine"
#define VULKAN_SWAP_CHAIN_PRESENTATION_MODE VK_PRESENT_MODE_MAILBOX_KHR

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
	VkQueue present_queue;
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphics_family;
		std::optional<uint32_t> present_family;

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

	/**
	*	Swap Chain
	*/
	VkSwapchainKHR vulkan_swap_chain;
	std::vector<VkImage> vulkan_swap_chain_images;
	VkFormat vulkan_swap_chain_image_format;
	VkExtent2D vulkan_swap_chain_extent;

	const std::vector<const char*> vulkan_device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR vulkan_surface_capabilities;
		std::vector<VkSurfaceFormatKHR> vulkan_surface_formats;
		std::vector<VkPresentModeKHR> present_modes;
	};

	bool checkVulkanDeviceExtensionSupport(VkPhysicalDevice);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>&);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>&);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR&);
	void createSwapChain();

	/**
	*	Image Views
	*/
	std::vector<VkImageView> vulkan_swap_chain_image_views;

	void createImageViews();
	void destroyImageViews();

	/**
	*	Graphics Pipeline Intro
	*/

	void createGraphicsPipline();
}; // End class VulkanAPI