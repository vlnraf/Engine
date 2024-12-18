#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

OrtographicCamera createCamera(glm::vec3 pos, const float width, const float height){
    OrtographicCamera camera = {};
    camera.position = pos;
    camera.width = width;
    camera.height = height;

    camera.projection = glm::ortho(0.0f, width, 0.0f, height, -100.0f, 100.0f);
    camera.view = glm::mat4(1.0f);

    return camera;
}

void followTarget(OrtographicCamera* camera, const glm::vec3 targetPos){
    camera->position.x = floor(targetPos.x - (camera->width / 2));
    camera->position.y = floor(targetPos.y - (camera->height / 2));
    camera->position.z = camera->position.z;

    camera->view = glm::translate(glm::mat4(1.0f), -camera->position);
}