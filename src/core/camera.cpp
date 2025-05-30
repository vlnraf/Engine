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
    //camera->position.x = targetPos.x - (camera->width / 2);
    //camera->position.y = targetPos.y - (camera->height / 2);
    camera->position.z = camera->position.z;

    camera->view = glm::translate(glm::mat4(1.0f), -camera->position);
}

glm::vec2 worldToScreen(const OrtographicCamera& camera, const glm::vec3& worldPos) {
    // Compute the combined VP matrix
    glm::mat4 vp = camera.projection * camera.view;

    // Transform the world position to clip space
    glm::vec4 clipSpacePos = vp * glm::vec4(worldPos, 1.0f);

    // Perform perspective division to get NDC (normalized device coordinates)
    glm::vec3 ndc = glm::vec3(clipSpacePos) / clipSpacePos.w;

    // Convert NDC to screen space
    float screenX = (ndc.x * 0.5f + 0.5f) * camera.width;
    float screenY = (ndc.y * 0.5f + 0.5f) * camera.height;
    screenY = camera.height - screenY;

    return glm::vec2(screenX, screenY);
}

glm::vec2 screenToWorld(const OrtographicCamera& camera, const glm::vec2& screenSize, const glm::vec2& screenPos){
    // Convert from top-left origin to bottom-left origin
    glm::vec2 flipped = {screenPos.x, screenSize.y - screenPos.y};

    // Normalize to [0, 1]
    glm::vec2 normalized = flipped / screenSize;

    // Scale to camera world size
    glm::vec2 worldPos = normalized * glm::vec2(camera.width, camera.height);

    // Offset by camera position 
    worldPos += glm::vec2(camera.position.x, camera.position.y);

    return worldPos;
}