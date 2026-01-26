#include "shader.hpp"
#include "core/tracelog.hpp"
#include "core/arena.hpp"
#include "core/mystring.hpp"

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#else
#include <GLES3/gl3.h>
#endif


// -------------------------------------------------------------------------------------------------
// Shader related functions
// -------------------------------------------------------------------------------------------------

String8 stringFromFile(Arena* arena, String8 path) {
    String8 result = {};
    String8 cPath = cStringFromString8(arena, path);
    FILE* file = fopen(cPath.str, "rb");
    if (!file) {
    LOGERROR("Error on loading the file %S", path);
    return result;
    }
    fseek(file, 0L, SEEK_END);
    uint64_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* fileContent = arenaAllocArrayZero(arena, char, fileSize + 1);
    char c;
    size_t readBytes = fread(fileContent, sizeof(char), fileSize, file);
    fclose(file);
    result.str = fileContent;
    result.size = readBytes;
    result.str[readBytes] = 0;
    return result;
}

//TODO: manage to attach a default shader when it fails
uint32_t compileShaderCode(Arena* arena, String8 shaderCode, int type){
    uint32_t id;
    id = glCreateShader(type);
    glShaderSource(id, 1, &shaderCode.str, NULL);
    glCompileShader(id);

    // print compile errors if any
    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        switch(type){
            case GL_VERTEX_SHADER :     LOGERROR("Shader ID = %u failed to compile vertex shader", id); break;
            case GL_FRAGMENT_SHADER:    LOGERROR("Shader ID = %u failed to compile fragment shader", id); break;
        }
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        String8 errorLog = {};
        errorLog.size = (uint64_t) length;
        errorLog.str = arenaAllocArrayZero(arena, char, errorLog.size);
        int actualLength = 0;
        glGetShaderInfoLog(id, errorLog.size, &actualLength, errorLog.str);
        errorLog.str[actualLength] = '\0';
        LOGERROR("Shader ID = %u compile error: %S", id, errorLog);
    };
    return id;
}

//TODO: manage to attach a default shader when it fails
uint32_t loadShaderProgram(Arena* arena, uint32_t vertex, uint32_t fragment){
    uint32_t id;

    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    // print linking errors if any
    int success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if(!success)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        String8 errorLog = {};
        errorLog.size = (uint64_t) length;
        errorLog.str = arenaAllocArrayZero(arena, char, errorLog.size);
        glGetProgramInfoLog(id, errorLog.size, NULL, errorLog.str);
        LOGERROR("Shader ID = %u Link error %S", id, errorLog);
    }
    return id;
}

//TODO: manage to attach a default shader when it fails
uint32_t loadShaderCode(Arena* arena, String8 vertexCode, String8 fragmentCode){
    uint32_t id;
    // Compile shader code
    uint32_t vertex = compileShaderCode(arena, vertexCode, GL_VERTEX_SHADER);
    uint32_t fragment = compileShaderCode(arena, fragmentCode, GL_FRAGMENT_SHADER);

    // Load shader program
    id = loadShaderProgram(arena, vertex, fragment);

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return id;
}

// Helper function to convert shader path to web path for Emscripten builds
// e.g., "shaders/quad-shader.vs" -> "shaders/web/quad-shader.vs"
#ifdef __EMSCRIPTEN__
static String8 convertToWebShaderPath(Arena* arena, String8 path) {
    // Find "shaders/" in the path
    const char* shadersDir = "shaders/";
    size_t shadersDirLen = 8;

    char* found = strstr(path.str, shadersDir);
    if (found) {
        // Build new path: everything before "shaders/" + "shaders/web/" + filename
        size_t beforeLen = found - path.str;
        size_t afterLen = path.size - beforeLen - shadersDirLen;
        const char* webDir = "shaders/web/";
        size_t webDirLen = 12;
        size_t newSize = beforeLen + webDirLen + afterLen;

        char* newPath = arenaAllocArray(arena, char, newSize + 1);
        memcpy(newPath, path.str, beforeLen);
        memcpy(newPath + beforeLen, webDir, webDirLen);
        memcpy(newPath + beforeLen + webDirLen, found + shadersDirLen, afterLen);
        newPath[newSize] = 0;

        return (String8){newPath, newSize};
    }
    return path;
}
#endif

Shader loadShader(Arena* arena, char* vertexPath, char* fragmentPath){
    String8 v = string8FromCString(vertexPath);
    String8 f = string8FromCString(fragmentPath);

    TempArena temp = getTempArena(arena);

#ifdef __EMSCRIPTEN__
    // Use shaders from shaders/web/ folder for WebGL builds
    v = convertToWebShaderPath(temp.arena, v);
    f = convertToWebShaderPath(temp.arena, f);
#endif

    String8 vertexCode = stringFromFile(temp.arena, v);
    String8 fragmentCode = stringFromFile(temp.arena, f);

    // compile shaders
    Shader shader = {0};
    shader.id = loadShaderCode(temp.arena, vertexCode, fragmentCode);

    releaseTempArena(temp);
    return shader;
}

void useShader(const Shader* shader){
    glUseProgram(shader->id);
}

void unBindShader(){
    glUseProgram(0);
}

void unloadShader(Shader* shader){
    if(shader->id != 0){
        glDeleteProgram(shader->id);
        shader->id = 0;
    }
}

void setUniform(const Shader* shader, const char* name , const float value){
    useShader(shader);
    int uniformId = glGetUniformLocation(shader->id, name);
    glUniform1f(uniformId, value);
}

void setUniform(const Shader* shader, const char* name , const bool value){
    useShader(shader);
    int uniformId = glGetUniformLocation(shader->id, name);
    glUniform1i(uniformId, value);
}

void setUniform(const Shader* shader, const char* name , const int* value, int size){
    useShader(shader);
    int uniformId = glGetUniformLocation(shader->id, name);
    glUniform1iv(uniformId, size, value);
}

void setUniform(const Shader* shader, const char* name , const int value){
    useShader(shader);
    int uniformId = glGetUniformLocation(shader->id, name);
    glUniform1i(uniformId, value);
}

void setUniform(const Shader* shader, const char* name , const uint32_t value){
    useShader(shader);
    int uniformId = glGetUniformLocation(shader->id, name);
    glUniform1i(uniformId, value);
}

void setUniform(const Shader* shader, const char* name , const glm::mat4 value){
    useShader(shader);
    int uniformId = glGetUniformLocation(shader->id, name);
    glUniformMatrix4fv(uniformId, 1, GL_FALSE, glm::value_ptr(value));
}

void setUniform(const Shader* shader, const char* name , const glm::vec3 value){
    useShader(shader);
    int uniformId = glGetUniformLocation(shader->id, name);
    glUniform3fv(uniformId, 1, glm::value_ptr(value));
}
