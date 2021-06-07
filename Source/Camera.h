#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct Camera
{
	Camera();

	glm::mat4 myProjection;
	glm::mat4 myView;
	glm::vec3 myPosition;
	glm::vec3 myFront;
	glm::vec3 myUp;
	float myFoV;
	float myYaw;
	float myPitch;
	float mySpeed;
};
