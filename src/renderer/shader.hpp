#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "core/coreapi.hpp"

struct Shader{
    unsigned int id;
};

CORE_API void useShader(const Shader* shader);
CORE_API Shader createShader(const char* vertexPath, const char* fragmentPath);
CORE_API void setUniform(const Shader* shader, const char* name , const float value);
CORE_API void setUniform(const Shader* shader, const char* name , const bool value);
CORE_API void setUniform(const Shader* shader, const char* name , const int value);
CORE_API void setUniform(const Shader* shader, const char* name , const uint32_t value);
CORE_API void setUniform(const Shader* shader, const char* name , const glm::mat4 value);
CORE_API void setUniform(const Shader* shader, const char* name , const glm::vec3 value);