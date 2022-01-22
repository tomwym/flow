#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

class Mesh {
public:
    Mesh(const std::vector<glm::vec3>& vertices);
    void Draw(const int n_elements, const int element_size);
    virtual ~Mesh();
protected:
private:
    Mesh(const Mesh& other) {};
    void operator=(const Mesh& other) {};    

    enum {
        POSITION_VB,
        NUM_BUFFERS
    };

    GLuint m_vertexArrayObject;
    GLuint m_vertexArrayBuffers[NUM_BUFFERS];

    unsigned int m_drawCount;
};