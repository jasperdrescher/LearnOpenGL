#include "Camera.h"
#include "InputManager.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

void GlfwErrorCallback(int aError, const char* aDescription)
{
	printf("%i %s\n", aError, aDescription);
}

static inline void GLAPIENTRY GlErrorCallback(GLenum aSource, GLenum aType, GLuint aID, GLenum aSeverity, GLsizei /*aLength*/, const GLchar* aMessage, const void* /*aUserParam*/)
{
	std::string source;
	std::string type;
	std::string severity;

	switch (aSource)
	{
		case GL_DEBUG_SOURCE_API:
			source = "API";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			source = "WINDOW SYSTEM";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			source = "SHADER COMPILER";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			source = "THIRD PARTY";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			source = "APPLICATION";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			source = "OTHER";
			break;
		default:
			source = "UNKNOWN";
			break;
	}

	switch (aType)
	{
		case GL_DEBUG_TYPE_ERROR:
			type = "ERROR";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			type = "DEPRECATED BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			type = "UDEFINED BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			type = "PORTABILITY";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			type = "PERFORMANCE";
			break;
		case GL_DEBUG_TYPE_OTHER:
			type = "OTHER";
			break;
		case GL_DEBUG_TYPE_MARKER:
			type = "MARKER";
			break;
		default:
			type = "UNKNOWN";
			break;
	}

	switch (aSeverity)
	{
		case GL_DEBUG_SEVERITY_HIGH:
			severity = "HIGH";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			severity = "MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			severity = "LOW";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			severity = "NOTIFICATION";
			break;
		default:
			severity = "UNKNOWN";
			break;
	}

	printf("Severity: %s Type: %s Source: %s Id: 0x%x", severity.c_str(), type.c_str(), source.c_str(), aID);
	printf(aMessage);
}

static inline void CheckGlError(GLFWwindow* aWindow)
{
	while (true)
	{
		const GLenum error = glGetError();
		if (error == GL_NO_ERROR)
			return;

		const char* errorString;
		switch (error)
		{
			case GL_NO_ERROR:
				errorString = "No error";
				break;
			case GL_INVALID_ENUM:
				errorString = "Invalid enum";
				break;
			case GL_INVALID_VALUE:
				errorString = "Invalid value";
				break;
			case GL_INVALID_OPERATION:
				errorString = "Invalid operation";
				break;
			case GL_STACK_OVERFLOW:
				errorString = "Stack overflow";
				break;
			case GL_STACK_UNDERFLOW:
				errorString = "Stack underflow";
				break;
			case GL_OUT_OF_MEMORY:
				errorString = "Out of memory";
				break;
			default:
				errorString = "Unknown error";
				break;
		}

		printf(errorString);
	}
}

void FrameBufferSizeCallback(GLFWwindow* /*aWindow*/, int aWidth, int aHeight)
{
    glViewport(0, 0, aWidth, aHeight);
}

void CursorCallback(GLFWwindow* /*aWindow*/, double aX, double aY)
{
	InputManager::GetInstance().OnCursorAction(aX, aY);
}

void ScrollCallback(GLFWwindow* /*aWindow*/, double aXOffset, double aYOffset)
{
	InputManager::GetInstance().OnScrollAction(aXOffset, aYOffset);
}

void ProcessInput(GLFWwindow* aWindow, Camera& aCamera, const float aDeltaTime)
{
    if (glfwGetKey(aWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(aWindow, true);
	if (glfwGetKey(aWindow, GLFW_KEY_1) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(aWindow, GLFW_KEY_2) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (glfwGetKey(aWindow, GLFW_KEY_W) == GLFW_PRESS)
		aCamera.myPosition += (aCamera.mySpeed * aDeltaTime) * aCamera.myForward;
	if (glfwGetKey(aWindow, GLFW_KEY_S) == GLFW_PRESS)
		aCamera.myPosition -= (aCamera.mySpeed * aDeltaTime) * aCamera.myForward;
	if (glfwGetKey(aWindow, GLFW_KEY_A) == GLFW_PRESS)
		aCamera.myPosition -= glm::normalize(glm::cross(aCamera.myForward, aCamera.myUp)) * (aCamera.mySpeed * aDeltaTime);
	if (glfwGetKey(aWindow, GLFW_KEY_D) == GLFW_PRESS)
		aCamera.myPosition += glm::normalize(glm::cross(aCamera.myForward, aCamera.myUp)) * (aCamera.mySpeed * aDeltaTime);
}

int main()
{
    glfwSetErrorCallback(GlfwErrorCallback);

    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create a GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL())
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);
	glDebugMessageCallback(GlErrorCallback, nullptr);

	glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, CursorCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	ShaderProgram defaultShaderProgram;
	defaultShaderProgram.Create("Data/Default.vert.glsl", "Data/Default.frag.glsl");

	float vertices[] = {
		// positions          // colors           // texture coordinates
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};

	unsigned int indices[] = {
	   0, 1, 3, // first triangle
	   1, 2, 3  // second triangle
	};

	float LampVertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(0));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int lampVAO;
	glGenVertexArrays(1, &lampVAO);

	unsigned int lampVBO;
	glGenBuffers(1, &lampVBO);

	glBindVertexArray(lampVAO);

	glBindBuffer(GL_ARRAY_BUFFER, lampVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(LampVertices), LampVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
	glEnableVertexAttribArray(0);

	Texture containerTexture;
	containerTexture.Create("Data/Container.jpg");

	Texture awesomeTexture;
	awesomeTexture.Create("Data/Awesome.png");

	defaultShaderProgram.Use();
	defaultShaderProgram.SetInt("texture1", 0);
	defaultShaderProgram.SetInt("texture2", 1);

	ShaderProgram lightCubeShaderProgram;
	lightCubeShaderProgram.Create("Data/Lamp.vert.glsl", "Data/Lamp.frag.glsl");

	CheckGlError(window);

	Camera camera;
	InputManager::GetInstance().AssignCamera(&camera);

	glm::vec3 cubePositions[] = {
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

	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
		const float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        ProcessInput(window, camera, deltaTime);

		camera.myProjection = glm::perspective(glm::radians(camera.myFoV), 800.0f / 600.0f, 0.1f, 100.0f);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, containerTexture.GetID());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, awesomeTexture.GetID());

		defaultShaderProgram.Use();
		defaultShaderProgram.SetMatrix4x4("projection", camera.myProjection);
		defaultShaderProgram.SetMatrix4x4("view", camera.GetViewMatrix());
		defaultShaderProgram.SetVector3("lightColor", lightColor);

		glBindVertexArray(VAO);
		for (int index = 0; index < 10; index++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[index]);
			float angle = 20.0f * index;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			defaultShaderProgram.SetMatrix4x4("model", model);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		lightCubeShaderProgram.Use();
		lightCubeShaderProgram.SetMatrix4x4("projection", camera.myProjection);
		lightCubeShaderProgram.SetMatrix4x4("view", camera.GetViewMatrix());
		glm::mat4 model(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lightCubeShaderProgram.SetMatrix4x4("model", model);

		glBindVertexArray(lampVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();

		CheckGlError(window);
    }

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	defaultShaderProgram.Delete();

    glfwTerminate();

    return 0;
}
