#include "InputManager.h"

#include "App.h"
#include "Camera.h"

#include <glm/glm.hpp>

InputManager::InputManager()
	: myLastX(800.0f / 2.0f)
	, myLastY(600.0f / 2.0f)
	, myIsFirstCursor(true)
{}

void InputManager::OnCursorAction(double aX, double aY)
{
	const float x = static_cast<float>(aX);
	const float y = static_cast<float>(aY);

	if (myIsFirstCursor)
	{
		myLastX = x;
		myLastY = y;
		myIsFirstCursor = false;
	}

	float xOffset = x - myLastX;
	float yOffset = myLastY - y;
	myLastX = x;
	myLastY = y;

	float sensitivity = 0.1f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	Camera& camera = App::GetInstance().GetCamera();
	camera.myYaw += xOffset;
	camera.myPitch += yOffset;

	if (camera.myPitch > 89.0f)
		camera.myPitch = 89.0f;
	if (camera.myPitch < -89.0f)
		camera.myPitch = -89.0f;

	glm::vec3 direction(0.0f);
	direction.x = cos(glm::radians(camera.myYaw)) * cos(glm::radians(camera.myPitch));
	direction.y = sin(glm::radians(camera.myPitch));
	direction.z = sin(glm::radians(camera.myYaw)) * cos(glm::radians(camera.myPitch));
	camera.myForward = glm::normalize(direction);
	camera.myRight = glm::normalize(glm::cross(camera.myForward, camera.myWorldUp));
	camera.myUp = glm::normalize(glm::cross(camera.myRight, camera.myForward));
}

void InputManager::OnScrollAction(double aXOffset, double aYOffset)
{
	Camera& camera = App::GetInstance().GetCamera();
	camera.myFoV -= static_cast<float>(aYOffset);
	if (camera.myFoV < 1.0f)
		camera.myFoV = 1.0f;
	if (camera.myFoV > 45.0f)
		camera.myFoV = 45.0f;
}
