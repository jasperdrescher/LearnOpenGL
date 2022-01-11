#include "Camera.h"
#include "Shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

static void FrameBufferSizeCallback(GLFWwindow* aWindow, int aWidth, int aHeight);
static void CursorCallback(GLFWwindow* aWindow, double aXPosition, double aYPosition);
static void ScrollCallback(GLFWwindow* aWindow, double aXOffset, double aYOffset);
static void ProcessInput(GLFWwindow* aWindow);
static unsigned int LoadTexture(const char* aFilepath);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    if (glfwInit() == GLFW_FALSE)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
    glfwSetCursorPosCallback(window, CursorCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (gladLoadGL() == 0)
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    
    Shader lightingShader;
	lightingShader.Load("Data/Shaders/5_4_light_casters.vert.glsl", "Data/Shaders/5_4_light_casters.frag.glsl");
    
    constexpr float vertices[] =
    {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    constexpr glm::vec3 cubePositions[] =
    {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // first, configure the cube's VAO (and vertexBufferObject)
    unsigned int vertexBufferObject, cubeVertexArrayObject;
    glGenVertexArrays(1, &cubeVertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVertexArrayObject);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // second, configure the light's VAO (vertexBufferObject stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightcubeVertexArrayObject;
    glGenVertexArrays(1, &lightcubeVertexArrayObject);
    glBindVertexArray(lightcubeVertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);

    const unsigned int diffuseMap = LoadTexture("Data/Textures/Container2.png");
    const unsigned int specularMap = LoadTexture("Data/Textures/Container2_specular.png");

    lightingShader.Use();
    lightingShader.SetInt("material.myDiffuse", 0);
    lightingShader.SetInt("material.mySpecular", 1);

    while (!glfwWindowShouldClose(window))
    {
        const auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ProcessInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.Use();
        lightingShader.SetVec3("light.myPosition", camera.myPosition);
        lightingShader.SetVec3("light.myDirection", camera.myFront);
        lightingShader.SetFloat("light.myCutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.SetFloat("light.myOuterCutOff", glm::cos(glm::radians(17.5f)));
        lightingShader.SetVec3("viewPosition", camera.myPosition);

        // light properties
        lightingShader.SetVec3("light.myAmbient", 0.1f, 0.1f, 0.1f);
        // we configure the diffuse intensity slightly higher; the right lighting conditions differ with each lighting method and environment.
        // each environment and lighting type requires some tweaking to get the best out of your environment.
        lightingShader.SetVec3("light.myDiffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.SetVec3("light.mySpecular", 1.0f, 1.0f, 1.0f);
        lightingShader.SetFloat("light.myConstant", 1.0f);
        lightingShader.SetFloat("light.myLinear", 0.09f);
        lightingShader.SetFloat("light.myQuadratic", 0.032f);

        // material properties
        lightingShader.SetFloat("material.myShininess", 32.0f);

        // view/projection transformations
        const glm::mat4 projection = glm::perspective(glm::radians(camera.myZoom), static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 0.1f, 100.0f);
        const glm::mat4 view = camera.GetViewMatrix();
        lightingShader.SetMat4("projection", projection);
        lightingShader.SetMat4("view", view);

        // world transformation
        const glm::mat4 lightingModelMatrix = glm::mat4(1.0f);
        lightingShader.SetMat4("model", lightingModelMatrix);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // render containers
        glBindVertexArray(cubeVertexArrayObject);
        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, cubePositions[i]);
            const float angle = 20.0f * static_cast<float>(i);
            modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.SetMat4("model", modelMatrix);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &cubeVertexArrayObject);
    glDeleteVertexArrays(1, &lightcubeVertexArrayObject);
    glDeleteBuffers(1, &vertexBufferObject);
    
    glfwTerminate();

    return 0;
}

void ProcessInput(GLFWwindow* aWindow)
{
    if (glfwGetKey(aWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(aWindow, true);
    if (glfwGetKey(aWindow, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovementDirection::Forward, deltaTime);
    if (glfwGetKey(aWindow, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovementDirection::Backward, deltaTime);
    if (glfwGetKey(aWindow, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovementDirection::Left, deltaTime);
    if (glfwGetKey(aWindow, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovementDirection::Right, deltaTime);
}

void FrameBufferSizeCallback(GLFWwindow* aWindow, int aWidth, int aHeight)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, aWidth, aHeight);
}

void CursorCallback(GLFWwindow* /*aWindow*/, double aXPosition, double aYPosition)
{
    const auto xPosition = static_cast<float>(aXPosition);
    const auto yPosition = static_cast<float>(aYPosition);

    if (firstMouse)
    {
        lastX = xPosition;
        lastY = yPosition;
        firstMouse = false;
    }

    const float xOffset = xPosition - lastX;
    const float yOffset = lastY - yPosition; // reversed since y-coordinates go from bottom to top

    lastX = xPosition;
    lastY = yPosition;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

void ScrollCallback(GLFWwindow* /*aWindow*/, double /*aXOffset*/, double aYOffset)
{
    camera.ProcessMouseScroll(static_cast<float>(aYOffset));
}

unsigned int LoadTexture(char const* aFilepath)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width = 0;
    int height = 0;
	int channels = 0;
    if (unsigned char* data = stbi_load(aFilepath, &width, &height, &channels, 0))
    {
        GLenum format = GL_RED;
	    switch (channels)
	    {
            case 1:
            {
                format = GL_RED;
                break;
            }
            case 3:
            {
                format = GL_RGB;
                break;
            }
            case 4:
            {
                format = GL_RGBA;
                break;
            }
			default:
            {
	            std::cout << "Failed to map the right number of channels: " << channels << std::endl;
                break;
            }
	    }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << aFilepath << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
