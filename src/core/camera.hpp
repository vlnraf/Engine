#pragma once

#include <glm/glm.hpp>

struct OrtographicCamera{
    glm::vec3 position;
    glm::vec3 target;
    float width, height; //Resolution

};

OrtographicCamera createCamera(glm::vec3 pos, float width, float height);
void followTarget(OrtographicCamera* camera, glm::vec3 targetPos);