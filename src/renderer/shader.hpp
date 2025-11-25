#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "core/coreapi.hpp"
#include "core/arena.hpp"

struct Shader{
    unsigned int id;
};

void useShader(const Shader* shader);  // Internal: Use beginShaderMode() instead
CORE_API Shader createShader(Arena* arena, char* vertexPath, char* fragmentPath);
CORE_API void destroyShader(Shader* shader);
CORE_API void loadShader(const Shader* shader);    // Load shader on GPU to set uniforms
CORE_API void unloadShader(const Shader* shader);  // Unload shader from GPU
CORE_API void setUniform(const Shader* shader, const char* name , const float value);
CORE_API void setUniform(const Shader* shader, const char* name , const bool value);
CORE_API void setUniform(const Shader* shader, const char* name , const int value);
CORE_API void setUniform(const Shader* shader, const char* name , const uint32_t value);
CORE_API void setUniform(const Shader* shader, const char* name , const glm::mat4 value);
CORE_API void setUniform(const Shader* shader, const char* name , const glm::vec3 value);