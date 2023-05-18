#include "./vulkan_api.h"

VulkanAPI::VulkanAPI()
{
	this->initVulkan();
}

VulkanAPI::~VulkanAPI()
{
    vkDestroyInstance(this->vulkan_instance, nullptr);
}

void VulkanAPI::initVulkan()
{
	this->createInstance();
}

void VulkanAPI::createInstance()
{
    // Vulkan Application Information
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = VULKAN_APP_NAME;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = VULKAN_ENGINE_NAME;
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Vulkan Application Creation Information
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // GLFW Window
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    createInfo.enabledLayerCount = 0;

    // Create Vulkan Instance
    VkResult result = vkCreateInstance(&createInfo, nullptr, &this->vulkan_instance);

    if (vkCreateInstance(&createInfo, nullptr, &this->vulkan_instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
} // End createInstance()
