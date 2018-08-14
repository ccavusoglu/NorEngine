#ifndef NORENGINE_CAMERA_H
#define NORENGINE_CAMERA_H


#include <SDL_gpu.h>
#include "vec3.hpp"

class Camera {
public:
    Camera();

    ~Camera();

    void setCamera(GPU_Camera *pCamera);

    void translate(glm::vec3 amount);

    GPU_Camera *getInternalCam();

private:
    GPU_Camera *camera;

};


#endif //NORENGINE_CAMERA_H
