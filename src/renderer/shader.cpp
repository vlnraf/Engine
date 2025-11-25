#include "shader.hpp"
#include "core/tracelog.hpp"
#include "core/arena.hpp"

#define FILE_CHUNK_SIZE 4096

struct String8{
    char* str;
    uint64_t size;
};

String8 cStringFromString8(Arena* arena, String8 string){
    String8 result = {0};
    result.size = string.size;
    result.str = arenaAllocArray(arena, char, string.size + 1); // +1 to produce a null terminated string
    memCopy(result.str, string.str, string.size);
    result.str[string.size] = 0;
    return result;
}

String8 string8FromCString(char* string){
    String8 result = {0};
    uint64_t size = 0;
    for(int i = 0; string[i] != '\0'; i++){
        size++;
    }
    result.size = size;
    result.str = string;
    return result;
}

//String8 stringFromFile(Arena* arena, String8 path) {
//    String8 result = {0};
//
//    String8 cPath = cStringFromString8(arena, path);
//    FILE* file = fopen(cPath.str, "rb");
//    if (!file) {
//        return result; // file open failed
//    }
//
//    // Start with a small buffer; we'll grow it in the arena as needed
//    size_t capacity = FILE_CHUNK_SIZE;
//    size_t size = 0;
//    char* buffer = arenaAllocArrayZero(arena, char, capacity);
//    if (!buffer) {
//        fclose(file);
//        return result;
//    }
//
//    while (1) {
//        // How much space we have left
//        size_t spaceLeft = capacity - size;
//        if (spaceLeft == 0) {
//            // Grow buffer by another chunk
//            char* newBuffer = arenaAllocArrayZero(arena, char, capacity + FILE_CHUNK_SIZE);
//            if (!newBuffer) break; // allocation failed
//            memcpy(newBuffer, buffer, size); // copy old data
//            buffer = newBuffer;
//            capacity += FILE_CHUNK_SIZE;
//            spaceLeft = FILE_CHUNK_SIZE;
//        }
//
//        size_t bytesRead = fread(buffer + size, 1, spaceLeft, file);
//        size += bytesRead;
//
//        if (bytesRead < spaceLeft) {
//            // Either EOF or read error
//            if (feof(file)) break;
//            if (ferror(file)) {
//                size = 0; // read error, discard data
//                break;
//            }
//        }
//    }
//
//    fclose(file);
//
//    result.str = buffer;
//    result.size = size;
//    if (size < capacity) {
//        result.str[size] = '\0'; // ensure null-termination
//    }
//    return result;
//}

String8 stringFromFile(Arena* arena, String8 path) {
    String8 result = {};  
    String8 cPath = cStringFromString8(arena, path);
    FILE* file = fopen(cPath.str, "rb");
    if (!file) {
    LOGERROR("Error on loading the file %s", path.str);
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

Shader createShader(Arena* arena, char* vertexPath, char* fragmentPath){
    String8 v = string8FromCString(vertexPath);
    String8 f = string8FromCString(fragmentPath);

    TempArena temp = getTempArena(arena);

    String8 vertexShader = stringFromFile(temp.arena, v);
    String8 fragmentShader = stringFromFile(temp.arena, f);
    //testtt(arena, v, f);
    // compile shaders
    Shader shader = {0};
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];
    
    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShader.str, NULL);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };
    
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShader.str, NULL);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    };
    
    // shader Program
    shader.id = glCreateProgram();
    glAttachShader(shader.id, vertex);
    glAttachShader(shader.id, fragment);
    glLinkProgram(shader.id);
    // print linking errors if any
    glGetProgramiv(shader.id, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shader.id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    releaseTempArena(temp);
    return shader;
}

//Shader createShader(Arena* arena, char* vertexPath, char* fragmentPath){
//    Shader shader = {0};
//    std::string vertexCode;
//    std::string fragmentCode;
//    std::ifstream vShaderFile;
//    std::ifstream fShaderFile;
//    // ensure ifstream objects can throw exceptions:
//    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
//    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
//    try 
//    {
//        // open files
//        vShaderFile.open(vertexPath);
//        fShaderFile.open(fragmentPath);
//        std::stringstream vShaderStream, fShaderStream;
//        // read file's buffer contents into streams
//        vShaderStream << vShaderFile.rdbuf();
//        fShaderStream << fShaderFile.rdbuf();		
//        // close file handlers
//        vShaderFile.close();
//        fShaderFile.close();
//        // convert stream into string
//        vertexCode   = vShaderStream.str();
//        fragmentCode = fShaderStream.str();		
//    }
//    catch(std::ifstream::failure e)
//    {
//        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
//    }
//    const char* vShaderCode = vertexCode.c_str();
//    const char* fShaderCode = fragmentCode.c_str();
//
//    // compile shaders
//    unsigned int vertex, fragment;
//    int success;
//    char infoLog[512];
//    
//    // vertex Shader
//    vertex = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vertex, 1, &vShaderCode, NULL);
//    glCompileShader(vertex);
//    // print compile errors if any
//    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
//    if(!success)
//    {
//        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
//        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
//    };
//    
//    // fragment Shader
//    fragment = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragment, 1, &fShaderCode, NULL);
//    glCompileShader(fragment);
//    // print compile errors if any
//    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
//    if(!success)
//    {
//        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
//        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
//    };
//    
//    // shader Program
//    shader.id = glCreateProgram();
//    glAttachShader(shader.id, vertex);
//    glAttachShader(shader.id, fragment);
//    glLinkProgram(shader.id);
//    // print linking errors if any
//    glGetProgramiv(shader.id, GL_LINK_STATUS, &success);
//    if(!success)
//    {
//        glGetProgramInfoLog(shader.id, 512, NULL, infoLog);
//        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
//    }
//    
//    // delete the shaders as they're linked into our program now and no longer necessary
//    glDeleteShader(vertex);
//    glDeleteShader(fragment);
//    return shader;
//}

void useShader(const Shader* shader){
    glUseProgram(shader->id);
}

void destroyShader(Shader* shader){
    if(shader->id != 0){
        glDeleteProgram(shader->id);
        shader->id = 0;
    }
}

void setUniform(const Shader* shader, const char* name , const float value){
    int uniformId = glGetUniformLocation(shader->id, name);
    glUniform1f(uniformId, value);
}

void setUniform(const Shader* shader, const char* name , const bool value){
    int uniformId = glGetUniformLocation(shader->id, name);
    glUniform1i(uniformId, value);
}

void setUniform(const Shader* shader, const char* name , const int value){
    int uniformId = glGetUniformLocation(shader->id, name);
    glUniform1i(uniformId, value);
}

void setUniform(const Shader* shader, const char* name , const uint32_t value){
    int uniformId = glGetUniformLocation(shader->id, name);
    glUniform1i(uniformId, value);
}

void setUniform(const Shader* shader, const char* name , const glm::mat4 value){
    int uniformId = glGetUniformLocation(shader->id, name);
    glUniformMatrix4fv(uniformId, 1, GL_FALSE, glm::value_ptr(value));
}

void setUniform(const Shader* shader, const char* name , const glm::vec3 value){
    int uniformId = glGetUniformLocation(shader->id, name);
    glUniform3fv(uniformId, 1, glm::value_ptr(value));
}