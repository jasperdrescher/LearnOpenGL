#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMovementDirection
{
    Forward,
    Backward,
    Left,
    Right
};

class Camera
{
public:
    Camera(const glm::vec3& aPosition);
    Camera(const glm::vec3& aPosition, const glm::vec3& aUp, const float aYaw, const float aPitch);

    void ProcessKeyboard(const CameraMovementDirection aDirection, const float aDeltaTime);
    void ProcessMouseMovement(const float aXOffset, const float aYOffset, const GLboolean aConstrainPitch = true);
    void ProcessMouseScroll(const float aYOffset);

    [[nodiscard]] glm::mat4 GetViewMatrix() const;

    glm::vec3 myPosition;
    glm::vec3 myFront;
    glm::vec3 myUp;
    glm::vec3 myRight;
    glm::vec3 myWorldUp;
    float myYaw;
    float myPitch;
    float myMovementSpeed;
    float myMouseSensitivity;
    float myZoom;

private:
    void UpdateCameraVectors();
};
