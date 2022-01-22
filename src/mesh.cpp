#include "mesh.hpp"
#include "e57handle.hpp"

#include <numeric>
#include <algorithm>

Mesh::Mesh(const std::vector<glm::vec3>& vertices) {
    m_drawCount = vertices.size();

    glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);

    glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffers);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[POSITION_VB]);
    glBufferData(GL_ARRAY_BUFFER, m_drawCount * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &m_vertexArrayObject);
}

void Mesh::Draw(const int n_elements, const int element_size) {
    glBindVertexArray(m_vertexArrayObject); 
    glColor4f( 1.0, 0.0, 0.0, 1.0 );
    std::vector<GLint> startingElements(n_elements);
    std::iota(startingElements.begin(), startingElements.end(), 0);
    transform(startingElements.begin(), startingElements.end(),
              startingElements.begin(),
              [element_size](int &c){ return c*element_size; });
    std::vector<GLint> counts(n_elements, element_size);
    
    glDrawArrays(GL_POINTS, 0, m_drawCount); // 2 fans
    //glMultiDrawArrays(GL_TRIANGLE_FAN, startingElements.data(), counts.data(), 2); // 2 fans
    glBindVertexArray(0);
}