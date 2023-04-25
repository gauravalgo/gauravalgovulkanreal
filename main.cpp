#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <optional>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <set>
#include <chrono>
#include <thread>
#include <cstdint>
#include <limits>
#include <algorithm>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char* > validationLayers = {
                    "VK_LAYER_KHRONOS_validation"
    
};
const std::vector<const char* > deviceExtensions = {
                    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
    const bool enableValidationLayers = true;
#else 
    const bool enableValidationLayers = true;
#endif
    
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessager)
{
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                                "vkCreateDebugUtilsMessengerEXT");
            if(func != nullptr){
                return func(instance,pCreateInfo,pAllocator,pDebugMessager);
                
        }
        else{
            return VK_ERROR_EXTENSION_NOT_PRESENT;
            
        }
}
void DestroyDebugUtilsMessangerEXT(VkInstance instance,
                        VkDebugUtilsMessengerEXT debugmessanger,
                        const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                        "vkDestroyDebugUtilsMessengerEXT");
    if(func != nullptr)
    {

        func(instance,debugmessanger,pAllocator);
        
    }
}
//#define qcreate 0
#define ASSERT_VULKAN(val) \
            if(val== VK_SUCCESS)\
                __debugbreak();\

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
    uint32_t total_number_queues;
};
struct SwapChainSupportDetails {

    VkSurfaceCapabilitiesKHR    capabilities;
    std::vector<VkSurfaceFormatKHR>  formats;
    std::vector<VkPresentModeKHR>   presentModes;
};
class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }


    GLFWwindow* window;
    VkInstance instance;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    std::vector<VkPhysicalDevice> devices;
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures{};
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos; 
    VkDeviceQueueCreateInfo queuecreateinfo[2];
    VkDebugUtilsMessengerEXT debugmessanger{};
    VkDeviceCreateInfo logicalcreateinfo{};
    QueueFamilyIndices indecies;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    void initWindow() {
        int width=800;
        int height=600;
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwGetFramebufferSize(window,&width,&height);
    }
    
    void initVulkan() {
         createInstance();
         setupDebuggerMessanger();
         createSurface();
         pickPhysicalDevice();
         getdeviceproperties();
         getdevicefeatures();
         createQueueLogicaldevice();
         createLogicalDevice();
         createSwapChain();
         createImageViews();
         
    }
    void createImageViews()
    {
        swapChainImageViews.resize( swapChainImages.size());
        for(size_t i=0; i<swapChainImages.size();i++)
        {
            VkImageViewCreateInfo createinfo{};
            createinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createinfo.image = swapChainImages[i];
            createinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createinfo.format = swapChainImageFormat;
            createinfo.components.a=VK_COMPONENT_SWIZZLE_IDENTITY;
            createinfo.components.b=VK_COMPONENT_SWIZZLE_IDENTITY;
            createinfo.components.g=VK_COMPONENT_SWIZZLE_IDENTITY;
            createinfo.components.r=VK_COMPONENT_SWIZZLE_IDENTITY;
            createinfo.subresourceRange.aspectMask=VK_IMAGE_ASPECT_COLOR_BIT;
            createinfo.subresourceRange.baseMipLevel = 0;
            createinfo.subresourceRange.levelCount = 1;
            createinfo.subresourceRange.baseArrayLayer =0;
            createinfo.subresourceRange.layerCount = 1;
            if(vkCreateImageView(device,&createinfo,nullptr,&swapChainImageViews[i])!=VK_SUCCESS)
            {
                throw std::runtime_error("failed to create image Views");
            }
        }
    }
    void createSwapChain()
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
        
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount +1 ;
        
        if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }
        
        VkSwapchainCreateInfoKHR createinfo{};
        createinfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createinfo.surface = surface;
        createinfo.minImageCount = imageCount;
        createinfo.imageFormat = surfaceFormat.format;
        createinfo.imageColorSpace = surfaceFormat.colorSpace;
        createinfo.imageExtent = extent;
        createinfo.imageArrayLayers =1;
        createinfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),indices.presentFamily.value()};
        if(indices.graphicsFamily!=indices.presentFamily)
        {
            createinfo.imageSharingMode=VK_SHARING_MODE_CONCURRENT;
            createinfo.queueFamilyIndexCount= 2;
            createinfo.pQueueFamilyIndices = queueFamilyIndices;
            
        }
        else{
            createinfo.imageSharingMode=VK_SHARING_MODE_EXCLUSIVE;
            createinfo.queueFamilyIndexCount=0;
            createinfo.pQueueFamilyIndices=nullptr;
        }
        
        createinfo.preTransform=swapChainSupport.capabilities.currentTransform;
        createinfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createinfo.presentMode = presentMode;
        createinfo.clipped=VK_TRUE;
        createinfo.oldSwapchain=VK_NULL_HANDLE;
        
        if((vkCreateSwapchainKHR(device,&createinfo,nullptr,&swapChain))!=VK_SUCCESS)
        {
            throw std::runtime_error("failed to create swapChain");
        }
        
        vkGetSwapchainImagesKHR(device,swapChain,&imageCount,nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device,swapChain,&imageCount,swapChainImages.data());
        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }
    void createSurface()
    {
    
        if(glfwCreateWindowSurface(instance,window,nullptr,&surface)!=VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface");
        }
    }
    void getdeviceproperties()
    {
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
        

    }
    
    void getdevicefeatures()
    {
        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
    }
      void createQueueLogicaldevice()
    {
        
        indecies = findQueueFamilies(physicalDevice);
        float queuePriority = 1.0f;
        std::set<uint32_t> uniqueQueueFamilies={indecies.graphicsFamily.value(),indecies.presentFamily.value()};
        int i=0;
        for(uint32_t queueFamily : uniqueQueueFamilies)
        {
               
            queuecreateinfo[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queuecreateinfo[i].flags = 0;
            queuecreateinfo[i].pNext = nullptr;
            queuecreateinfo[i].queueFamilyIndex = queueFamily;
            queuecreateinfo[i].pQueuePriorities =  &queuePriority;
            queuecreateinfo[i].queueCount = 1; //indicates the queue created for graphicsFamily should be implemented for this logical device
            queueCreateInfos.push_back(queuecreateinfo[i]);
            i++;
        }
    }
    void createLogicalDevice()
    {
    
        logicalcreateinfo.sType= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        logicalcreateinfo.pQueueCreateInfos = queueCreateInfos.data();
        logicalcreateinfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        logicalcreateinfo.pEnabledFeatures = &deviceFeatures;
        logicalcreateinfo.enabledExtensionCount=static_cast<uint32_t>(deviceExtensions.size());
        logicalcreateinfo.ppEnabledExtensionNames = deviceExtensions.data();
        if(enableValidationLayers)
        {
            logicalcreateinfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            logicalcreateinfo.ppEnabledLayerNames = validationLayers.data();
        }
        if(vkCreateDevice(physicalDevice,&logicalcreateinfo,nullptr,&device)!=VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device");
        }
        vkGetDeviceQueue(device,indecies.graphicsFamily.value(),0,&graphicsQueue);
        vkGetDeviceQueue(device,indecies.presentFamily.value(),0,&presentQueue);
    
    }   
    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    

  
   
    void cleanup() {
        for(auto imageView : swapChainImageViews)
        {
            vkDestroyImageView(device,imageView,nullptr);
        }
        vkDestroySwapchainKHR(device,swapChain,nullptr);
        vkDestroyDevice(device,nullptr);
        if(enableValidationLayers)
        {
            DestroyDebugUtilsMessangerEXT(instance,debugmessanger,nullptr);
        }
        vkDestroySurfaceKHR(instance,surface,nullptr);
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);

        glfwTerminate();
    }
    bool checkDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device,nullptr,&extensionCount,nullptr);
        
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device,nullptr,&extensionCount,availableExtensions.data());
        
        std::set<std::string> requiredExtensions(deviceExtensions.begin(),deviceExtensions.end());
        for(const auto & extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }
        return requiredExtensions.empty();
    }
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device)
    {
    
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,surface,&details.capabilities);
        
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&formatCount,nullptr);
        
        if(formatCount!=0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&formatCount,details.formats.data());
        }
        
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&presentModeCount,nullptr);
        
        if(presentModeCount!=0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&presentModeCount,details.presentModes.data());
        }
        
        
        return details;
    }
    bool isDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndices indecies;
        indecies = findQueueFamilies(device);
        bool extensionSupported = checkDeviceExtensionSupport(device) ;
        bool swapChainAdequate = false;
        if(extensionSupported){
        
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
            
        }
        return indecies.isComplete() && extensionSupported && swapChainAdequate;
    }
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;
        uint32_t amountofqueuefamiliesproperties = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device,&amountofqueuefamiliesproperties,nullptr);
        std::vector<VkQueueFamilyProperties> queuefamilies(amountofqueuefamiliesproperties);
        vkGetPhysicalDeviceQueueFamilyProperties(device,&amountofqueuefamiliesproperties,queuefamilies.data());
        int i=0;
        for(const auto & vkqueuefamily : queuefamilies)
        {
            if(vkqueuefamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
                
                
                 std::cout<<"graphicsFamily support included"<< i << std::endl;
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device,i,surface,&presentSupport);
          // std::cout<<"present support included"<<presentSupport<<std::endl;
           std::this_thread::sleep_for(std::chrono::milliseconds(500));
            if(presentSupport)
            {
            
                std::cout<<"present support included"<< i << std::endl;
                indices.presentFamily = i;
            }
            if(indices.isComplete())
            {
                    break;
            }
            i++;
        }
        indices.total_number_queues = amountofqueuefamiliesproperties;
        return indices;

    }
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities)
    {
        if(capabilities.currentExtent.width!= std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width,height;
            glfwGetFramebufferSize(window,&width,&height);
            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };
            
            actualExtent.width = std::clamp(actualExtent.width,capabilities.minImageExtent.width,capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height,capabilities.minImageExtent.height,capabilities.maxImageExtent.height);
            return actualExtent;
        }
        
    }
    
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for(const auto &availablePresentMode : availablePresentModes)
        {
            if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        
        for(const auto & availableFormat: availableFormats)
        {
            if(availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }
    void pickPhysicalDevice()
    {
        uint32_t deviceCount=0;
        VkResult result;
        result = vkEnumeratePhysicalDevices(instance,&deviceCount,nullptr);

        if(deviceCount == 0 && result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to find Gpus which have Vulkan support");
        }
        devices.resize(deviceCount);
        result = vkEnumeratePhysicalDevices(instance,&deviceCount,devices.data());
         if(result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to find Gpus which have Vulkan support");
        }

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }
    void createInstance()
    {
        if(enableValidationLayers && !checkValidationLayerSupport()){
            throw std::runtime_error("validation requested but not available");
            
        }
        VkApplicationInfo appInfo{};
        appInfo.sType=VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.pNext = nullptr;
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
    
       VkInstanceCreateInfo instanceInfo; 
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        
        instanceInfo.flags = 0;
        instanceInfo.pApplicationInfo = &appInfo;
        
        auto extensions = getRequiredExtensions();
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if(enableValidationLayers)
        {
            std::cout<<"enable validation layer selected"<<std::endl;
            instanceInfo.enabledLayerCount=static_cast<uint32_t>(validationLayers.size());
            instanceInfo.ppEnabledLayerNames=validationLayers.data();
            populateDebugMessangerCreateInfo(debugCreateInfo);
            instanceInfo.pNext=(VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else
        {
            instanceInfo.pNext = nullptr;
            instanceInfo.enabledLayerCount=0;
            instanceInfo.ppEnabledLayerNames=nullptr;
        }
        instanceInfo.enabledExtensionCount=static_cast<uint32_t>(extensions.size());
        instanceInfo.ppEnabledExtensionNames=extensions.data();

        VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);
       if (result != VK_SUCCESS) {
            //ASSERT_VULKAN(result);
            throw std::runtime_error("failed to create instance!");
        }

    }
    void populateDebugMessangerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createinfo)
    {                                     
        createinfo = {};
        createinfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createinfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT|
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        createinfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT|
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT|
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createinfo.pfnUserCallback = debugCallback;
        
    }
    bool checkValidationLayerSupport()
    {
    
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount,nullptr);
        std::vector<VkLayerProperties> availableLayer(layerCount);
        
        vkEnumerateInstanceLayerProperties(&layerCount,availableLayer.data());
        for(const char* layername: validationLayers)
        {
            bool layerFound = false;
            for(const auto & layerProperties : availableLayer)
            {
                if(strcmp(layerProperties.layerName,layername)==0)
                {
                    layerFound = true;
                    break;
                }
            }
            if(layerFound == false)
                return false;
        }
        return true;
    }
    std::vector<const char*> getRequiredExtensions()
    {
    
        uint32_t glfwExtensionCount=0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        
        std::vector<const char*> extensions(glfwExtensions,glfwExtensions+glfwExtensionCount);
        
        if(enableValidationLayers)
        {
        
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        return extensions;
        
        
    }
    void setupDebuggerMessanger()
    {
    
        if(!enableValidationLayers) return;
        VkDebugUtilsMessengerCreateInfoEXT createinfo{};
        populateDebugMessangerCreateInfo(createinfo);
        
        if(CreateDebugUtilsMessengerEXT(instance,&createinfo,nullptr,&debugmessanger)!= VK_SUCCESS){
            throw std::runtime_error("failed to setup the debug messanger");
        }
        
        
    }
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        )
        {
            std::cerr<< ""<<pCallbackData->pMessage<<std::endl;
        
            return VK_FALSE;
        }
    
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    
    }

    return EXIT_SUCCESS;
}
