#include "mesh.hpp"

#include <algorithm>
#include <numeric>
#include <GL/glew.h>

// Mesh //
Mesh::Mesh(const std::vector<glm::vec3>& vertices1,
           const std::vector<glm::vec3>& vertices2) {

    glGenVertexArrays(NUM_VAO, m_vao);
    glGenBuffers(NUM_VBO, m_vbo);

    glViewport(0, 0, (GLsizei)m_width, (GLsizei)m_height);
    // static mesh
    UpdateStatic({});
    // dynamic mesh
    UpdateDynamic({});
}

void Mesh::UpdateStatic(const std::vector<glm::vec3>& vertices) {
    m_drawCount[0] = vertices.size();
    glBindVertexArray(m_vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
    glBufferData(GL_ARRAY_BUFFER,
                 m_drawCount[0] * sizeof(glm::vec3),
                 vertices.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Mesh::UpdateDynamic(const std::vector<glm::vec3>& vertices) {
    m_drawCount[1] = vertices.size();
    glBindVertexArray(m_vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
    glBufferData(GL_ARRAY_BUFFER,
                 m_drawCount[1] * sizeof(glm::vec3),
                 vertices.data(),
                 GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(NUM_VAO, m_vao);
}

void Mesh::Draw() {
    glColor4f( 1.0, 0.0, 0.0, 1.0 );

    glBindVertexArray(m_vao[0]);
    glDrawArrays(DRAW_STATIC_PRIMIVE, 0, m_drawCount[0]);

    glBindVertexArray(m_vao[1]);
    glDrawArrays(DRAW_DYNAMIC_PRIMIVE, 0, m_drawCount[1]);

    glBindVertexArray(0);
}

void Mesh::DrawNArrays(const unsigned n_elements, const unsigned element_size) {
    glColor4f( 1.0, 0.0, 0.0, 1.0 );

    glBindVertexArray(m_vao[0]);
    glDrawArrays(DRAW_STATIC_PRIMIVE, 0, m_drawCount[0]);

    glBindVertexArray(m_vao[1]);

    std::vector<GLint> startingElements(n_elements);
    // creates {0,1,2,...}
    std::iota(startingElements.begin(), startingElements.end(), 0);
    // -> {0,c,2*c,3*c,...}
    transform(startingElements.begin(), startingElements.end(),
              startingElements.begin(),
              [element_size](int &c){ return c*element_size; });
    std::vector<GLint> counts(n_elements, element_size);
    glMultiDrawArrays(DRAW_DYNAMIC_PRIMIVE, startingElements.data(), counts.data(), n_elements);

    glBindVertexArray(0);
}

void Mesh::SetDrawStaticPrimitive(const unsigned in) {
    DRAW_STATIC_PRIMIVE = in;
}
void Mesh::SetDrawDynamicPrimitive(const unsigned in) {
    DRAW_DYNAMIC_PRIMIVE = in;
}