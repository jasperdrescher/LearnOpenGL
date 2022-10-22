#pragma once

class RenderMate;

class App
{
public:
	App();
	~App();

	void Initialize();
	void Run();
	void Destroy() const;

private:
	RenderMate* myRenderMate;
	bool myShouldDestroy;
};
