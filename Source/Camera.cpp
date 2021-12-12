#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
	: myProjection(0.0f)
	, myPosition(0.0f, 0.0f, 3.0f)
	, myForward(0.0f, 0.0f, -1.0f)
	, myRight(0.0f, 0.0f, 0.0f)
	, myUp(0.0f, 1.0f, 0.0f)
	, myWorldUp(0.0, 1.0f, 0.0f)
	, myFoV(45.0f)
	, myYaw(-90.0f)
	, myPitch(0.0f)
	, mySpeed(2.5f)
{}

glm::mat4x4 Camera::GetViewMatrix() const
{
	return glm::lookAt(myPosition, myPosition + myForward, myUp);
}
