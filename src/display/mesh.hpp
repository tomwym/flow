#pragma once 

#include "display.hpp"

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Mesh : public Display {
public:
    Mesh(const std::vector<glm::vec3>& vertices1, const std::vector<glm::vec3>& vertices2);
    void Draw(const std::vector<glm::vec3>& vertices, 
              const int n_elements, const int element_size);
    virtual ~Mesh();
protected:
private:
    Mesh(const Mesh& other) {}
    void operator=(const Mesh& other) {}   

    enum {
        POSITION_VB,
        NUM_BUFFERS
    };
    static const GLuint NUM_VBO = 2;
    static const GLuint NUM_VAO = 2;

    GLuint m_vao[NUM_VAO];
    GLuint m_vbo[NUM_VBO];

    unsigned int m_drawCount[2];
};
