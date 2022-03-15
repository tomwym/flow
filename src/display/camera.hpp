#pragma once

#include <glm/glm.hpp>

#include "display.hpp"

class Camera : public Display {
public:
    Camera(const glm::vec3& pos, const float fov, const float zNear, const float zFar) {
        m_perspective = glm::perspective(fov, m_aspect, zNear, zFar);
        m_position = pos;
        m_forward = glm::vec3(0,0,1);
        m_up = glm::vec3(0,1,0);
    }
    inline glm::mat4 GetViewProjection() const {
        return m_perspective * glm::lookAt(m_position, m_position+m_forward, m_up);
    }

protected:
private:
    constexpr static float m_aspect = m_width/m_height;
    glm::mat4 m_perspective;
    glm::vec3 m_position;
    glm::vec3 m_forward;
    glm::vec3 m_up;
};