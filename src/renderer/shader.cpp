#include "shader.hpp"
#include "core/tracelog.hpp"

Shader createShader(const char* vertexPath, const char* fragmentPath){
    Shader shader = {0};
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();		
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();		
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];
    
    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
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
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
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
    return shader;
}

void useShader(const Shader* shader){
    glUseProgram(shader->id);
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