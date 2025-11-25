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

CORE_API OrtographicCamera createCamera(float left, float right, float bottom, float top);
CORE_API void setProjection(OrtographicCamera* camera, float left, float right, float bottom, float top);
CORE_API void updateCameraAspectRatio(OrtographicCamera* camera, float viewportWidth, float viewportHeight);
CORE_API void setPosition(OrtographicCamera* camera, const glm::vec3& position);
CORE_API void followTarget(OrtographicCamera* camera, const glm::vec3 targetPos);
CORE_API glm::vec2 worldToScreen(const OrtographicCamera& camera, const glm::vec3& worldPos);
CORE_API glm::vec2 worldToScreen(const OrtographicCamera& camera, const glm::vec2& worldPos);
CORE_API glm::vec2 convertScreenCoords(glm::vec2 pos, glm::vec2 size, glm::vec2 screenSize);
CORE_API glm::vec2 screenToWorld(const OrtographicCamera& camera, const glm::vec2& screenSize, const glm::vec2& screenPos);
CORE_API void setActiveCamera(OrtographicCamera* camera);
CORE_API OrtographicCamera* getActiveCamera();