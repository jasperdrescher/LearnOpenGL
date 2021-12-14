#pragma once

struct GLFWwindow;

class Window
{
public:
	Window();

	void CreateWindow();
	void Update(const float aDeltaTime);
	void SwapBuffers();
	void OnFrameBufferSizeChange(const int aWidth, const int aHeight);

	bool ShouldWindowClose() const { return myShouldWindowClose; }

private:
	GLFWwindow* myWindow;
	bool myShouldWindowClose;
};
