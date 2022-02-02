#pragma once

#include <string>
#include <GL/glew.h>

#include "display.hpp"
#include "transform.hpp"
#include "camera.hpp"

class Shader : public Display {
public: 
    Shader(const std::string& fileName);

    void Bind();
    void Update(const Transform& transform, const Camera& camera);

    virtual ~Shader();
protected:
private:
    static const unsigned int NUM_SHADERS = 2;
    Shader(const Shader& other) {}
    void operator=(const Shader& other) {}

    enum {
        TRANSFORM_U,
        NUM_UNIFORMS
    };

    std::string LoadShader(const std::string& fileName);
    void CheckShaderError(const GLuint shader,
                          const GLuint flag,
                          const bool isProgram,
                          const std::string& errorMessage);
    GLuint CreateShader(const std::string& text, const unsigned int type);

    GLuint m_program;
    GLuint m_shaders[NUM_SHADERS];
    GLuint m_uniforms[NUM_UNIFORMS];
};