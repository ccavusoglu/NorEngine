#include "Camera.h"

Camera::Camera() {
}

void Camera::setCamera(GPU_Camera *pCamera) {
    camera = pCamera;
}

void Camera::translate(glm::vec3 amount) {
    camera->x += amount.x;
    camera->y += amount.y;
    camera->z += amount.z;
}

GPU_Camera *Camera::getInternalCam() {
    return camera;
}

Camera::~Camera() {
    delete camera;
}
