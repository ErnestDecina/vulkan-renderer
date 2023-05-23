#include "./vulkan_api.h"

//
//
//  Vulkan Initialization
//
//

/**
* VulkanAPI constructor
*/
VulkanAPI::VulkanAPI(GLFWwindow* glfw_window_pointer)
{
    this->glfw_window = glfw_window_pointer;
	this->initVulkan();
} // End VulkanAPI constructor

/**
*   VulkanAPI deconstructor
*/
VulkanAPI::~VulkanAPI()
{
    if (this->enable_validation_layers)
        destroyDebugUtilsMessengerEXT(this->vulkan_instance, this->debug_messenger, nullptr);

    vkDestroyPipeline(this->vulkan_logical_device, this->vulkan_graphics_pipeline, nullptr);
    vkDestroyPipelineLayout(this->vulkan_logical_device, this->vulkan_pipeline_layout, nullptr);
    vkDestroyRenderPass(this->vulkan_logical_device, this->vulkan_render_pass, nullptr);
    this->destroyImageViews();
    vkDestroySwapchainKHR(this->vulkan_logical_device, this->vulkan_swap_chain, nullptr);
    vkDestroyDevice(this->vulkan_logical_device, nullptr);
    vkDestroySurfaceKHR(this->vulkan_instance, this->vulkan_window_surface, nullptr); // Destroying 1 out of 2 SurfaceKHR??????
    vkDestroyInstance(this->vulkan_instance, nullptr);
} // End VulkanAPI deconstructor

/**
*   getVulkanInstance()
*   desc:
*       returns vulkan instance
*   
*   @return VkInstance vulkan_instance
*/
VkInstance VulkanAPI::getVulkanInstance()
{
    return this->vulkan_instance;
} // End getVulkanInstance()

/**
*   getVulkanWindowSurface()
*   desc:
*       returns VkSurfaceKHR vulkan_window_surface
* 
*   @returns VkSurfaceKHR vulkan_window_surface
*/
VkSurfaceKHR* VulkanAPI::getVulkanWindowSurface()
{
    return &this->vulkan_window_surface;
} // End getVulkanWindowSurface()

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
    // this->printPhysicalDevices();
    this->pickPhysicalDevice();
    this->createVulkanWindowSurface();
    this->printSelectedVulkanDevice();
    this->createLogicalDevice();
    this->createSwapChain();
    this->createRenderPass();
    this->createGraphicsPipline();
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
        this->vulkan_physical_device = vulkan_device_candidates.rbegin()->second;
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
    vkGetPhysicalDeviceProperties(this->vulkan_physical_device, &vulkan_device_properties);

    VkPhysicalDeviceFeatures vulkan_device_features;
    vkGetPhysicalDeviceFeatures(this->vulkan_physical_device, &vulkan_device_features);

    VkPhysicalDeviceMemoryProperties vulkan_device_memory_properties;
    vkGetPhysicalDeviceMemoryProperties(this->vulkan_physical_device, &vulkan_device_memory_properties);

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
    return graphics_family.has_value() && present_family.has_value();
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
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(vulkan_device, i, this->vulkan_window_surface, &presentSupport);

        if (presentSupport)
        {
            indices.present_family = i;
        } // End if

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

    bool vulkan_device_extensions_supported = checkVulkanDeviceExtensionSupport(vulkan_device);

    bool swap_chain_adequate = false;
    if (vulkan_device_extensions_supported)
    {
        SwapChainSupportDetails swap_chain_support = querySwapChainSupport(vulkan_device);
        swap_chain_adequate = !swap_chain_support.vulkan_surface_formats.empty() && !swap_chain_support.present_modes.empty();
    } // End if

    return indices.isComplete() && vulkan_device_extensions_supported && swap_chain_adequate;
} // End isVulkanDeviceQueueFamilySuitable()

//
//
//  Logical device and queues
//
//

/**
*   createLogicalDevice()
*   desc:
*       Seting up a logical device to interface using physical device
* 
*/
void VulkanAPI::createLogicalDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(this->vulkan_physical_device);

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = { indices.graphics_family.value(), indices.present_family.value() };

    float queue_priority = 1.0f;
    for (uint32_t queue_family : unique_queue_families)
    {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    } // End for

    VkPhysicalDeviceFeatures device_features{};

    VkDeviceCreateInfo vulkan_logical_device_create_info{};
    vulkan_logical_device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    vulkan_logical_device_create_info.pQueueCreateInfos = queue_create_infos.data();
    vulkan_logical_device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    vulkan_logical_device_create_info.pEnabledFeatures = &device_features;
    vulkan_logical_device_create_info.enabledExtensionCount = static_cast<uint32_t>(vulkan_device_extensions.size());
    vulkan_logical_device_create_info.ppEnabledExtensionNames = vulkan_device_extensions.data();


    if (enable_validation_layers)
    {
        vulkan_logical_device_create_info.enabledLayerCount = static_cast<uint32_t> (this->validation_layers.size());
        vulkan_logical_device_create_info.ppEnabledLayerNames = validation_layers.data();
    } // End if
    else
    {
        vulkan_logical_device_create_info.enabledLayerCount = 0;
    } // End else

    if (vkCreateDevice(this->vulkan_physical_device, &vulkan_logical_device_create_info, nullptr, &this->vulkan_logical_device) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create logical device");
    } // End if
} // End createLogicalDevice()

//
//
//  Window Surface
//
//

/**
*   createVulkanWindowSurface()
*   desc:
*       creates window surface between vulkan and glfw_window
*/
void VulkanAPI::createVulkanWindowSurface()
{
    if (glfwCreateWindowSurface(this->vulkan_instance, this->glfw_window, nullptr, &this->vulkan_window_surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create GLFW window surface");
    } // End if
} // End createVulkanWindowSurface()



//
//
//  Swap Chain
//
//

/**
*   checkVulkanDeviceExtensionSupport()
*   desc:
*       Checks if Vulkan device can support required extensions
*   
*   @param VkPhysicalDevice vulkan_physical_device
* 
*   @return bool if 
*/
bool VulkanAPI::checkVulkanDeviceExtensionSupport(VkPhysicalDevice vulkan_physical_device)
{
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(vulkan_physical_device, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(vulkan_physical_device, nullptr, &extension_count, available_extensions.data());

    std::set<std::string> required_extensions(vulkan_device_extensions.begin(), vulkan_device_extensions.end());

    for (VkExtensionProperties& extension : available_extensions)
    {
        required_extensions.erase(extension.extensionName);
    } // End for

    return required_extensions.empty();
} // End checkVulkanDeviceExtensionSupport()

/**
*   querySwapChainSupport()
*   desc:
*       return SwapChainSupportDetails with vulkan physical device swap chain information and details
* 
*   @param VkPhysicalDevice vulkan_physical_device
*   
*   @return VulkanAPI::SwapChainSupportDetails
*       
*/
VulkanAPI::SwapChainSupportDetails VulkanAPI::querySwapChainSupport(VkPhysicalDevice vulkan_physical_device)
{
    SwapChainSupportDetails swap_chain_support_details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan_physical_device, this->vulkan_window_surface, &swap_chain_support_details.vulkan_surface_capabilities);

    // Surface Formats
    uint32_t vulkan_format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_physical_device, this->vulkan_window_surface, &vulkan_format_count, nullptr);

    if (vulkan_format_count != 0)
    {
        swap_chain_support_details.vulkan_surface_formats.resize(vulkan_format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_physical_device, this->vulkan_window_surface, &vulkan_format_count, swap_chain_support_details.vulkan_surface_formats.data());
    } // End if

    // Present Modes
    uint32_t vulkan_present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan_physical_device, this->vulkan_window_surface, &vulkan_present_mode_count, nullptr);

    if (vulkan_present_mode_count != 0)
    {
        swap_chain_support_details.present_modes.resize(vulkan_present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan_physical_device, this->vulkan_window_surface, &vulkan_present_mode_count, swap_chain_support_details.present_modes.data());
    } // End if

    return swap_chain_support_details;
} // End

/**
*   chooseSwapSurfaceFormat()
*   desc:
*       returns a VkSurfaceFormatKHR with the appropriate swap format
*   
*   @param const std::vector<VkSurfaceFormatKHR>& available_formats
* 
*   @return VkSurfaceFormatKHR with information of swap format
*       
*/
VkSurfaceFormatKHR VulkanAPI::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats)
{
    for (VkSurfaceFormatKHR available_format : available_formats)
    {
        if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return available_format;
        } // End if
    } // End for

    // Default to first one available format
    return available_formats[0];
} // End chooseSwapSurfaceFormat

/**
*   chooseSwapPresentMode()
*   desc:
*       returns a VkPresentModeKHR with the appropriate present mode
* 
*   @param const std::vector<VkPresentModeKHR>& available_present_modes
* 
*   @return VkPresentModeKHR with information on the present mode
* 
*/
VkPresentModeKHR VulkanAPI::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes)
{
    for (const VkPresentModeKHR& available_present_mode : available_present_modes)
    {
        if (available_present_mode == VULKAN_SWAP_CHAIN_PRESENTATION_MODE)
        {
            return available_present_mode;
        } // End if 
    } // End for

    return VK_PRESENT_MODE_FIFO_KHR;
} // End chooseSwapPresentMode()

/**
*   chooseSwapExtent()
*   desc:
*         returns a VkExtent2D with the windows extent
*   
*   @param const VkSurfaceCapabilitiesKHR& surface_capabilities
* 
*   @return VkExtent2D information of window width and window hieght
* 
*/
VkExtent2D VulkanAPI::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surface_capabilities)
{
    if (surface_capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
    {
        return surface_capabilities.currentExtent;
    } // End if
    else
    {
        int window_width;
        int window_height;

        glfwGetFramebufferSize(this->glfw_window, &window_width, &window_height);

        VkExtent2D actual_window_extent = {
            static_cast<uint32_t>(window_width),
            static_cast<uint32_t>(window_height)
        };

        actual_window_extent.width = std::clamp(actual_window_extent.width, surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width);
        actual_window_extent.height = std::clamp(actual_window_extent.height, surface_capabilities.minImageExtent.height, surface_capabilities.minImageExtent.height);
        
        return actual_window_extent;
    } // End else
} // End chooseSwapExtent()

/**
*   createSwapChain()
*   desc:
*       Creates a vulkan swap chain
*   
*/
void VulkanAPI::createSwapChain()
{
    SwapChainSupportDetails swap_chain_support = querySwapChainSupport(this->vulkan_physical_device);

    VkSurfaceFormatKHR surface_format = chooseSwapSurfaceFormat(swap_chain_support.vulkan_surface_formats);
    VkPresentModeKHR present_mode = chooseSwapPresentMode(swap_chain_support.present_modes);
    VkExtent2D extent = chooseSwapExtent(swap_chain_support.vulkan_surface_capabilities);

    uint32_t image_count = swap_chain_support.vulkan_surface_capabilities.minImageCount + 1;
    if (swap_chain_support.vulkan_surface_capabilities.maxImageCount > 0 && image_count > swap_chain_support.vulkan_surface_capabilities.maxImageCount)
    {
        image_count = swap_chain_support.vulkan_surface_capabilities.maxImageCount;
    } // End if

    // Swap Chain Creation Information
    VkSwapchainCreateInfoKHR swap_chain_create_info{};
    swap_chain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swap_chain_create_info.surface = this->vulkan_window_surface;
    
    swap_chain_create_info.minImageCount = image_count;
    swap_chain_create_info.imageFormat = surface_format.format;
    swap_chain_create_info.imageColorSpace = surface_format.colorSpace;
    swap_chain_create_info.imageExtent = extent;
    swap_chain_create_info.imageArrayLayers = 1;
    swap_chain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(this->vulkan_physical_device);
    uint32_t queue_family_indices[] = { indices.graphics_family.value(), indices.present_family.value() };


    if (indices.graphics_family != indices.present_family)
    {
        swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swap_chain_create_info.queueFamilyIndexCount = 1;
        swap_chain_create_info.pQueueFamilyIndices = queue_family_indices;
    } // End if
    else
    {
        swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swap_chain_create_info.queueFamilyIndexCount = 0;
        swap_chain_create_info.pQueueFamilyIndices = nullptr;
    } // End else

    swap_chain_create_info.preTransform = swap_chain_support.vulkan_surface_capabilities.currentTransform;
    swap_chain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swap_chain_create_info.presentMode = present_mode;
    swap_chain_create_info.clipped = VK_TRUE;
    swap_chain_create_info.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(this->vulkan_logical_device, &swap_chain_create_info, nullptr, &this->vulkan_swap_chain))
    {
        throw std::runtime_error("Failed to create swap chain");
    } // End if

    // Retrieving the swap chain images
    vkGetSwapchainImagesKHR(this->vulkan_logical_device, this->vulkan_swap_chain, &image_count, nullptr);
    this->vulkan_swap_chain_images.resize(image_count);
    vkGetSwapchainImagesKHR(this->vulkan_logical_device, this->vulkan_swap_chain, &image_count, this->vulkan_swap_chain_images.data());

    // Saving Format and Extent
    this->vulkan_swap_chain_image_format = surface_format.format;
    this->vulkan_swap_chain_extent = extent;
} // End createSwapChain

//
//
//  
// Image Views
//
//

/**
*   createImageViews()
*   desc:
*       
*/
void VulkanAPI::createImageViews()
{
    this->vulkan_swap_chain_image_views.resize(this->vulkan_swap_chain_images.size());

    for (uint32_t image_view_index = 0; image_view_index < vulkan_swap_chain_images.size(); image_view_index++)
    {
        VkImageViewCreateInfo vulkan_image_view_create_info{};
        vulkan_image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        vulkan_image_view_create_info.image = this->vulkan_swap_chain_images[image_view_index];
        vulkan_image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        vulkan_image_view_create_info.format = this->vulkan_swap_chain_image_format;
        vulkan_image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        vulkan_image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        vulkan_image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        vulkan_image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        vulkan_image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        vulkan_image_view_create_info.subresourceRange.baseMipLevel = 0;
        vulkan_image_view_create_info.subresourceRange.levelCount = 1;
        vulkan_image_view_create_info.subresourceRange.baseArrayLayer = 0;
        vulkan_image_view_create_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(this->vulkan_logical_device, &vulkan_image_view_create_info, nullptr, &vulkan_swap_chain_image_views[image_view_index]) != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed to create image views");
        } // End if
    } // End for
} // End createImageViews()

/**
*   destoryImageViews
*   desc:
*       
*/
void VulkanAPI::destroyImageViews()
{
    for (VkImageView swap_chain_image_view : this->vulkan_swap_chain_image_views)
    {
        vkDestroyImageView(this->vulkan_logical_device, swap_chain_image_view, nullptr);
    } // End for
} // End destoryImageViews()

//
//
//  Graphics Pipeline
//
//

/**
*   createGraphicsPipline()
*   desc:
*/
void VulkanAPI::createGraphicsPipline()
{
    std::vector<char> vert_sharder_code = FileSystem::readFile("E:/Code/Personal/vulkan-renderer/vulkan-renderer/src/shaders/shader-modules/compiled/vert.spv");
    std::vector<char> frag_shader_code = FileSystem::readFile("E:/Code/Personal/vulkan-renderer/vulkan-renderer/src/shaders/shader-modules/compiled/frag.spv");

    VkShaderModule vert_shader_module = createShaderModule(vert_sharder_code);
    VkShaderModule frag_shader_module = createShaderModule(frag_shader_code);

    // Vulkan Graphics Pipline Shader Stage Create Info
    VkPipelineShaderStageCreateInfo vulkan_vert_shader_stage_info{};
    vulkan_vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vulkan_vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vulkan_vert_shader_stage_info.module = vert_shader_module;
    vulkan_vert_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo vulkan_frag_shader_stage_info{};
    vulkan_frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vulkan_frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    vulkan_frag_shader_stage_info.module = frag_shader_module;
    vulkan_frag_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stages[] = {vulkan_vert_shader_stage_info, vulkan_frag_shader_stage_info};

    // Dynamic State
    std::vector<VkDynamicState> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamic_state{};
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();

    // Vertext Input
    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 0;
    vertex_input_info.pVertexBindingDescriptions = nullptr; // Optional
    vertex_input_info.vertexAttributeDescriptionCount = 0;
    vertex_input_info.pVertexAttributeDescriptions = nullptr; // Optional

    // Input Assembly
    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    // Viewports & Scissors
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) this->vulkan_swap_chain_extent.width;
    viewport.height = (float)this->vulkan_swap_chain_extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = this->vulkan_swap_chain_extent;

    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = &viewport;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = &scissor;

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    // Depth and Stencil Testing
    // Color Blending
    VkPipelineColorBlendAttachmentState  color_blend_attachment{};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo color_blending{};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;
    color_blending.blendConstants[0] = 0.0f; // Optional
    color_blending.blendConstants[1] = 0.0f; // Optional
    color_blending.blendConstants[2] = 0.0f; // Optional
    color_blending.blendConstants[3] = 0.0f; // Optional

    // Pipeline layout
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 0; // Optional
    pipeline_layout_info.pSetLayouts = nullptr; // Optional
    pipeline_layout_info.pushConstantRangeCount = 0; // Optional
    pipeline_layout_info.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(this->vulkan_logical_device, &pipeline_layout_info, nullptr, &this->vulkan_pipeline_layout))
    {
        throw std::runtime_error("Failed to create pipeline layout");
    } // End if  

    // Create Pipeline
    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisampling;
    pipeline_info.pDepthStencilState = nullptr; // Optional
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.pDynamicState = &dynamic_state;
    pipeline_info.layout = this->vulkan_pipeline_layout;
    pipeline_info.renderPass = this->vulkan_render_pass;
    pipeline_info.subpass = 0;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipeline_info.basePipelineIndex = -1; // Optional

    if (vkCreateGraphicsPipelines(this->vulkan_logical_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &this->vulkan_graphics_pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create graphics pipeline");
    } // End if

    // Clean
    vkDestroyShaderModule(this->vulkan_logical_device, frag_shader_module, nullptr);
    vkDestroyShaderModule(this->vulkan_logical_device, vert_shader_module, nullptr);
} // End createGraphicsPipeline

//
//
//  Shader Module
//
//

/**
*   createShaderModule()
*   desc:
*       
*/
VkShaderModule VulkanAPI::createShaderModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo shader_module_create_info{};
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.codeSize = code.size();
    shader_module_create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shader_module;
    if (vkCreateShaderModule(this->vulkan_logical_device, &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create shader module");
    } // End if

    return shader_module;
} // End createShaderModule()

/**
*   destroyShaderModule()
*   desc:
*       
*/
void VulkanAPI::destroyShaderModule()
{

} // End destroyShaderModule()

//
//
//  Render Passes
//
//

/**
*   createRenderPass()
*   desc:
*       
*/
void VulkanAPI::createRenderPass()
{
    // Attachment Description
    VkAttachmentDescription color_attachment{};
    color_attachment.format = this->vulkan_swap_chain_image_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // Subpasses and Attachment References
    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    // Render Pass
    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;

    if (vkCreateRenderPass(this->vulkan_logical_device, &render_pass_info, nullptr, &this->vulkan_render_pass) != VK_SUCCESS)
    {
        std::runtime_error("Failed to create render pass");
    } // End if

} // End createRenderPass()
