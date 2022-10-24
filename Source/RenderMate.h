#pragma once

#include "Model.h"

#include <memory>

class TextureLoader;
class ModelLoader;
class Shader;
class Camera;
struct GLFWwindow;

class RenderMate
{
public:
	RenderMate();
	~RenderMate();

	void Initialize();
	void Update(float aDeltaTime) const;
	void Destroy() const;

	[[nodiscard]] bool ShouldClose() const;

private:
	void CreateWindow();
	static void CreateContext();
	static void FrameBufferSizeCallback(GLFWwindow* aWindow, int aWidth, int aHeight);
	static void KeyCallback(GLFWwindow* aWindow, int aKey, int aScancode, int anAction, int aMode);
	static void CursorCallback(GLFWwindow* aWindow, double aXPosition, double aYPosition);
	static void ScrollCallback(GLFWwindow* aWindow, double aXOffset, double aYOffset);
	static void MouseButtonCallback(GLFWwindow* aWindow, int aButton, int anAction, int aModifiers);

	std::vector<std::shared_ptr<Model>> myModels;
	GLFWwindow* myWindow;
	Camera* myCamera;
	Shader* myShader;
	TextureLoader* myTextureLoader;
	ModelLoader* myModelLoader;
};
