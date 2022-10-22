#include "RenderMate.h"

#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>

#include "Camera.h"
#include "GLUtility.h"
#include "LogUtility.h"
#include "InputManager.h"
#include "Mesh.h"
#include "ModelLoader.h"
#include "Shader.h"
#include "TextureLoader.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
bool firstMouse = true;
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;

RenderMate::RenderMate()
    : myWindow(nullptr)
    , myCamera(nullptr)
    , myShader(nullptr)
    , myTextureLoader(nullptr)
    , myModelLoader(nullptr)
{
}

RenderMate::~RenderMate()
{
    delete myModelLoader;
    delete myTextureLoader;
    delete myShader;
    delete myCamera;
}

void RenderMate::Initialize()
{
    CreateWindow();
    CreateContext();

    myCamera = new Camera();
    myCamera->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));

    myShader = new Shader();
    myShader->Load("Data/Shaders/ColoredCube.vert.glsl", "Data/Shaders/ColoredCube.frag.glsl");

    const std::string objFilepath = "Data/Models/ColoredCube/ColoredCube.obj";

    myTextureLoader = new TextureLoader();
    myModelLoader = new ModelLoader(*myTextureLoader);
    if (const std::shared_ptr<Model> model = myModelLoader->LoadModel(objFilepath))
        myModels.emplace_back(model);

    for (const std::shared_ptr<Model>& model : myModels)
    {
        for (Mesh& mesh : model->myMeshes)
            mesh.SetupMesh();
    }
}

void RenderMate::Update(float aDeltaTime) const
{
    if (InputManager::GetInstance().IsKeyDown(Keys::Escape))
        glfwSetWindowShouldClose(myWindow, true);

    myCamera->Update(aDeltaTime);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 projection = myCamera->GetProjectionMatrix();
    const glm::mat4 view = myCamera->GetViewMatrix();
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f)); // Translate it down so it's at the center of the scene
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f)); // It's a bit too big for our scene, so scale it down

    for (const std::shared_ptr<Model>& model : myModels)
    {
        for (Mesh& mesh : model->myMeshes)
            mesh.Draw();
    }

    myShader->Use();
    myShader->SetMat4("model", modelMatrix);
    myShader->SetMat4("projection", projection);
    myShader->SetMat4("view", view);

    glfwSwapBuffers(myWindow);
    glfwPollEvents();
}

void RenderMate::Destroy() const
{
    glfwDestroyWindow(myWindow);
    glfwTerminate();
}

bool RenderMate::ShouldClose() const
{
    return glfwWindowShouldClose(myWindow);
}

void RenderMate::CreateWindow()
{
    glfwSetErrorCallback(GLUtility::GLFWErrorCallback);

    if (glfwInit() == GLFW_FALSE)
    {
        LogUtility::PrintError(LogUtility::LogCategory::GLFW, "Failed to initialize GLFW");
        glfwTerminate();
        return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    myWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (!myWindow)
    {
        LogUtility::PrintError(LogUtility::LogCategory::GLFW, "Failed to create a GLFW window");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(myWindow);
    glfwSetWindowUserPointer(myWindow, this);
    glfwSetKeyCallback(myWindow, KeyCallback);
    glfwSetInputMode(myWindow, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetCursorPosCallback(myWindow, CursorCallback);
    glfwSetInputMode(myWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(myWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetScrollCallback(myWindow, ScrollCallback);
    glfwSetMouseButtonCallback(myWindow, MouseButtonCallback);
}

void RenderMate::CreateContext()
{
    if (gladLoadGL() == 0)
    {
        LogUtility::PrintError(LogUtility::LogCategory::GLAD, "Failed to initialize GLAD");
        return;
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
}

void RenderMate::FrameBufferSizeCallback(GLFWwindow* aWindow, int aWidth, int aHeight)
{
    glViewport(0, 0, aWidth, aHeight);
}

void RenderMate::KeyCallback(GLFWwindow* /*aWindow*/, int aKey, int aScancode, int anAction, int aMode)
{
    InputManager::GetInstance().OnKeyAction(aKey, aScancode, anAction != GLFW_RELEASE, aMode);
}

void RenderMate::CursorCallback(GLFWwindow* /*aWindow*/, double aXPosition, double aYPosition)
{
    InputManager::GetInstance().OnCursorAction(aXPosition, aYPosition);
}

void RenderMate::ScrollCallback(GLFWwindow* /*aWindow*/, double aXOffset, double aYOffset)
{
    InputManager::GetInstance().OnScrollAction(aXOffset, aYOffset);
}

void RenderMate::MouseButtonCallback(GLFWwindow* /*aWindow*/, int aButton, int anAction, int aModifiers)
{
    InputManager::GetInstance().OnMouseButtonAction(aButton, anAction, aModifiers);
}
