#include "Camera.h"

#include "InputManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

Camera::Camera()
	: myProjection(0.0f)
	, myView(0.0f)
	, myPosition(0.0f)
	, myFront(0.0f)
	, myRight(0.0f)
	, myUp(0.0f)
	, myDirection(0.0f)
	, myViewport(0.0f)
	, myHorizontalAngle(3.14f)
	, myVerticalAngle(0.0f)
	, myDefaultFieldOfView(45.0f)
	, myFieldOfView(myDefaultFieldOfView)
	, myMouseSpeed(0.01f)
	, myKeySpeed(2.5f)
	, myKeyBoostMultiplier(2.0f)
{
	myProjection = glm::perspective(glm::radians(myFieldOfView), 1280.0f / 720.0f, 0.1f, 100.0f);
	myPosition = glm::vec3(4.0f, 3.0f, 3.0f);
	myFront = glm::vec3(0.0f, 0.0f, -1.0f);
	myRight = glm::vec3(1.0f, 0.0f, 0.0f);
	myUp = glm::vec3(0.0f, 1.0f, 0.0f);
	myView = glm::lookAt(myPosition, myPosition + myFront, myUp);
	myDirection = glm::vec3(0.0f, 0.0f, 0.0f);
}

void Camera::Update(float aDeltaTime)
{
	float movementSpeed = myKeySpeed;

	const InputManager& inputManager = InputManager::GetInstance();
	if (inputManager.IsKeyDown(Keys::LeftShift))
		movementSpeed *= myKeyBoostMultiplier;

	if (inputManager.IsKeyDown(Keys::W))
		myPosition += aDeltaTime * movementSpeed * myFront;

	if (inputManager.IsKeyDown(Keys::A))
		myPosition -= aDeltaTime * movementSpeed * glm::normalize(glm::cross(myFront, myUp));

	if (inputManager.IsKeyDown(Keys::S))
		myPosition -= aDeltaTime * movementSpeed * myFront;

	if (inputManager.IsKeyDown(Keys::D))
		myPosition += aDeltaTime * movementSpeed * glm::normalize(glm::cross(myFront, myUp));

	if (inputManager.IsKeyDown(Keys::Spacebar))
		myPosition += aDeltaTime * movementSpeed * myUp;

	if (inputManager.IsKeyDown(Keys::LeftControl))
		myPosition -= aDeltaTime * movementSpeed * myUp;

	if (inputManager.IsMouseButtonDown(MouseButtons::Right))
	{
		myHorizontalAngle += aDeltaTime * myMouseSpeed * (1280.0f / 2.0f - inputManager.GetCursorXPosition());
		myVerticalAngle += aDeltaTime * myMouseSpeed * (720.0f / 2.0f - inputManager.GetCursorYPosition());
	}

	myDirection = glm::vec3(cos(myVerticalAngle) * sin(myHorizontalAngle), sin(myVerticalAngle), cos(myVerticalAngle) * cos(myHorizontalAngle));
	myRight = glm::vec3(sin(myHorizontalAngle - 3.14f / 2.0f), 0.0f, cos(myHorizontalAngle - 3.14f / 2.0f));
	myUp = glm::cross(myRight, myDirection);
	myFront = glm::normalize(myDirection);
	myFieldOfView = myDefaultFieldOfView - 5.0f * inputManager.GetScrollYOffset();

	myProjection = glm::perspective(glm::radians(myFieldOfView), 1280.0f / 720.0f, 0.1f, 100.0f);
	myView = glm::lookAt(myPosition, myPosition + myDirection, myUp);
}

void Camera::SetViewportSize(const glm::vec2& aSize)
{
	myViewport = glm::vec2(aSize);
}

void Camera::SetPosition(const glm::vec3& aPosition)
{
	myPosition = aPosition;
}
