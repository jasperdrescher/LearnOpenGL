#include "Camera.h"
#include "InputManager.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <filesystem>
#include <iostream>
#include <unordered_map>

void GlfwErrorCallback(int aError, const char* aDescription)
{
	printf("%i %s\n", aError, aDescription);
}

bool DoesFileExist(const std::string& aPath)
{
	std::filesystem::file_status fileStatus;
	std::filesystem::path test = std::filesystem::current_path();
	return std::filesystem::status_known(fileStatus) ? std::filesystem::exists(fileStatus) : std::filesystem::exists(aPath);
}

std::string ReadFile(const std::string& aPath)
{
	std::string sourceResult;
	std::ifstream sourceStream(aPath, std::ios::in);

	if (!sourceStream.is_open())
	{
		printf("File can't be read: %s", aPath.c_str());
		return "";
	}

	std::stringstream sourceStringStream;
	sourceStringStream << sourceStream.rdbuf();
	sourceResult = sourceStringStream.str();
	sourceStream.close();

	return sourceResult;
}

std::string GetNameFromPath(const std::string& aPath)
{
	const std::size_t& nameStartIndex = aPath.find_last_of("/\\") + 1;
	const std::size_t& extensionStartIndex = aPath.find_last_of(".");
	return aPath.substr(nameStartIndex, aPath.length() - nameStartIndex - (aPath.length() - extensionStartIndex));
}

std::string GetExtensionFromPath(const std::string& aPath)
{
	const std::size_t& extensionStartIndex = aPath.find_last_of(".") + 1;
	return aPath.substr(extensionStartIndex, aPath.length() - extensionStartIndex);
}

std::string GetDirectoryFromPath(const std::string& aPath)
{
	std::size_t lastSlashIndex = aPath.find_last_of("/\\");
	return aPath.substr(0, lastSlashIndex + 1);
}

struct Vertex
{
	glm::vec3 myPosition;
	glm::vec3 myNormal;
	//glm::vec2 myTextureCoordinates;

	bool operator==(const Vertex& aOther) const
	{
		return myPosition == aOther.myPosition;
		//return myPosition == aOther.myPosition && myTextureCoordinates == aOther.myTextureCoordinates;
	}
};

inline void CombineHash(std::size_t& aSeed) {}

template <typename T, typename... Rest>
inline void CombineHash(std::size_t& aSeed, const T& aValue, Rest... aRest)
{
	std::hash<T> hasher;
	aSeed ^= hasher(aValue) + 0x9e3779b9 + (aSeed << 6) + (aSeed >> 2);
	CombineHash(aSeed, aRest...);
}

namespace std
{
	template<> struct hash<Vertex>
	{
		size_t operator()(Vertex const& aVertex) const
		{
			size_t seed = 0;
			CombineHash(seed, aVertex.myPosition);
			return seed;
		}
	};
}

struct Mesh
{
	std::vector<Vertex> myVertices;
	std::vector<unsigned int> myIndices;
};

Mesh* LoadObj(const std::string& aFilePath)
{
	tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warning;
	std::string error;

	const std::string& fileName = GetNameFromPath(aFilePath);
	const std::string& directory = GetDirectoryFromPath(aFilePath);
	const std::string& fileExtension = GetExtensionFromPath(aFilePath);

	if (!tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, aFilePath.c_str(), directory.c_str(), true))
	{
		printf("Failed to load %s", aFilePath.c_str());

		return nullptr;
	}

	if (!warning.empty())
	{
		printf("%s", warning.c_str());
	}

	if (!error.empty())
	{
		printf("%s", error.c_str());
	}

	std::unordered_map<Vertex, unsigned int> uniqueVertices = {};

	for (const tinyobj::shape_t& shape : shapes)
	{
		Mesh* mesh = new Mesh();

		for (const tinyobj::index_t& index : shape.mesh.indices)
		{
			Vertex vertex;
			const size_t vertexIndexStride = 3 * static_cast<size_t>(index.vertex_index);
			vertex.myPosition.x = attributes.vertices[vertexIndexStride + 0];
			vertex.myPosition.y = attributes.vertices[vertexIndexStride + 1];
			vertex.myPosition.z = attributes.vertices[vertexIndexStride + 2];

			if (attributes.normals.size() > 0)
			{
				vertex.myNormal.x = attributes.normals[vertexIndexStride + 0];
				vertex.myNormal.y = attributes.normals[vertexIndexStride + 1];
				vertex.myNormal.z = attributes.normals[vertexIndexStride + 2];
			}

			/*if (attributes.texcoords.size() > 0)
			{
				const size_t textureCoordinatesIndexStride = 2 * static_cast<size_t>(index.texcoord_index);
				vertex.myTextureCoordinates.x = attributes.texcoords[textureCoordinatesIndexStride + 0];
				vertex.myTextureCoordinates.y = attributes.texcoords[textureCoordinatesIndexStride + 1];
			}*/

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<unsigned int>(mesh->myVertices.size());
				mesh->myVertices.push_back(vertex);
			}

			mesh->myIndices.push_back(uniqueVertices[vertex]);
		}

		return mesh;
	}

	return nullptr;
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
	defaultShaderProgram.Create("Data/Colored.vert.glsl", "Data/Colored.frag.glsl");

	Mesh* mesh = LoadObj("Data/Cube.obj");

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
	glBufferData(GL_ARRAY_BUFFER, mesh->myVertices.size() * sizeof(Vertex), &mesh->myVertices.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->myIndices.size() * sizeof(unsigned int), &mesh->myIndices.front(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, myPosition)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, myNormal)));
	glEnableVertexAttribArray(1);

	/*glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, myTextureCoordinates)));
	glEnableVertexAttribArray(2);*/

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
		
		/*glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, containerTexture.GetID());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, awesomeTexture.GetID());*/

		defaultShaderProgram.Use();
		defaultShaderProgram.SetMatrix4x4("projection", camera.myProjection);
		defaultShaderProgram.SetMatrix4x4("view", camera.GetViewMatrix());
		//defaultShaderProgram.SetVector3("lightColor", lightColor);

		glBindVertexArray(VAO);
		for (int index = 0; index < 10; index++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[index]);
			float angle = 20.0f * index;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			defaultShaderProgram.SetMatrix4x4("model", model);

			glDrawElements(GL_TRIANGLES, mesh->myIndices.size(), GL_UNSIGNED_INT, 0);
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
