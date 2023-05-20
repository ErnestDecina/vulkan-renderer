#include "./vulkan_api.h"

//
//
//  Vulkan Initialization
//
//

/**
* VulkanAPI constructor
*/
VulkanAPI::VulkanAPI()
{
	this->initVulkan();
} // End VulkanAPI constructor

/**
*   VulkanAPI deconstructor
*/
VulkanAPI::~VulkanAPI()
{
    if (this->enable_validation_layers)
        destroyDebugUtilsMessengerEXT(this->vulkan_instance, this->debug_messenger, nullptr);

    vkDestroyInstance(this->vulkan_instance, nullptr);
} // End VulkanAPI deconstructor

/**
*   initVulkan()
*   desc:
*     initializes vulkan, extensions and validation layers
* 
*/
void VulkanAPI::initVulkan()
{
	this->createInstance();
    // this->extensionSupport();
    this->setupDebugMessenger();
    // this->printPhysicalDevices();
    this->pickPhysicalDevice();
    this->printSelectedVulkanDevice();
} // End initVulkan


/**
*   createInstance()
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
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
    if (enable_validation_layers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        createInfo.ppEnabledLayerNames = validation_layers.data();
        
        populateDebugMessengerCreateInfo(debug_create_info);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
    } // End if
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    } // End else

    std::vector<const char*> required_extensions = getRequiredExtensions();
    // createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    createInfo.enabledExtensionCount = (uint32_t) required_extensions.size();
    createInfo.ppEnabledExtensionNames = required_extensions.data();

    // Create Vulkan Instance
    if (vkCreateInstance(&createInfo, nullptr, &this->vulkan_instance) != VK_SUCCESS) 
        throw std::runtime_error("failed to create instance!");
    // End if
} // End createInstance()

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

//
//
//  Validation Layers
//
//

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
    populateDebugMessengerCreateInfo(create_info);

    if (createDebugUtilsMessengerEXT(this->vulkan_instance, &create_info, nullptr, &this->debug_messenger) != VK_SUCCESS)
        throw std::runtime_error("Failed to set up debug messenger");
    // End if
} // End setupDebugMessenger

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
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkan_instance, "vkDestroyDebugUtilsMessengerEXT");
    
    if (func != nullptr)
        func(vulkan_instance, debug_message, pAllocator);
} // End destroyDebugUtilsMessengerEXT

/**
*   populateDebugMessengerCreateInfo()
*   desc:
*       Inputs the informations needed for the debug_callback
*   
*   @param VkDebugUtilsMessengerCreateInfoEXT& create_info
* 
*/
void VulkanAPI::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info)
{
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debugCallback;
} // End populateDebugMessengerCreateInfo

//
//
//  Physical Devices
//
//

/**
*   pickPhysicalDevice()
*   desc:
*       Chooses the physical GPU to utilize
* 
*/
void VulkanAPI::pickPhysicalDevice()
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(this->vulkan_instance, &device_count, nullptr);

    // If no devices have vulkan support
    if (device_count == 0)
        throw std::runtime_error("Failed to find GPU's with Vulkan support");

    // Allocate memory for all vulkan devices
    std::vector<VkPhysicalDevice> vulkan_devices(device_count);
    vkEnumeratePhysicalDevices(this->vulkan_instance, &device_count, vulkan_devices.data());
    
    // Look for the first suitable Vulkan Device
    /*
    for (const VkPhysicalDevice& device : vulkan_devices)
    {
        if (isVulkanDeviceSuitable(device))
        {
            this->physical_device = device;
            break;
        } // End if
    } // End for
    */

    // Look for the highest rate suitable Vulkan Device
    std::multimap<int, VkPhysicalDevice> vulkan_device_candidates;

    // Rate all available vulkan devices
    for (VkPhysicalDevice& device : vulkan_devices)
    {
        int score = rateVulkanDeviceSuitability(device);
        vulkan_device_candidates.insert(std::make_pair(score, device));
    } // End for

    // Check if best candidate is suitable
    if (vulkan_device_candidates.rbegin()->first > 0)
    {
        this->physical_device = vulkan_device_candidates.rbegin()->second;
    } // End if
    else
    {
        throw std::runtime_error("Failed to find a suitable GPU");
    } // End else
} // End pickPhysicalDevice

/**
*   printPhyiscalDevices()
*   desc:
*       Prints all available GPU devices
*
*/
void VulkanAPI::printPhysicalDevices()
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(this->vulkan_instance, &device_count, nullptr);

    // If no devices have vulkan support
    if (device_count == 0)
        throw std::runtime_error("Failed to find GPU's with Vulkan support");

    // Allocate memory for all vulkan devices
    std::vector<VkPhysicalDevice> vulkan_devices(device_count);
    vkEnumeratePhysicalDevices(this->vulkan_instance, &device_count, vulkan_devices.data());

    for (VkPhysicalDevice vulkan_device : vulkan_devices)
    {
        VkPhysicalDeviceProperties vulkan_device_properties;
        vkGetPhysicalDeviceProperties(vulkan_device, &vulkan_device_properties);

        VkPhysicalDeviceFeatures vulkan_device_features;
        vkGetPhysicalDeviceFeatures(vulkan_device, &vulkan_device_features);

        VkPhysicalDeviceMemoryProperties vulkan_device_memory_properties;
        vkGetPhysicalDeviceMemoryProperties(vulkan_device, &vulkan_device_memory_properties);

        std::cout << "Device Name: " << vulkan_device_properties.deviceName << std::endl;
        std::cout << "\tVulkan API Version:     " << vulkan_device_properties.apiVersion << std::endl;
        std::cout << "\tDevice Type:            " << vulkan_device_properties.deviceType << std::endl;
        std::cout << "\tDriver Verion:          " << vulkan_device_properties.driverVersion << std::endl;
        std::cout << "\tGPU Memory size:        " << vulkan_device_memory_properties.memoryHeaps[0].size / 1000000 << "MB" << std::endl;
        std::cout << "\tShared Memory size:     " << vulkan_device_memory_properties.memoryHeaps[1].size / 1000000 << "MB" << std::endl;
    } // End for
} // End printPhysicalDevice


/**
*   printSelectedVulkanDevice()
*   desc:
        Prints details of selected Vulkan Device
* 
*/
void VulkanAPI::printSelectedVulkanDevice()
{
    VkPhysicalDeviceProperties vulkan_device_properties;
    vkGetPhysicalDeviceProperties(this->physical_device, &vulkan_device_properties);

    VkPhysicalDeviceFeatures vulkan_device_features;
    vkGetPhysicalDeviceFeatures(this->physical_device, &vulkan_device_features);

    VkPhysicalDeviceMemoryProperties vulkan_device_memory_properties;
    vkGetPhysicalDeviceMemoryProperties(this->physical_device, &vulkan_device_memory_properties);

    std::cout << "Selected Device Name: " << vulkan_device_properties.deviceName << std::endl;
    std::cout << "\tVulkan API Version:     " << vulkan_device_properties.apiVersion << std::endl;
    std::cout << "\tDevice Type:            " << vulkan_device_properties.deviceType << std::endl;
    std::cout << "\tDriver Verion:          " << vulkan_device_properties.driverVersion << std::endl;
    std::cout << "\tGPU Memory size:        " << vulkan_device_memory_properties.memoryHeaps[0].size / 1000000 << "MB" << std::endl;
    std::cout << "\tShared Memory size:     " << vulkan_device_memory_properties.memoryHeaps[1].size / 1000000 << "MB" << std::endl;
} // End printSelectedVulkanDevice()

/**
*   isVulkanDevicePropertiesFeaturesSuitable()
*   desc:
*       Checks if Vulkan Device is suitable for use
*   
*   @param VkPhysicalDevice vulkan_device
*   
*   @return bool if Vulkan Device is sui5able
*/
bool VulkanAPI::isVulkanDevicePropertiesFeaturesSuitable(VkPhysicalDevice vulkan_device)
{
    VkPhysicalDeviceProperties vulkan_device_properties;
    vkGetPhysicalDeviceProperties(vulkan_device, &vulkan_device_properties);

    VkPhysicalDeviceFeatures vulkan_device_features;
    vkGetPhysicalDeviceFeatures(vulkan_device, &vulkan_device_features);

    

    return vulkan_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
            vulkan_device_features.geometryShader;
} // End isVulkanDeviceSuitable

/**
*   rateVulkanDeviceSuitability()
*   desc:
*       Checks for Vulkan Fetaures and Properties and returns a rating of the GPU
*   
*   @param VkPhysicalDevice vulkan_device
* 
*   @return int vulkan_device rating
*/
int VulkanAPI::rateVulkanDeviceSuitability(VkPhysicalDevice vulkan_device)
{
    VkPhysicalDeviceProperties vulkan_device_properties;
    vkGetPhysicalDeviceProperties(vulkan_device, &vulkan_device_properties);

    VkPhysicalDeviceFeatures vulkan_device_features;
    vkGetPhysicalDeviceFeatures(vulkan_device, &vulkan_device_features);

    // No Geometry shaders
    if (!vulkan_device_features.geometryShader)
        return 0;

    int score = 0;

    // Discrete GPU are rated higher
    if (vulkan_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 1000;

    // Max possible size of textures
    score += vulkan_device_properties.limits.maxImageDimension2D;


    return score;
} // End rateVulkanDevicesSuitability()

//
//
//  Queue Families
//
//

/**
*   QueueFamilyIndices::isComplete()
*   desc:
*       Checks if graphics_family has a value
*
*   @return bool if graphics_family has a value
*
*/
bool VulkanAPI::QueueFamilyIndices::isComplete()
{
    return graphics_family.has_value();
} // End QueueFamilyIndices::isComplete()

/**
*   findQueueFamilies()
*   desc: 
*       Check which queue families are supported by physical_device
* 
*   @param VkPhysicalDevice vulkan_device
* 
*   @return QueueFamilyIndices 
*/
VulkanAPI::QueueFamilyIndices VulkanAPI::findQueueFamilies(VkPhysicalDevice vulkan_device)
{
    QueueFamilyIndices indices;

    // 
    uint32_t vulkan_device_queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vulkan_device, &vulkan_device_queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> vulkan_device_queue_families(vulkan_device_queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(vulkan_device, &vulkan_device_queue_family_count, vulkan_device_queue_families.data());

    int  i = 0;
    for (VkQueueFamilyProperties &queue_family : vulkan_device_queue_families)
    {
        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphics_family = i;
        } // End if 

        if (indices.isComplete())
        {
            break;
        } // End if

        i++;
    } // End for

    return indices;
} // End findQueueFamilies()

/**
*   isVulkanDeviceQueueFamilySuitable()  
*   desc:
*       Checks if the vulkan devices queue family is suitable
*
*   @param VkPhysicalDevice vulkan_device
*   
*   @return bool if the vulkan device has a queue family that is suitable
* 
*/
bool VulkanAPI::isVulkanDeviceQueueFamilySuitable(VkPhysicalDevice vulkan_device)
{
    QueueFamilyIndices indices = findQueueFamilies(vulkan_device);
    return indices.isComplete();
} // End isVulkanDeviceQueueFamilySuitable()


