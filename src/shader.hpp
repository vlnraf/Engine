#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

struct Shader{
    unsigned int id;
};

void useShader(const Shader* shader);
Shader createShader(const char* vertexPath, const char* fragmentPath);
void setUniform(const Shader* shader, const char* name , const float value);
void setUniform(const Shader* shader, const char* name , const bool value);
void setUniform(const Shader* shader, const char* name , const int value);
void setUniform(const Shader* shader, const char* name , const glm::mat4 value);
void setUniform(const Shader* shader, const char* name , const glm::vec3 value);