#include "Camera.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

void GlfwErrorCallback(int anError, const char* aDescription)
{
	printf("%i %s\n", anError, aDescription);
}

static inline void CheckGLError(GLFWwindow* aWindow)
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
		glfwSetWindowShouldClose(aWindow, true);
	}
}

void FrameBufferSizeCallback(GLFWwindow* /*aWindow*/, int aWidth, int aHeight)
{
    glViewport(0, 0, aWidth, aHeight);
}

void ProcessInput(GLFWwindow* aWindow, Camera& aCamera)
{
    if (glfwGetKey(aWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(aWindow, true);
	if (glfwGetKey(aWindow, GLFW_KEY_1) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(aWindow, GLFW_KEY_2) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (glfwGetKey(aWindow, GLFW_KEY_W) == GLFW_PRESS)
		aCamera.myPosition += aCamera.mySpeed * aCamera.myFront;
	if (glfwGetKey(aWindow, GLFW_KEY_S) == GLFW_PRESS)
		aCamera.myPosition -= aCamera.mySpeed * aCamera.myFront;
	if (glfwGetKey(aWindow, GLFW_KEY_A) == GLFW_PRESS)
		aCamera.myPosition -= glm::normalize(glm::cross(aCamera.myFront, aCamera.myUp)) * aCamera.mySpeed;
	if (glfwGetKey(aWindow, GLFW_KEY_D) == GLFW_PRESS)
		aCamera.myPosition += glm::normalize(glm::cross(aCamera.myFront, aCamera.myUp)) * aCamera.mySpeed;
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
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

	glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

	ShaderProgram shaderProgram;
	shaderProgram.Create("Data/DefaultVertexShader.vert.glsl", "Data/DefaultFragmentShader.frag.glsl");

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

	Texture containerTexture;
	containerTexture.Create("Data/Container.jpg");

	Texture awesomeTexture;
	awesomeTexture.Create("Data/Awesome.png");

	shaderProgram.Use();
	shaderProgram.SetInt("texture1", 0);
	shaderProgram.SetInt("texture2", 1);

	CheckGLError(window);

	Camera camera;
	camera.mySpeed = 0.002f;
	camera.myView = glm::translate(camera.myView, glm::vec3(0.0f, 0.0f, -3.0f));
	camera.myProjection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	camera.myPosition = glm::vec3(0.0f, 0.0f, 3.0f);
	camera.myFront = glm::vec3(0.0f, 0.0f, -1.0f);
	camera.myUp = glm::vec3(0.0f, 1.0f, 0.0f);

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

    while (!glfwWindowShouldClose(window))
    {
        ProcessInput(window, camera);

		camera.myView = glm::lookAt(camera.myPosition, camera.myPosition + camera.myFront, camera.myUp);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, containerTexture.GetID());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, awesomeTexture.GetID());

		shaderProgram.Use();
		shaderProgram.SetMatrix4x4("view", camera.myView);
		shaderProgram.SetMatrix4x4("projection", camera.myProjection);

		glBindVertexArray(VAO);
		for (int index = 0; index < 10; index++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[index]);
			float angle = 20.0f * index;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			shaderProgram.SetMatrix4x4("model", model);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

        glfwSwapBuffers(window);
        glfwPollEvents();

		CheckGLError(window);
    }

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	shaderProgram.Delete();

    glfwTerminate();

    return 0;
}
