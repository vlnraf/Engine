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

Shader loadShader(Arena* arena, char* vertexPath, char* fragmentPath){
    String8 v = string8FromCString(vertexPath);
    String8 f = string8FromCString(fragmentPath);

    TempArena temp = getTempArena(arena);

    String8 vertexCode = stringFromFile(temp.arena, v);
    String8 fragmentCode = stringFromFile(temp.arena, f);

#ifdef __EMSCRIPTEN__
    // WebGL requires "#version 300 es" instead of "#version 330"
    if (vertexCode.size >= 12 && memcmp(vertexCode.str, "#version 330", 12) == 0) {
        // Find end of version line
        char* lineEnd = vertexCode.str + 12;
        while (*lineEnd == '\r' || *lineEnd == '\n') lineEnd++;

        // Build new shader: "#version 300 es\n" + rest
        uint64_t afterVersionLen = vertexCode.size - (lineEnd - vertexCode.str);
        uint64_t newSize = 16 + afterVersionLen; // "#version 300 es\n" is 16 chars

        char* newVert = arenaAllocArray(temp.arena, char, newSize + 1);
        memcpy(newVert, "#version 300 es\n", 16);
        memcpy(newVert + 16, lineEnd, afterVersionLen);
        newVert[newSize] = 0;

        vertexCode.str = newVert;
        vertexCode.size = newSize;

        // WebGL doesn't support integer vertex attributes - replace "in int aTexIndex" with "in float aTexIndex"
        const char* searchPattern = "in int aTexIndex";
        char* found = strstr(vertexCode.str, searchPattern);
        if (found) {
            // Rebuild string: replace "in int aTexIndex" with "in float aTexIndex"
            const char* replacement = "in float aTexIndex";
            size_t beforeLen = found - vertexCode.str;
            size_t afterLen = vertexCode.size - (found + strlen(searchPattern) - vertexCode.str);
            size_t newSize = beforeLen + strlen(replacement) + afterLen;

            char* newCode = arenaAllocArray(temp.arena, char, newSize + 1);
            memcpy(newCode, vertexCode.str, beforeLen);
            memcpy(newCode + beforeLen, replacement, strlen(replacement));
            memcpy(newCode + beforeLen + strlen(replacement),
                   found + strlen(searchPattern), afterLen);
            newCode[newSize] = 0;

            vertexCode.str = newCode;
            vertexCode.size = newSize;

            // Now find "TexIndex = aTexIndex;" and replace with "TexIndex = int(aTexIndex);"
            const char* assignPattern = "TexIndex = aTexIndex;";
            char* assignFound = strstr(vertexCode.str, assignPattern);
            if (assignFound) {
                // Build new line: "TexIndex = int(aTexIndex);"
                const char* assignReplacement = "TexIndex = int(aTexIndex);";
                beforeLen = assignFound - vertexCode.str;
                afterLen = vertexCode.size - (assignFound + strlen(assignPattern) - vertexCode.str);
                newSize = beforeLen + strlen(assignReplacement) + afterLen;

                char* finalCode = arenaAllocArray(temp.arena, char, newSize + 1);
                memcpy(finalCode, vertexCode.str, beforeLen);
                memcpy(finalCode + beforeLen, assignReplacement, strlen(assignReplacement));
                memcpy(finalCode + beforeLen + strlen(assignReplacement),
                       assignFound + strlen(assignPattern), afterLen);
                finalCode[newSize] = 0;

                vertexCode.str = finalCode;
                vertexCode.size = newSize;
            }
        }
    }

    if (fragmentCode.size >= 12 && memcmp(fragmentCode.str, "#version 330", 12) == 0) {
        // Find end of version line (skip \r\n characters)
        char* lineEnd = fragmentCode.str + 12;
        while (*lineEnd == '\r' || *lineEnd == '\n' || *lineEnd == ' ' || *lineEnd == '\t') lineEnd++;

        // Build new fragment shader: "#version 300 es\nprecision mediump float;\n" + rest
        const char* header = "#version 300 es\nprecision mediump float;\n";
        uint64_t headerLen = 41; // Length of header string
        uint64_t afterVersionLen = fragmentCode.size - (lineEnd - fragmentCode.str);
        uint64_t newSize = headerLen + afterVersionLen;

        char* newFrag = arenaAllocArray(temp.arena, char, newSize + 1);
        memcpy(newFrag, header, headerLen);
        memcpy(newFrag + headerLen, lineEnd, afterVersionLen);
        newFrag[newSize] = 0;

        fragmentCode.str = newFrag;
        fragmentCode.size = newSize;

        // Fix dynamic sampler array indexing - WebGL doesn't support sprite[TexIndex]
        // Search for "texture(sprite[TexIndex]" and replace with switch statement
        const char* searchPattern = "texture(sprite[TexIndex]";
        char* found = strstr(fragmentCode.str, searchPattern);
        if (found) {
            // Find the complete texture() call
            char* texStart = found;
            char* texEnd = strchr(found, ')');
            if (texEnd) {
                texEnd++; // Include the closing paren

                // Extract TexCoord parameter (between , and ))
                char* coordStart = strchr(found, ',');
                if (coordStart) {
                    coordStart++; // Skip comma
                    while (*coordStart == ' ') coordStart++; // Skip spaces
                    char* coordEnd = texEnd - 1;
                    while (*(coordEnd-1) == ' ') coordEnd--; // Skip trailing spaces

                    // Build replacement switch statement
                    const char* switchTemplate =
                        "TexIndex == 0 ? texture(sprite[0], %.*s) :\n"
                        "        TexIndex == 1 ? texture(sprite[1], %.*s) :\n"
                        "        TexIndex == 2 ? texture(sprite[2], %.*s) :\n"
                        "        TexIndex == 3 ? texture(sprite[3], %.*s) :\n"
                        "        TexIndex == 4 ? texture(sprite[4], %.*s) :\n"
                        "        TexIndex == 5 ? texture(sprite[5], %.*s) :\n"
                        "        TexIndex == 6 ? texture(sprite[6], %.*s) :\n"
                        "        TexIndex == 7 ? texture(sprite[7], %.*s) :\n"
                        "        TexIndex == 8 ? texture(sprite[8], %.*s) :\n"
                        "        TexIndex == 9 ? texture(sprite[9], %.*s) :\n"
                        "        TexIndex == 10 ? texture(sprite[10], %.*s) :\n"
                        "        TexIndex == 11 ? texture(sprite[11], %.*s) :\n"
                        "        TexIndex == 12 ? texture(sprite[12], %.*s) :\n"
                        "        TexIndex == 13 ? texture(sprite[13], %.*s) :\n"
                        "        TexIndex == 14 ? texture(sprite[14], %.*s) :\n"
                        "        texture(sprite[15], %.*s)";

                    int coordLen = coordEnd - coordStart;
                    char switchCode[2048];
                    int switchLen = snprintf(switchCode, sizeof(switchCode), switchTemplate,
                        coordLen, coordStart, coordLen, coordStart, coordLen, coordStart, coordLen, coordStart,
                        coordLen, coordStart, coordLen, coordStart, coordLen, coordStart, coordLen, coordStart,
                        coordLen, coordStart, coordLen, coordStart, coordLen, coordStart, coordLen, coordStart,
                        coordLen, coordStart, coordLen, coordStart, coordLen, coordStart, coordLen, coordStart);

                    // Build new shader with replacement
                    uint64_t beforeTexLen = texStart - fragmentCode.str;
                    uint64_t afterTexLen = fragmentCode.size - (texEnd - fragmentCode.str);
                    uint64_t newSize = beforeTexLen + switchLen + afterTexLen;

                    char* finalCode = arenaAllocArray(temp.arena, char, newSize + 1);
                    memcpy(finalCode, fragmentCode.str, beforeTexLen);
                    memcpy(finalCode + beforeTexLen, switchCode, switchLen);
                    memcpy(finalCode + beforeTexLen + switchLen, texEnd, afterTexLen);
                    finalCode[newSize] = 0;

                    fragmentCode.str = finalCode;
                    fragmentCode.size = newSize;
                }
            }
        }
    }
#endif

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