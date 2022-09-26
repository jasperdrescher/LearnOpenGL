#include "Camera.h"
#include "Shader.h"
#include "Model.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>

#include <iostream>
#include <unordered_map>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

static void FrameBufferSizeCallback(GLFWwindow* aWindow, int aWidth, int aHeight);
static void CursorCallback(GLFWwindow* aWindow, double aXPosition, double aYPosition);
static void ScrollCallback(GLFWwindow* aWindow, double aXOffset, double aYOffset);
static void ProcessInput(GLFWwindow* aWindow);
unsigned int LoadTexture(const std::string& aFilepath);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

constexpr glm::vec3 cubePositions[] =
{
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f, 2.0f, -2.5f),
    glm::vec3(1.5f, 0.2f, -1.5f),
    glm::vec3(-1.3f, 1.0f, -1.5f)
};

constexpr glm::vec3 pointLightPositions[] =
{
    glm::vec3(0.7f, 0.2f, 2.0f),
    glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f, 2.0f, -12.0f),
    glm::vec3(0.0f, 0.0f, -3.0f)
};

std::string GetNameFromPath(const std::string& aPath)
{
    const std::size_t nameStartIndex = aPath.find_last_of("/\\") + 1;
    const std::size_t extensionStartIndex = aPath.find_last_of(".");
    return aPath.substr(nameStartIndex, aPath.length() - nameStartIndex - (aPath.length() - extensionStartIndex));
}

std::string GetExtensionFromPath(const std::string& aPath)
{
    const std::size_t extensionStartIndex = aPath.find_last_of(".") + 1;
    return aPath.substr(extensionStartIndex, aPath.length() - extensionStartIndex);
}

std::string GetDirectoryFromPath(const std::string& aPath)
{
   const std::size_t lastSlashIndex = aPath.find_last_of("/\\");
    return aPath.substr(0, lastSlashIndex + 1);
}

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
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader lightingShader;
    lightingShader.Load("Data/Shaders/5_4_light_casters.vert.glsl", "Data/Shaders/6_multiple_lights.frag.glsl");

    Shader lightCubeShader;
    lightCubeShader.Load("Data/Shaders/5_4_light_casters.vert.glsl", "Data/Shaders/5_4_light_casters.frag.glsl");

    const std::string path = "Data/Models/MagicCube/MagicCube.obj";
    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warning;
    std::string error;
    const std::string& fileName = GetNameFromPath(path);
    const std::string& directory = GetDirectoryFromPath(path);
    const std::string& fileExtension = GetExtensionFromPath(path);
    if (!tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, path.c_str(), directory.c_str(), true))
        return -1;

    std::cout << "vertices: " << attributes.vertices.size() << std::endl;
    std::cout << "colors: " << attributes.colors.size() << std::endl;
    std::cout << "normals: " << attributes.normals.size() << std::endl;
    std::cout << "texcoords: " << attributes.texcoords.size() << std::endl;

    std::cout << "Shapes: " << shapes.size() << std::endl;
    for (const tinyobj::shape_t& shape : shapes)
    {
        std::cout << "name: " << shape.name << std::endl;
        std::cout << "indices: " << shape.mesh.indices.size() << std::endl;
        std::cout << "material_ids: " << shape.mesh.material_ids.size() << std::endl;
        std::cout << "num_face_vertices: " << shape.mesh.num_face_vertices.size() << std::endl;
    }

    std::cout << "Materials: " << materials.size() << std::endl;
    for (const tinyobj::material_t& material : materials)
    {
        std::cout << "diffuse_texname: " << material.diffuse_texname << std::endl;
    }

    Model model;
    for (const tinyobj::shape_t& shape : shapes)
    {
        Mesh mesh;
        std::unordered_map<glm::vec3, uint32_t> uniqueVertices;
        for (const tinyobj::index_t& index : shape.mesh.indices)
        {
            Vertex vertex;
            size_t vertexIndexStride = 3 * static_cast<size_t>(index.vertex_index);
            vertex.myPosition.x = attributes.vertices[vertexIndexStride];
            vertex.myPosition.y = attributes.vertices[vertexIndexStride + 1];
            vertex.myPosition.z = attributes.vertices[vertexIndexStride + 2];

            if (!attributes.normals.empty())
            {
                const size_t normalIndexStride = 3 * static_cast<size_t>(index.normal_index);
                vertex.myNormal.x = attributes.normals[normalIndexStride];
                vertex.myNormal.y = attributes.normals[normalIndexStride + 1];
                vertex.myNormal.z = attributes.normals[normalIndexStride + 2];
            }

            if (!attributes.texcoords.empty())
            {
                const size_t textureCoordinatesIndexStride = 2 * static_cast<size_t>(index.texcoord_index);
                vertex.myTextureCoordinates.x = attributes.texcoords[textureCoordinatesIndexStride];
                vertex.myTextureCoordinates.y = attributes.texcoords[textureCoordinatesIndexStride + 1];
            }

            if (uniqueVertices.count(vertex.myPosition) == 0)
            {
                uniqueVertices[vertex.myPosition] = static_cast<uint32_t>(mesh.myVertices.size());
                mesh.myVertices.push_back(vertex);
            }

            mesh.myIndices.push_back(uniqueVertices[vertex.myPosition]);
        }

        model.myMeshes.push_back(mesh);
    }

    std::map<std::string, GLuint> textures;
    for (const tinyobj::material_t& material : materials)
    {
        if (!material.diffuse_texname.empty())
        {
            if (textures.find(material.diffuse_texname) == textures.end())
            {
                const unsigned int textureIdentifier = LoadTexture(directory + material.diffuse_texname);
                textures.insert(std::make_pair(material.diffuse_texname, textureIdentifier));
            }
        }
    }

    for (const std::pair<std::string, GLuint> pair : textures)
    {
        Texture texture;
        texture.myPath = pair.first;
        texture.myIdentifier = pair.second;
        texture.myType = "texture_diffuse";
        model.myMeshes[0].myTextures.push_back(texture);
    }

    std::cout << "Textures: " << textures.size() << std::endl;
    std::cout << "Meshes: " << model.myMeshes.size() << std::endl;
    std::cout << "Indices: " << model.myMeshes[0].myIndices.size() << std::endl;
    std::cout << "Vertices: " << model.myMeshes[0].myVertices.size() << std::endl;

    constexpr glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);
    constexpr glm::vec3 green = glm::vec3(0.0f, 1.0f, 0.0f);
    constexpr glm::vec3 blue = glm::vec3(0.0f, 0.0f, 1.0f);
    constexpr glm::vec3 yellow = glm::vec3(1.0f, 1.0f, 0.0f);

    constexpr glm::vec3 pointLightColors[] = {red, green, blue, yellow};

    // first, configure the cube's VAO (and vertexBufferObject)

    for (Mesh& mesh : model.myMeshes)
        mesh.SetupMesh();

    unsigned int vertexBufferObject = model.myMeshes[0].myVertexBufferObject;

   /* glGenVertexArrays(1, &cubeVertexArrayObject);
    glBindVertexArray(cubeVertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, mesh.myVertices.size() * sizeof(Vertex), &mesh.myVertices.front(), GL_STATIC_DRAW);*/

    /*glGenBuffers(1, &elementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.myIndices.size() * sizeof(unsigned int), &mesh.myIndices.front(), GL_STATIC_DRAW);*/

    /*glBindVertexArray(cubeVertexArrayObject);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, myPosition)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, myNormal)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, myTextureCoordinates)));
    glEnableVertexAttribArray(2);*/

    // second, configure the light's VAO (vertexBufferObject stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVertexArrayObject;
    glGenVertexArrays(1, &lightCubeVertexArrayObject);
    glBindVertexArray(lightCubeVertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);

    //const unsigned int diffuseMap = LoadTexture("Data/Textures/Container2.png");
    //const unsigned int specularMap = LoadTexture("Data/Textures/Container2_specular.png");

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
        lightingShader.SetVec3("viewPosition", camera.myPosition);
        lightingShader.SetFloat("material.myShininess", 32.0f);

        lightingShader.SetVec3("directionalLight.myDirection", glm::vec3(-0.2f, -1.0f, -0.3f));
        lightingShader.SetVec3("directionalLight.myAmbient", glm::vec3(0.05f, 0.05f, 0.05f));
        lightingShader.SetVec3("directionalLight.myDiffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        lightingShader.SetVec3("directionalLight.mySpecular", glm::vec3(0.5f, 0.5f, 0.5f));

        for (unsigned int i = 0; i < 4; i++)
        {
            const std::string index = std::to_string(i);
            lightingShader.SetVec3("pointLights[" + index + "].myPosition", pointLightPositions[i]);
            lightingShader.SetVec3("pointLights[" + index + "].myAmbient", pointLightColors[i] * 0.1f);
            lightingShader.SetVec3("pointLights[" + index + "].myDiffuse", pointLightColors[i]);
            lightingShader.SetVec3("pointLights[" + index + "].mySpecular", glm::vec3(1.0f));
            lightingShader.SetFloat("pointLights[" + index + "].myConstant", 1.0f);
            lightingShader.SetFloat("pointLights[" + index + "].myLinear", 0.09f);
            lightingShader.SetFloat("pointLights[" + index + "].myQuadratic", 0.032f);
        }

        lightingShader.SetVec3("spotLight.myPosition", camera.myPosition);
        lightingShader.SetVec3("spotLight.myDirection", camera.myFront);

        lightingShader.SetVec3("spotLight.myAmbient", glm::vec3(0.0f));
        lightingShader.SetVec3("spotLight.myDiffuse", glm::vec3(1.0f));
        lightingShader.SetVec3("spotLight.mySpecular", glm::vec3(1.0f));
        lightingShader.SetFloat("spotLight.myConstant", 1.0f);
        lightingShader.SetFloat("spotLight.myLinear", 0.09f);
        lightingShader.SetFloat("spotLight.myQuadratic", 0.032f);
        lightingShader.SetFloat("spotLight.myCutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.SetFloat("spotLight.myOuterCutOff", glm::cos(glm::radians(15.0f)));

        // view/projection transformations
        const glm::mat4 projection = glm::perspective(glm::radians(camera.myZoom), static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 0.1f, 100.0f);
        const glm::mat4 view = camera.GetViewMatrix();
        lightingShader.SetMat4("projection", projection);
        lightingShader.SetMat4("view", view);

        // world transformation
        //const glm::mat4 lightingModelMatrix = glm::mat4(1.0f);
        //lightingShader.SetMat4("model", lightingModelMatrix);

        // bind diffuse map
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, specularMap);

        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        lightingShader.SetMat4("model", modelMatrix);
        for (Mesh& mesh : model.myMeshes)
            mesh.Draw(lightingShader);

        // render containers
        //glBindVertexArray(cubeVertexArrayObject);
        /*for (const Mesh& mesh : model.myMeshes)
        {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, cubePositions[0]);
            constexpr float angle = 20.0f * static_cast<float>(1.0f);
            modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.SetMat4("model", modelMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }*/
        //for (unsigned int i = 0; i < 10; i++)
        //{
        //    // calculate the model matrix for each object and pass it to shader before drawing
        //    glm::mat4 modelMatrix = glm::mat4(1.0f);
        //    modelMatrix = glm::translate(modelMatrix, cubePositions[i]);
        //    const float angle = 20.0f * static_cast<float>(i);
        //    modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        //    lightingShader.SetMat4("model", modelMatrix);

        //    glBindTexture(GL_TEXTURE_2D, mesh.myTextures[2].myIdentifier);
        //      //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
        //    //glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.myIndices.size()), GL_UNSIGNED_INT, nullptr);
        //    glDrawArrays(GL_TRIANGLES, 0, 36);
        //}

        lightCubeShader.Use();
        lightCubeShader.SetMat4("projection", projection);
        lightCubeShader.SetMat4("view", view);

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVertexArrayObject);
        for (unsigned int i = 0; i < 4; i++)
        {
            glm::mat4 lightCubeModelMatrix = glm::mat4(1.0f);
            lightCubeModelMatrix = glm::translate(lightCubeModelMatrix, pointLightPositions[i]);
            lightCubeModelMatrix = glm::scale(lightCubeModelMatrix, glm::vec3(0.2f)); // Make it a smaller cube
            lightCubeShader.SetMat4("model", lightCubeModelMatrix);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //glDeleteVertexArrays(1, &cubeVertexArrayObject);
    glDeleteVertexArrays(1, &lightCubeVertexArrayObject);
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

unsigned int LoadTexture(const std::string& aFilepath)
{
    unsigned int textureID = 0;
    glGenTextures(1, &textureID);

    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* data = stbi_load(aFilepath.c_str(), &width, &height, &channels, 0);
    if (!data)
    {
        std::cout << "Texture failed to load at path: " << aFilepath << std::endl;
        stbi_image_free(data);

        return textureID;
    }

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

    return textureID;
}
