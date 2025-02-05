#pragma once

#include <glm/glm.hpp>
#include "core/coreapi.hpp"

struct OrtographicCamera{
    glm::vec3 position;
    glm::vec3 target;
    float width, height; //Resolution
    glm::mat4 projection;
    glm::mat4 view;

};

CORE_API OrtographicCamera createCamera(glm::vec3 pos, const float width, const float height);
CORE_API void followTarget(OrtographicCamera* camera, const glm::vec3 targetPos);