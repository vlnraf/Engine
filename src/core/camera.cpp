#include "camera.hpp"


Camera createCamera(glm::vec3 pos){
    Camera camera = {};
    camera.position = pos;

    return camera;
}