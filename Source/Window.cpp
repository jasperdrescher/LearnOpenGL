#include "Window.h"

#include "App.h"
#include "Camera.h"
#include "InputManager.h"
#include "GlfwCallback.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdexcept>

Window::Window()
	: myWindow(nullptr)
	, myShouldWindowClose(false)
{}

void Window::CreateWindow()
{
	glfwSetErrorCallback(GlfwCallback::ErrorCallback);

	if (!glfwInit())
	{
		printf("Failed to initialize GLFW\n");
		glfwTerminate();
		throw std::runtime_error("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	myWindow = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (!myWindow)
	{
		printf("Failed to create a GLFW window\n");
		glfwTerminate();
		throw std::runtime_error("Failed to create a GLFW window");
	}

	glfwMakeContextCurrent(myWindow);
	glfwSetFramebufferSizeCallback(myWindow, GlfwCallback::FrameBufferSizeCallback);
	glfwSetInputMode(myWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(myWindow, GlfwCallback::CursorCallback);
	glfwSetScrollCallback(myWindow, GlfwCallback::ScrollCallback);
}

void Window::Update(const float aDeltaTime)
{
	myShouldWindowClose = glfwWindowShouldClose(myWindow);
	Camera& camera = App::GetInstance().GetCamera();
	if (glfwGetKey(myWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(myWindow, true);
	if (glfwGetKey(myWindow, GLFW_KEY_1) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(myWindow, GLFW_KEY_2) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (glfwGetKey(myWindow, GLFW_KEY_W) == GLFW_PRESS)
		camera.myPosition += (camera.mySpeed * aDeltaTime) * camera.myForward;
	if (glfwGetKey(myWindow, GLFW_KEY_S) == GLFW_PRESS)
		camera.myPosition -= (camera.mySpeed * aDeltaTime) * camera.myForward;
	if (glfwGetKey(myWindow, GLFW_KEY_A) == GLFW_PRESS)
		camera.myPosition -= glm::normalize(glm::cross(camera.myForward, camera.myUp)) * (camera.mySpeed * aDeltaTime);
	if (glfwGetKey(myWindow, GLFW_KEY_D) == GLFW_PRESS)
		camera.myPosition += glm::normalize(glm::cross(camera.myForward, camera.myUp)) * (camera.mySpeed * aDeltaTime);
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(myWindow);
	glfwPollEvents();
}

void Window::OnFrameBufferSizeChange(const int aWidth, const int aHeight)
{
	glViewport(0, 0, aWidth, aHeight);
}
