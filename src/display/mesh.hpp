#pragma once

#include "display.hpp"

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Mesh : public Display {
public:
    Mesh(const std::vector<glm::vec3>& vertices1, const std::vector<glm::vec3>& vertices2);
    void UpdateStatic(const std::vector<glm::vec3>& vertices);
    void UpdateDynamic(const std::vector<glm::vec3>& vertices);
    void SetDrawStaticPrimitive(const unsigned in);
    void SetDrawDynamicPrimitive(const unsigned in);

    void Draw();
    virtual ~Mesh();
protected:
private:
    Mesh(const Mesh& other) = delete;
    void operator=(const Mesh& other) = delete;

    enum {
        POSITION_VB,
        NUM_BUFFERS
    };
    static const GLuint NUM_VBO = 2;
    static const GLuint NUM_VAO = 2;

    GLuint m_vao[NUM_VAO];
    GLuint m_vbo[NUM_VBO];

    unsigned int m_drawCount[2] = {0, 0};

    unsigned DRAW_STATIC_PRIMIVE = GL_POINTS;
    unsigned DRAW_DYNAMIC_PRIMIVE = GL_POINTS;
};
