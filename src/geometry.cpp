#include "geometry.hpp"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <numbers>

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

Eigen::MatrixXf Geometry::EigenFromVector(const std::vector<glm::vec3>& vvec3) {
    return {};
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

template<typename T>
Eigen::Matrix3f Geometry::CollectRotationMatrix(const T& rotations) {
    const float x = rotations[0];
    const float y = rotations[1];
    const float z = rotations[2];

    const float cz = std::cos(z);
    const float sz = std::sin(z);
    Eigen::Matrix3f rotZ = Eigen::Matrix3f::Identity();
    rotZ(0,0) = cz; rotZ(0,1) = -sz;
    rotZ(1,0) = sz; rotZ(1,1) = cz;

    const float cy = std::cos(y);
    const float sy = std::sin(y);
    Eigen::Matrix3f rotY = Eigen::Matrix3f::Identity();
    rotY(0,0) = cy; rotY(0,2) = sy;
    rotY(2,0) = -sy; rotY(2,2) = cy;

    const float cx = std::cos(x);
    const float sx = std::sin(x);
    Eigen::Matrix3f rotX = Eigen::Matrix3f::Identity();
    rotZ(1,1) = cx; rotZ(1,2) = -sx;
    rotZ(2,1) = sx; rotZ(2,2) = cx;

    return rotZ * rotY * rotX;
}


void Geometry::CollectPlanarData() {
    m_planarData = Eigen::MatrixXf(m_normalizedData.rows(), 3);
    Eigen::Matrix3f rotation = CollectRotationMatrix<Eigen::Vector3f>(m_rotation_vals);
    Eigen::MatrixXf out = m_normalizedData * rotation.transpose();
    // sets the normal to project to according to current rotation
    Eigen::Matrix3f basis = Eigen::Matrix3f::Identity();
    Eigen::Vector3f e_x = basis.col(0);
    Eigen::Vector3f e_y = basis.col(1);
    Eigen::Vector3f e_z = basis.col(2);
    // get last column in 3x3 matrix (starting at [0,2], for 3 rows 1  column)
    //m_view_normal = basis.block(0, 2, 3, 1);
    m_view_normal = e_z;

    // m_planarData;
    int i = 0;
    for (const auto& row : out.rowwise()) {
        // [0, 0, 1]
        const float mag = row.dot(m_view_normal);
        // pprime is the point which now exists on the plane
        const Eigen::Vector3f pprime = row.transpose() - mag * m_view_normal;
        const float projxprime = pprime.dot(e_x);
        const float projyprime = pprime.dot(e_y);
        m_planarData(i,0) = projxprime;
        m_planarData(i,1) = projyprime;
        m_planarData(i,2) = 0;
        ++i;
    }
    m_reducedPlanarData = Geometry::ReduceSize(m_planarData.rows()/20, m_planarData);
}


Eigen::MatrixXf Geometry::ReduceSize(const size_t size, const Eigen::MatrixXf& input) {
    Eigen::MatrixXf out(size, input.cols());
    Eigen::VectorXf random_indices = Eigen::VectorXf::Random(size);
    const int lower = 0;
    const int upper = input.rows()-1;
    random_indices *= (upper-lower)/2;
    random_indices.array() += ((float)upper-(float)lower)/2;
    for (size_t i=0; i<size; ++i) {
        out.row(i) = input.row((int)random_indices(i));
    }
    return out;
}

void Geometry::CollectClusteredData(const Eigen::MatrixXf& input) {
    m_clusteredData = KMeans<void>(Geometry::ReduceSize(10000, input));
}

void Geometry::CollectBoundaryPoints() {
    // start off with matrix input
    std::ifstream ifs("./dat/bunnyBoundary.dat");
    std::string buffer;
    std::getline(ifs, buffer, '\n');
    std::stringstream lineStream(buffer);

    std::vector<float> container;

    while (std::getline(ifs, buffer, '\n')) {
        std::stringstream ss(buffer);
        float temp = 0;
        ss >> temp;
        container.push_back(temp);
        ss >> temp;
        container.push_back(temp);
        container.push_back(0);
    }
    m_boundaryPoints = Eigen::Map<const Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
        (container.data(), container.size()/3, 3);
}

// returns the circle, in pixel coordinates
// sets m_primitiveCircle and m_primitiveCircleScaled
std::vector<glm::vec3> Geometry::GetCirclePrimitive() {
    float r = 100;
    float rx = 2*r/m_width;
    float ry = 2*r/m_height;
    int n_partitions = 8;

    std::vector<glm::vec3> out;
    for (float i=0; i<=2*std::numbers::pi; i+= 2*std::numbers::pi/n_partitions) {
        out.push_back(glm::vec3(r*std::cos(i), r*std::sin(i), 0));
    }
    m_primitiveCircle = out;
    m_primitiveCircleScaled = out;
    std::for_each(m_primitiveCircleScaled.begin(), m_primitiveCircleScaled.end(), [m_width, m_height]<typename T>(T& pt){
        pt[0] *= 2/m_width;
        pt[1] *= 2/m_height;
    });

    return out;
}

// returns the circle, in pixel coordinates
// sets m_primitiveCircle and m_primitiveCircleScaled
std::vector<glm::vec3> Geometry::TestShapePrimitive() {
    m_testShapeScaled;
    std::for_each(m_testShapeScaled.begin(),
                  m_testShapeScaled.end(),
                  [m_width, m_height]<typename T>(T& pt){
        pt[0] *= 2/m_width;
        pt[1] *= 2/m_height;
    });

    return {};
}

const Eigen::MatrixXf& Geometry::GetNormalizedData() const {
    return m_normalizedData;
}

const Eigen::MatrixXf& Geometry::GetPlanarData() const {
    return m_planarData;
}

const Eigen::MatrixXf& Geometry::GetReducedPlanarData() const {
    return m_reducedPlanarData;
}

const Eigen::MatrixXf& Geometry::GetClusteredData() const {
    return m_clusteredData;
}

const Eigen::MatrixXf& Geometry::GetBoundaryPoints() const {
    return m_boundaryPoints;
}
