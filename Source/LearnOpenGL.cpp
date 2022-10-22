#include "Camera.h"
#include "Shader.h"
#include "Model.h"
#include "LogUtility.h"
#include "GLUtility.h"
#include "ModelLoader.h"
#include "TextureLoader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

static void FrameBufferSizeCallback(GLFWwindow* aWindow, int aWidth, int aHeight);
static void CursorCallback(GLFWwindow* aWindow, double aXPosition, double aYPosition);
static void ScrollCallback(GLFWwindow* aWindow, double aXOffset, double aYOffset);
static void ProcessInput(GLFWwindow* aWindow);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
#if _WIN32
    const char* bits = "x86";
#elif _WIN64
    const char* bits = "x64";
#endif
    LogUtility::PrintMessage(LogUtility::LogCategory::Core, "Windows %s", bits);
    LogUtility::PrintMessage(LogUtility::LogCategory::Core, "Current working directory: %s", std::filesystem::current_path().string().c_str());

    glfwSetErrorCallback(GLUtility::GLFWErrorCallback);

    if (glfwInit() == GLFW_FALSE)
    {
        LogUtility::PrintError(LogUtility::LogCategory::GLFW, "Failed to initialize GLFW");
        glfwTerminate();
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (!window)
    {
        LogUtility::PrintError(LogUtility::LogCategory::GLFW, "Failed to create a GLFW window");
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
        LogUtility::PrintError(LogUtility::LogCategory::GLAD, "Failed to initialize GLAD");
        return -1;
    }

    int contextFlags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
    if (contextFlags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLUtility::GLDebugMessageCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader coloredCubeShader;
    coloredCubeShader.Load("Data/Shaders/ColoredCube.vert.glsl", "Data/Shaders/ColoredCube.frag.glsl");

    const std::string objFilepath = "Data/Models/ColoredCube/ColoredCube.obj";

    TextureLoader textureLoader;
    const ModelLoader modelLoader(textureLoader);
    const std::shared_ptr<Model> model = modelLoader.LoadModel(objFilepath);

    for (Mesh& mesh : model->myMeshes)
        mesh.SetupMesh();

    GLUtility::CheckForGLErrors();

    while (!glfwWindowShouldClose(window))
    {
        const auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ProcessInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const glm::mat4 projection = glm::perspective(glm::radians(camera.myZoom), static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 0.1f, 100.0f);
        const glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f)); // Translate it down so it's at the center of the scene
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f)); // It's a bit too big for our scene, so scale it down

        for (Mesh& mesh : model->myMeshes)
            mesh.Draw();

        coloredCubeShader.Use();
        coloredCubeShader.SetMat4("model", modelMatrix);
        coloredCubeShader.SetMat4("projection", projection);
        coloredCubeShader.SetMat4("view", view);

        GLUtility::CheckForGLErrors();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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
