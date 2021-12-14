#include "App.h"
#include "Camera.h"
#include "GlError.h"
#include "GlfwCallback.h"
#include "InputManager.h"
#include "MeshManager.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Window.h"

#include <GLFW/glfw3.h>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main()
{
	App& app = App::GetInstance();
	app.Initialize();

	try
	{
		app.GetWindow().CreateWindow();
	}
	catch (const std::runtime_error& aError)
	{
		printf(aError.what());
		return -1;
	}

    if (!gladLoadGL())
    {
		printf("Failed to initialize GLAD\n");
        return -1;
    }

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);
	glDebugMessageCallback(GlError::GlErrorCallback, nullptr);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glViewport(0, 0, 800, 600);

	const std::filesystem::path path = std::filesystem::current_path();
	printf("Current path: %s\n", path.string().c_str());
	const std::string shadersFolder = "Data/Shaders/";

	ShaderProgram lightingShaderProgram;
	lightingShaderProgram.Create(shadersFolder + "LightingCasters.vert.glsl", shadersFolder + "LightingCasters.frag.glsl");

	ShaderProgram cubeShaderProgram;
	cubeShaderProgram.Create(shadersFolder + "FullFragColor.vert.glsl", shadersFolder + "FullFragColor.frag.glsl");

	MeshManager meshManager;
	Mesh* coloredCubeMesh = meshManager.LoadObj("Data/Cube.obj");
	Mesh* cubeMesh = meshManager.LoadObj("Data/Cube.obj");

	unsigned int coloredCubeVAO;
	glGenVertexArrays(1, &coloredCubeVAO);

	unsigned int coloredCubeVBO;
	glGenBuffers(1, &coloredCubeVBO);

	unsigned int coloredCubeEBO;
	glGenBuffers(1, &coloredCubeEBO);

	glBindVertexArray(coloredCubeVAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, coloredCubeVBO);
	glBufferData(GL_ARRAY_BUFFER, coloredCubeMesh->myVertices.size() * sizeof(Vertex), coloredCubeMesh->myVertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, coloredCubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, coloredCubeMesh->myIndices.size() * sizeof(unsigned int), coloredCubeMesh->myIndices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, myPosition)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, myNormal)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, myTextureCoordinates)));
	glEnableVertexAttribArray(2);

	unsigned int lampVAO;
	glGenVertexArrays(1, &lampVAO);

	unsigned int lampVBO;
	glGenBuffers(1, &lampVBO);

	unsigned int lampEBO;
	glGenBuffers(1, &lampEBO);

	glBindVertexArray(lampVAO);

	glBindBuffer(GL_ARRAY_BUFFER, lampVBO);
	glBufferData(GL_ARRAY_BUFFER, cubeMesh->myVertices.size() * sizeof(Vertex), cubeMesh->myVertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lampEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeMesh->myIndices.size() * sizeof(unsigned int), cubeMesh->myIndices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, myPosition)));
	glEnableVertexAttribArray(0);

	ShaderProgram lampShaderProgram;
	lampShaderProgram.Create(shadersFolder + "Lamp.vert.glsl", shadersFolder + "Lamp.frag.glsl");

	GlError::CheckGlError();

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

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	Texture diffuseMap;
	diffuseMap.Create("Data/Container.jpg");

	Texture specularMap;
	specularMap.Create("Data/Container.jpg");

	const float lightingDistance = 2.0f;

	Camera& camera = app.GetCamera();

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

    while (!app.GetWindow().ShouldWindowClose())
    {
		const float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		app.Update(deltaTime);

		camera.myProjection = glm::perspective(glm::radians(camera.myFoV), 800.0f / 600.0f, 0.1f, 100.0f);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cubeShaderProgram.Use();
		cubeShaderProgram.SetMatrix4x4("projection", camera.myProjection);
		cubeShaderProgram.SetMatrix4x4("view", camera.GetViewMatrix());

		glBindVertexArray(coloredCubeVAO);
		for (int cubeIndex = 0; cubeIndex < 10; cubeIndex++)
		{
			glm::mat4 model = glm::mat4(0.5f);
			model = glm::translate(model, cubePositions[cubeIndex]);
			float angle = 20.0f * cubeIndex;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			cubeShaderProgram.SetMatrix4x4("model", model);
			cubeShaderProgram.SetVector3("viewPosition", camera.myPosition);
			cubeShaderProgram.SetInt("material.diffuse", 0);
			cubeShaderProgram.SetInt("material.specular", 1);
			cubeShaderProgram.SetFloat("material.shininess", 32.0f);

			cubeShaderProgram.SetVector3("directionalLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
			cubeShaderProgram.SetVector3("directionalLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
			cubeShaderProgram.SetVector3("directionalLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			cubeShaderProgram.SetVector3("directionalLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

			for (int pointLightIndex = 0; pointLightIndex < 4; pointLightIndex++)
			{
				const std::string index = std::to_string(pointLightIndex);
				cubeShaderProgram.SetVector3("pointLights[" + index + "].position", pointLightPositions[pointLightIndex]);
				cubeShaderProgram.SetVector3("pointLights[" + index + "].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
				cubeShaderProgram.SetVector3("pointLights[" + index + "].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
				cubeShaderProgram.SetVector3("pointLights[" + index + "].specular", glm::vec3(1.0f, 1.0f, 1.0f));
				cubeShaderProgram.SetFloat("pointLights[" + index + "].constant", 1.0f);
				cubeShaderProgram.SetFloat("pointLights[" + index + "].linear", 0.09f);
				cubeShaderProgram.SetFloat("pointLights[" + index + "].quadratic", 0.032f);
			}

			cubeShaderProgram.SetVector3("spotLight.position", camera.myPosition);
			cubeShaderProgram.SetVector3("spotLight.direction", camera.myForward);
			cubeShaderProgram.SetVector3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			cubeShaderProgram.SetVector3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
			cubeShaderProgram.SetVector3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
			cubeShaderProgram.SetFloat("spotLight.constant", 1.0f);
			cubeShaderProgram.SetFloat("spotLight.linear", 0.09f);
			cubeShaderProgram.SetFloat("spotLight.quadratic", 0.032f);
			cubeShaderProgram.SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
			cubeShaderProgram.SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap.GetID());
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specularMap.GetID());

			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(coloredCubeMesh->myIndices.size()), GL_UNSIGNED_INT, static_cast<void*>(0));
		}

		lampShaderProgram.Use();
		lampShaderProgram.SetMatrix4x4("projection", camera.myProjection);
		lampShaderProgram.SetMatrix4x4("view", camera.GetViewMatrix());

		for (int lampIndex = 0; lampIndex < 4; lampIndex++)
		{
			glm::mat4 model(1.0f);
			model = glm::translate(model, pointLightPositions[lampIndex]);
			model = glm::scale(model, glm::vec3(0.2f));
			lampShaderProgram.SetMatrix4x4("model", model);

			glBindVertexArray(lampVAO);
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cubeMesh->myIndices.size()), GL_UNSIGNED_INT, static_cast<void*>(0));
		}

		app.GetWindow().SwapBuffers();

		GlError::CheckGlError();
    }

	glDeleteVertexArrays(1, &coloredCubeVAO);
	glDeleteBuffers(1, &coloredCubeVBO);
	glDeleteBuffers(1, &coloredCubeEBO);
	glDeleteVertexArrays(1, &lampVAO);
	glDeleteBuffers(1, &lampVBO);
	lampShaderProgram.Delete();
	cubeShaderProgram.Delete();
	delete cubeMesh;
	delete coloredCubeMesh;

    glfwTerminate();

    return 0;
}
