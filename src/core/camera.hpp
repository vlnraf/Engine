#pragma once

#include <glm/glm.hpp>

struct Camera{
    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 target;

    //For perspective and 3d if needs to be implemented later (rotation)
    float pitch, yaw;

};
