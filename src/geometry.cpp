#include "geometry.hpp"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

Geometry::Geometry(const E57Handle* const _e57objp) 
    : e57objp(_e57objp) {
}


std::vector<glm::vec3> Geometry::VectorFromEigen(const Eigen::MatrixXf& matrix) {
    std::vector<glm::vec3> out;
    out.reserve(matrix.rows());
    for (const auto& row : matrix.rowwise()) {
        out.push_back(glm::vec3(row[0], row[1], row[2]));
    }
    return out;
}

void Geometry::CollectRawData() {
    std::cerr << "in collect raw data"  << '\n';
    std::pair<int64_t, int64_t> dimensions{e57objp->GetDataDims()};
    m_rawData = Eigen::Map<const Eigen::MatrixXf>
        (e57objp->HandleLinearData().data(), dimensions.first, 3);
}

// normalize such that max(x,y,z)=1, min(x,y,z)=-1
// translate then scale
void Geometry::CollectNormalizedData() {
    std::cerr << "in collect normalized data"  << '\n';
    m_normalizedData = m_rawData;
    std::vector<float> deltas {};
    deltas.reserve(m_normalizedData.cols());
    
    for (auto col : m_normalizedData.colwise()) {
        const float maxval = col.maxCoeff();
        const float minval = col.minCoeff();
        const float delta = maxval-minval;
        const float translate = minval+delta/2.f;
        col = col.array() - translate;
        deltas.push_back(maxval-minval);
    }

    const auto maxel = max_element(deltas.begin(), deltas.end());
    const float arbitrary_window_scale = 0.5;
    const float scale = arbitrary_window_scale*2/deltas.at(std::distance(deltas.begin(), maxel));
    m_normalizedData = (scale*m_normalizedData.array()).matrix();
}

Eigen::Matrix3f Geometry::CollectRotationMatrix() {
    const float x = m_rotation_vals[0];
    const float y = m_rotation_vals[1];
    const float z = m_rotation_vals[2];

    const float cz = std::cos(z);
    const float sz = std::sin(z);
    Eigen::Matrix3f rotZ = Eigen::Matrix3f::Identity();
    rotZ(0,0) = cz; rotZ(0,1) = -sz;
    rotZ(1,0) = sz; rotZ(1,1) = cz;

    const float cy = std::cos(y);
    const float sy = std::sin(y);
    Eigen::Matrix3f rotY = Eigen::Matrix3f::Identity();
    rotY(0,0) = cy; rotY(0,2) = -sy;
    rotY(2,0) = sy; rotY(2,2) = cy;

    const float cx = std::cos(x);
    const float sx = std::sin(x);
    Eigen::Matrix3f rotX = Eigen::Matrix3f::Identity();
    rotX(1,1) = cx; rotZ(1,2) = -sx;
    rotX(2,1) = sx; rotZ(2,2) = cx;

    return rotZ * rotY * rotX;
}

void Geometry::CollectTranslationVector() {

}

void Geometry::CollectTransformedData() {

}

void Geometry::CollectPlanarData() {
    m_planarData = Eigen::MatrixXf(m_normalizedData.rows(), 3);
    Eigen::Matrix3f rotation = CollectRotationMatrix();
    // sets the normal to project to according to current rotation
    Eigen::Matrix3f basis = rotation * Eigen::Matrix3f::Identity();
    Eigen::Vector3f e_x = basis.col(0);
    Eigen::Vector3f e_y = basis.col(1);
    Eigen::Vector3f e_z = basis.col(2);
    // get last column in 3x3 matrix (starting at [0,2], for 3 rows 1  column)
    //m_view_normal = basis.block(0, 2, 3, 1);
    m_view_normal = e_z;

    // m_planarData;
    int i = 0;
    for (const auto& row : m_normalizedData.rowwise()) {
        const float mag = row.dot(m_view_normal);
        const Eigen::Vector3f pprime = row - mag * row;
        const float projxprime = pprime.dot(e_x);
        const float projyprime = pprime.dot(e_y);
        m_planarData(i,0) = projxprime;
        m_planarData(i,1) = projyprime;
        m_planarData(i,2) = 0;
        ++i;
    }
}

void Geometry::CollectReducedData() {

}

void Geometry::CollectBoundaryPoints() {

}


const Eigen::MatrixXf& Geometry::GetNormalizedData() const {
    return m_normalizedData;
}

const Eigen::MatrixXf& Geometry::GetPlanarData() const {
    return m_planarData;
}