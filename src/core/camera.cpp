#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

OrtographicCamera* activeCamera;

OrtographicCamera createCamera(float left, float right, float bottom, float top){
    OrtographicCamera camera = {};
    camera.position = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.width = right - left;
    camera.height = top - bottom;

    camera.projection = glm::ortho(left, right, bottom, top, -100.0f, 100.0f);
    camera.view = glm::mat4(1.0f);

    return camera;
}

void setProjection(OrtographicCamera* camera, float left, float right, float bottom, float top){
    camera->width = right - left;
    camera->height = top - bottom;
    camera->projection = glm::ortho(left, right, bottom, top, -100.0f, 100.0f);
}

void updateCameraAspectRatio(OrtographicCamera* camera, float viewportWidth, float viewportHeight){
    // Keep vertical height constant, adjust horizontal width based on aspect ratio
    float verticalSize = camera->height;  // This was set in createCamera
    float aspectRatio = viewportWidth / viewportHeight;

    float halfHeight = verticalSize / 2.0f;
    float halfWidth = halfHeight * aspectRatio;

    // Update projection to maintain aspect ratio
    camera->width = halfWidth * 2.0f;
    camera->projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -100.0f, 100.0f);
}

void setPosition(OrtographicCamera* camera, const glm::vec3& position){
    camera->position = position;
    camera->view = glm::translate(glm::mat4(1.0f), -camera->position);
}

void followTarget(OrtographicCamera* camera, const glm::vec3 targetPos){
    // For Hazel-style centered cameras: just set position to target
    // The centered projection automatically centers the view on this position
    camera->position = targetPos;
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

glm::vec2 worldToScreen(const OrtographicCamera& camera, const glm::vec2& worldPos) {
    return worldToScreen(camera, glm::vec3(worldPos, 0.0f));
}

glm::vec2 convertScreenCoords(glm::vec2 pos, glm::vec2 size, glm::vec2 screenSize){
    glm::vec2 screenPos = {pos.x, screenSize.y - (pos.y + size.y)};
    return screenPos;
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

void setActiveCamera(OrtographicCamera* camera){
    activeCamera = camera;
}

OrtographicCamera* getActiveCamera(){
    return activeCamera;
}