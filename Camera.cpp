#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        //TODO
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;

        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUp));
        this->cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));

    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        //TODO
        return glm::lookAt(
            cameraPosition,
            cameraPosition + cameraFrontDirection,
            cameraUpDirection
        );
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        if (direction == MOVE_FORWARD)
            cameraPosition += speed * cameraFrontDirection;

        if (direction == MOVE_BACKWARD)
            cameraPosition -= speed * cameraFrontDirection;

        if (direction == MOVE_RIGHT)
            cameraPosition += speed * cameraRightDirection;

        if (direction == MOVE_LEFT)
            cameraPosition -= speed * cameraRightDirection;
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {

        // rotim directia de privire in jurul axei Y (yaw)
        glm::mat4 yawRotation = glm::rotate(glm::mat4(1.0f),
            glm::radians(yaw),
            glm::vec3(0.0f, 1.0f, 0.0f));

        cameraFrontDirection = glm::vec3(yawRotation * glm::vec4(cameraFrontDirection, 0.0f));

        // axa locala right pentru pitch
        glm::vec3 right = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));

        glm::mat4 pitchRotation = glm::rotate(glm::mat4(1.0f),
            glm::radians(pitch),
            right);

        cameraFrontDirection = glm::vec3(pitchRotation * glm::vec4(cameraFrontDirection, 0.0f));

        cameraFrontDirection = glm::normalize(cameraFrontDirection);

        // recalculam axele
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }

    void Camera::reset(glm::vec3 position, glm::vec3 target) {
        cameraPosition = position;
        cameraFrontDirection = glm::normalize(target - position);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }

    glm::vec3 Camera::getPosition() const {
        return cameraPosition;
    }

    glm::vec3 Camera::getFront() {
        return cameraFrontDirection;
    }
    void gps::Camera::setPosition(glm::vec3 position) {
        cameraPosition = position;
		cameraTarget = cameraPosition + cameraFrontDirection;
	}
}
