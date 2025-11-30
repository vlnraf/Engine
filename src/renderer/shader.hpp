#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "core/coreapi.hpp"
#include "core/arena.hpp"

//enum AssetType{
//    ASSET_NONE = 0,
//    ASSET_TEXTURE,
//    ASSET_AUDIO,
//    ASSET_SHADER,
//    ASSET_FONT
//
//};

// -------------------------------------------------------------------------------------------------
// Shader 
// -------------------------------------------------------------------------------------------------

struct Shader{
    unsigned int id;
};

CORE_API Shader loadShader(Arena* arena, char* vertexPath, char* fragmentPath);
CORE_API void unloadShader(Shader* shader);
CORE_API void setUniform(const Shader* shader, const char* name , const float value);
CORE_API void setUniform(const Shader* shader, const char* name , const bool value);
CORE_API void setUniform(const Shader* shader, const char* name , const int value);
CORE_API void setUniform(const Shader* shader, const char* name , const uint32_t value);
CORE_API void setUniform(const Shader* shader, const char* name , const glm::mat4 value);
CORE_API void setUniform(const Shader* shader, const char* name , const glm::vec3 value);
void setUniform(const Shader* shader, const char* name , const int* value, int size);

void useShader(const Shader* shader);  // Internal: Use beginShaderMode() instead
void unBindShader();