#include "mesh.hpp"

#include <numeric>
#include <GL/glew.h>

// Mesh //
Mesh::Mesh(const std::vector<glm::vec3>& vertices1,
           const std::vector<glm::vec3>& vertices2) {

    m_drawCount[0] = vertices1.size();
    m_drawCount[1] = vertices2.size();

    glGenVertexArrays(NUM_VAO, m_vao);
    glGenBuffers(NUM_VBO, m_vbo);

    // vertices1
    glBindVertexArray(m_vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
    glBufferData(GL_ARRAY_BUFFER,
                 m_drawCount[0] * sizeof(glm::vec3),
                 vertices1.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // vertices2
    glBindVertexArray(m_vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
    glBufferData(GL_ARRAY_BUFFER,
                 m_drawCount[1] * sizeof(glm::vec3),
                 vertices2.data(),
                 GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(NUM_VAO, m_vao);
}

void Mesh::Draw(const std::vector<glm::vec3>& vertices, 
                const int n_elements, const int element_size) {
    glColor4f( 1.0, 0.0, 0.0, 1.0 );
    std::vector<GLint> startingElements(n_elements);
    std::iota(startingElements.begin(), startingElements.end(), 0);
    transform(startingElements.begin(), startingElements.end(),
              startingElements.begin(),
              [element_size](int &c){ return c*element_size; });
    std::vector<GLint> counts(n_elements, element_size);

    glBindVertexArray(m_vao[0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_drawCount[0]);

    glBindVertexArray(m_vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
    glBufferData(GL_ARRAY_BUFFER,
                 m_drawCount[1] * sizeof(glm::vec3),
                 vertices.data(),
                 GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, m_drawCount[1]);
    glBindVertexArray(0);

}
