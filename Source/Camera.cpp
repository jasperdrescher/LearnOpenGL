#include "Camera.h"

Camera::Camera()
	: myProjection(0.0f)
	, myView(0.0f)
	, myFoV(45.0f)
	, myYaw(-90.0f)
	, myPitch(0.0f)
	, mySpeed(2.5f)
{
	myPosition = glm::vec3(0.0f, 0.0f, 3.0f);
	myFront = glm::vec3(0.0f, 0.0f, -1.0f);
	myUp = glm::vec3(0.0f, 1.0f, 0.0f);
}
