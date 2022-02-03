#pragma once

#include <vector>
#include <glm/glm.hpp>

template<typename T>
std::vector<glm::vec3> makeTriangle(float width, float height, const float theta, const float r) {
    constexpr static float dtheta = M_PI/9.0;
    float rx = 2*r/(float)width;
    float ry = 2*r/(float)height;
    std::vector<glm::vec3> out({glm::vec3(0,0,0)});
    out.push_back(glm::vec3(rx*std::cos(theta), ry*std::sin(theta), 0));
    out.push_back(glm::vec3(rx*std::cos(theta+dtheta), ry*std::sin(theta+dtheta), 0));
    return out;
}

template<typename T>
std::vector<glm::vec3> makecircle(float width, float height, float r, float x, float y) {
    float normalized_x = 2*x/width;
    float normalized_y = 2*y/height;
    float rx = 2*r/width;
    float ry = 2*r/height;
    // std::cout << rx << ' ' << ry << '\n';
    // std::cout << normalized_x << ' ' << normalized_y << '\n';

    std::vector<glm::vec3> out({{normalized_x,normalized_y,0}});
    for (float i=0; i<=2*M_PI; i+= 2*M_PI/10) {
        out.push_back(glm::vec3(rx*std::cos(i) + normalized_x, ry*std::sin(i) + normalized_y, 0));
    }

    return out;
}

template<typename T>
std::vector<glm::vec3> convertPoints(const Eigen::MatrixXf& X) {
    std::vector<glm::vec3> out;
    out.reserve(X.rows());
    for (const auto& row : X.rowwise()) {
        out.push_back(glm::vec3(row[0], row[1], row[2]));
    }
    return out;
}
