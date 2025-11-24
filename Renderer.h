#ifndef RENDERER_H
#define RENDERER_H

#include <QWindow>
#include <vulkan/vulkan_core.h>
#include <string>
#include <vector>
#include "Vertex.h"

//Forward declarations
struct SwapChainSupportDetails;
struct QueueFamilyIndices;

// New struct to hold per-entity rendering data
struct EntityRenderData
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
    uint32_t mipLevels;

    uint32_t entityID;  // To track which entity this belongs to

};
struct TerrainHit
{
    uint32_t triIndex;   // first index of the triangle (i0)
    float height;
    glm::vec3 normal;
    bool hit;
};


class Renderer : public QWindow
{
    Q_OBJECT
public:
    explicit Renderer(QWindow* parent = nullptr);
    ~Renderer();

    void initVulkan();
    void drawFrame();

    EntityRenderData* getFirstTerrainEntity();
    TerrainHit findTriangleUnderBall(const EntityRenderData& terrain,
                                     const glm::vec3& ballPos);
    glm::mat4 getWorldMatrixForEntity(short entityID);

    TerrainHit findTriangleUnderBallWithHint(const EntityRenderData& terrain,
                                             const glm::vec3& ballPos,
                                             int hintTriIndex);
    void setTraceCurve(const std::vector<glm::vec3>& points);

protected:
    //Qt event handlers - called when requestUpdate(); is called
    void exposeEvent(QExposeEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool event(QEvent* event) override;

private:
    // CPU-side trace data
    std::vector<glm::vec3> tracePoints;
    uint32_t traceVertexCount = 0;

    // GPU-side trace buffer
    VkBuffer traceVertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory traceVertexBufferMemory = VK_NULL_HANDLE;

    // Simple line pipeline for the trace
    VkPipeline tracePipeline = VK_NULL_HANDLE;

    void createOrResizeTraceBuffer(size_t pointCount);
    void updateTraceBuffer();
    void createTracePipeline();


    //Dynamic UBO stuff
    VkDeviceSize dynamicAlignment;
    std::vector<glm::mat4> modelMatrices;
    int numObjects = 3;

    //Instanced rendering stuff
    std::vector<VkBuffer> storageBuffers;
    std::vector<VkDeviceMemory> storageBuffersMemory;
    int numInstances;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    VkDevice device;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkCommandPool commandPool;

    VkImage colorImage;
    VkDeviceMemory colorImageMemory;
    VkImageView colorImageView;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    //we store multiple entities' render data
    std::vector<EntityRenderData> entityRenderData;


    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;

    bool framebufferResized = false;

    // ---- Functions ----
    void cleanupSwapChain();
    void cleanup();
    void recreateSwapChain();
    void createInstance();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createStorageBuffers();
    void createCommandPool();
    void createColorResources();
    void createDepthResources();
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat();
    bool hasStencilComponent(VkFormat format);

    //Now takes EntityRenderData reference and entity ID
    void createTextureImage(EntityRenderData& entityData, const std::string& path);
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    VkSampleCountFlagBits getMaxUsableSampleCount();
    void createTextureImageView(EntityRenderData& entityData);
    void createTextureSampler(EntityRenderData& entityData);

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
                     VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                     VkImage& image, VkDeviceMemory& imageMemory);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    void loadEntities(); //Combined loading function
    void loadModel(EntityRenderData& entityData, const std::string& path);
    void createVertexBuffer(EntityRenderData& entityData);
    void createIndexBuffer(EntityRenderData& entityData);

        bool sampleTerrainTriangle(const EntityRenderData& terrain,
                                   uint32_t firstIndex,     // first index of this triangle in terrain.indices
                                   const glm::vec2& xzPos,
                                   float& outHeight,
                                   glm::vec3& outNormal);

    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createCommandBuffers();
    void recordCommandBuffer(uint32_t imageIndex);
    void createSyncObjects();
    void updateUniformBuffer(uint32_t currentImage, glm::mat4x4 proj, glm::mat4x4 view);
    VkShaderModule createShaderModule(const std::vector<char>& code);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    std::vector<const char*> getRequiredExtensions();
    bool checkValidationLayerSupport();
    static std::vector<char> readFile(const std::string& filename);
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};


#endif // RENDERER_H
