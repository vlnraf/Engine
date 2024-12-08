#include "camera.hpp"


OrtographicCamera createCamera(glm::vec3 pos, float width, float height){
    OrtographicCamera camera = {};
    camera.position = pos;
    camera.width = width;
    camera.height = height;

    return camera;
}

void followTarget(OrtographicCamera* camera, glm::vec3 targetPos){
    camera->position.x = floor(targetPos.x - (camera->width / 2));
    camera->position.y = floor(targetPos.y - (camera->height / 2));
    camera->position.z = camera->position.z;
}