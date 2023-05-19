#include "./vulkan_api.h"

/**
* VulkanAPI constructor
*/
VulkanAPI::VulkanAPI()
{
	this->initVulkan();
}

/**
*   VulkanAPI deconstructor
*/
VulkanAPI::~VulkanAPI()
{
    if (this->enable_validation_layers)
        destroyDebugUtilsMessengerEXT(this->vulkan_instance, this->debug_messenger, nullptr);

    vkDestroyInstance(this->vulkan_instance, nullptr);
}

/**
*   initVulkan()
*   desc:
*     initializes vulkan, extensions and validation layers
* 
*/
void VulkanAPI::initVulkan()
{
	this->createInstance();
    this->extensionSupport();
    this->setupDebugMessenger();
}


/**
*   createInctance()
*   desc:
*       Creates vulkan instance
*/
void VulkanAPI::createInstance()
{
    // Check for validation layers
    if (enable_validation_layers && !checkValidationLayerSupport())
        throw std::runtime_error("Validation layers requested, but not available");
    // End if

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


    // Include validation layer names
    if (enable_validation_layers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        createInfo.ppEnabledLayerNames = validation_layers.data();
    } // End if
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    std::vector<const char*> required_extensions = getRequiredExtensions();
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    createInfo.enabledExtensionCount = (uint32_t) required_extensions.size();
    createInfo.ppEnabledExtensionNames = required_extensions.data();

    // Create Vulkan Instance
    if (vkCreateInstance(&createInfo, nullptr, &this->vulkan_instance) != VK_SUCCESS) 
        throw std::runtime_error("failed to create instance!");
    // End if
} // End createInstance()

/**
*   setupDebugMessenger()
*   desc:
*       Sets up debug messaging for vulkan validation layers 
* 
*/
void VulkanAPI::setupDebugMessenger()
{
    if (!this->enable_validation_layers)
        return;
    // End if

    VkDebugUtilsMessengerCreateInfoEXT create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debugCallback;
    create_info.pUserData = nullptr; // Optional

    if (createDebugUtilsMessengerEXT(this->vulkan_instance, &create_info, nullptr, &this->debug_messenger) != VK_SUCCESS)
        throw std::runtime_error("Failed to set up debug messenger");
    // End if
} // End setupDebugMessenger

/**
*   extensionSupport()  
*   desc:
*       Prints out available vulkan extensions
* 
*/
void VulkanAPI::extensionSupport()
{
    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> vulkan_extensions_support(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, vulkan_extensions_support.data());

    std::cout << "available extensions:\n";
    for (const auto& extension : vulkan_extensions_support) 
        std::cout << '\t' << extension.extensionName << '\n';
    // End for
} // End extensionSupport

/**
*   checkValidationLayerSupport()
*   desc:
*       Checks if Vulkan Validation Layer is available
*   
*   @return bool if the Vulkan Validation Layer is available
*/
bool VulkanAPI::checkValidationLayerSupport()
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    for (const char* layer_name : this->validation_layers)
    {
        bool layer_found = false;

        for (const auto& layer_properties : available_layers)
        {
            if (strcmp(layer_name, layer_properties.layerName) == 0)
            {
                layer_found = true;
                break;
            } // End if
        } // End for

        if (!layer_found)
            return false;
        // End if
    } // End for

    return true;
} // End checkValidationLayerSupport


/**
*   getRequiredExtensions()
*   desc:
*       Gets all required extensions for glfw
*   
*   @return std::vector<const char*>
* 
*/
std::vector<const char*> VulkanAPI::getRequiredExtensions()
{
    uint32_t glfw_extentions_count = 0;
    const char** glfw_extensions;

    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extentions_count);

    std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extentions_count);

    if (this->enable_validation_layers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    } // End if


    return extensions;
} // End getRequiredExtensions


/**
*   debugCallback()
*   desc: 
*       Vulkan Callback function
* 
*   @param VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity
*   @param VkDebugUtilsMessageTypeFlagsEXT messageType
*   @param const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
*   @param  void* pUserData
* 
*   @return VkBool32
*/
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanAPI::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

/**
*   createDebugUtilsMessengerEXT()
*   desc:
*       Creates a Debug Utils Messenger EXT
* 
*   @param VkInstance instance
*   @param const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo
*   @param const VkAllocationCallbacks* pAllocator
*   @param VkDebugUtilsMessengerEXT* pDebugMessenger
* 
*   @return VkResult  
* 
*/
VkResult VulkanAPI::createDebugUtilsMessengerEXT(
    VkInstance instance, 
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
    const VkAllocationCallbacks* pAllocator, 
    VkDebugUtilsMessengerEXT* pDebugMessenger
)
{
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    // End if
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    // End else
}

/**
*   destoryDebugUtilsMessengerEXT()
*   desc:
*       Clears memory of Debug Utils Messenger EXT
*   
*   @param VkInstance vulkan_instance
*   @param VkDebugUtilsMessengerEXT debug_message
*   @param const VkAllocationCallbacks* pAllocator
*/
void VulkanAPI::destroyDebugUtilsMessengerEXT(
    VkInstance vulkan_instance, 
    VkDebugUtilsMessengerEXT debug_message, 
    const VkAllocationCallbacks* pAllocator
)
{
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkan_instance, "vkDestroyDebugUtilsMessengerETX");
    
    if (func != nullptr)
        func(vulkan_instance, debug_message, pAllocator);
} // End destroyDebugUtilsMessengerEXT
