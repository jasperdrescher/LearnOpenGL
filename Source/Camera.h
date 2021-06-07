#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct Camera
{
	Camera();

	glm::mat4x4 GetViewMatrix() const;

	glm::mat4 myProjection;
	glm::vec3 myPosition;
	glm::vec3 myForward;
	glm::vec3 myRight;
	glm::vec3 myUp;
	glm::vec3 myWorldUp;
	float myFoV;
	float myYaw;
	float myPitch;
	float mySpeed;
};
