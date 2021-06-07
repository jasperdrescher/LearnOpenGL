#include "InputManager.h"

#include "Camera.h"

#include <glm/glm.hpp>

InputManager::InputManager()
	: myCamera(nullptr)
	, myLastX(800.0f / 2.0)
	, myLastY(600.0f / 2.0)
	, myIsFirstCursor(true)
{

}

void InputManager::AssignCamera(Camera* aCamera)
{
	myCamera = aCamera;
}

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

	myCamera->myYaw += xOffset;
	myCamera->myPitch += yOffset;

	if (myCamera->myPitch > 89.0f)
		myCamera->myPitch = 89.0f;
	if (myCamera->myPitch < -89.0f)
		myCamera->myPitch = -89.0f;

	glm::vec3 direction(0.0f);
	direction.x = cos(glm::radians(myCamera->myYaw)) * cos(glm::radians(myCamera->myPitch));
	direction.y = sin(glm::radians(myCamera->myPitch));
	direction.z = sin(glm::radians(myCamera->myYaw)) * cos(glm::radians(myCamera->myPitch));
	myCamera->myForward = glm::normalize(direction);
	myCamera->myRight = glm::normalize(glm::cross(myCamera->myForward, myCamera->myWorldUp));
	myCamera->myUp = glm::normalize(glm::cross(myCamera->myRight, myCamera->myForward));
}

void InputManager::OnScrollAction(double aXOffset, double aYOffset)
{
	myCamera->myFoV -= static_cast<float>(aYOffset);
	if (myCamera->myFoV < 1.0f)
		myCamera->myFoV = 1.0f;
	if (myCamera->myFoV > 45.0f)
		myCamera->myFoV = 45.0f;
}
