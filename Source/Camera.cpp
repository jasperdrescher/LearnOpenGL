#include "Camera.h"

#define YAW -90.0f
#define PITCH 0.0f
#define SPEED 2.5f
#define SENSITIVITY 0.1f
#define ZOOM 45.0f

Camera::Camera(const glm::vec3& aPosition)
    : myPosition(aPosition)
    , myFront(glm::vec3(0.0f, 0.0f, -1.0f))
    , myUp(glm::vec3(0.0f, 1.0f, 0.0f))
    , myRight(0.0f)
    , myWorldUp(glm::vec3(0.0f, 1.0f, 0.0f))
    , myYaw(YAW)
    , myPitch(PITCH)
    , myMovementSpeed(SPEED)
    , myMouseSensitivity(SENSITIVITY)
    , myZoom(ZOOM)
{
    UpdateCameraVectors();
}

Camera::Camera(const glm::vec3& aPosition, const glm::vec3& aUp, const float aYaw, const float aPitch)
    : myPosition(aPosition)
    , myFront(glm::vec3(0.0f, 0.0f, -1.0f))
    , myUp(glm::vec3(0.0f, 1.0f, 0.0f))
    , myRight(0.0f)
    , myWorldUp(aUp)
    , myYaw(aYaw)
    , myPitch(aPitch)
    , myMovementSpeed(SPEED)
    , myMouseSensitivity(SENSITIVITY)
    , myZoom(ZOOM)
{
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(myPosition, myPosition + myFront, myUp);
}

void Camera::ProcessKeyboard(const CameraMovementDirection aDirection, const float aDeltaTime)
{
    const float velocity = myMovementSpeed * aDeltaTime;
    if (aDirection == CameraMovementDirection::Forward)
        myPosition += myFront * velocity;
    if (aDirection == CameraMovementDirection::Backward)
        myPosition -= myFront * velocity;
    if (aDirection == CameraMovementDirection::Left)
        myPosition -= myRight * velocity;
    if (aDirection == CameraMovementDirection::Right)
        myPosition += myRight * velocity;
}

void Camera::ProcessMouseMovement(float aXOffset, float aYOffset, GLboolean aConstrainPitch)
{
    aXOffset *= myMouseSensitivity;
    aYOffset *= myMouseSensitivity;

    myYaw += aXOffset;
    myPitch += aYOffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (aConstrainPitch)
    {
        if (myPitch > 89.0f)
            myPitch = 89.0f;
        if (myPitch < -89.0f)
            myPitch = -89.0f;
    }

    // update myFront, myRight and myUp Vectors using the updated Euler angles
    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(const float aYOffset)
{
    myZoom -= aYOffset;
    if (myZoom < 1.0f)
        myZoom = 1.0f;
    if (myZoom > 45.0f)
        myZoom = 45.0f;
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(myYaw)) * cos(glm::radians(myPitch));
    front.y = sin(glm::radians(myPitch));
    front.z = sin(glm::radians(myYaw)) * cos(glm::radians(myPitch));
    myFront = glm::normalize(front);
    myRight = glm::normalize(glm::cross(myFront, myWorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    myUp = glm::normalize(glm::cross(myRight, myFront));
}
