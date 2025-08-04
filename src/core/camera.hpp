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
CORE_API glm::vec2 worldToScreen(const OrtographicCamera& camera, const glm::vec3& worldPos);
CORE_API glm::vec2 worldToScreen(const OrtographicCamera& camera, const glm::vec2& worldPos);
CORE_API glm::vec2 worldToScreen(glm::vec2 pos, glm::vec2 size, glm::vec2 screenSize);
CORE_API glm::vec2 screenToWorld(const OrtographicCamera& camera, const glm::vec2& screenSize, const glm::vec2& screenPos);