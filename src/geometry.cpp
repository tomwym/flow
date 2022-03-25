#include "geometry.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <numbers>
#include <thread>
#include <chrono>

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

void Geometry::CollectObjData(const std::string& fname) {
    std::ifstream ifs(fname);
    std::string buffer;
    std::string buf0;
    std::vector<float> container;

    while (std::getline(ifs, buffer, '\n')) {
        std::stringstream ss(buffer);
        ss >> buf0;
        if (buf0 != "v") {
            break;
        }
        for (int i=0; i<3; ++i) {
            float temp = 0; ss >> temp;
            container.push_back(temp);
        }
    }
    std::cout << " obj size: " << container.size()/3 << '\n';
    m_rawData = Eigen::Map<const Eigen::MatrixXf, Eigen::RowMajor>
            (container.data(), container.size()/3, 3);
}

/*
 * Collect the raw data from the fileIO input, transform s.t. the min and max
 * coordinate in all 3 axis is no larger than 1, min no smaller than -1
 * Translate such that each basis axis is centered
 * Scale down such that max basis dim = 2 (1 - (-1))
 */
void Geometry::CollectNormalizedData() {
    std::cerr << "in collect normalized data"  << '\n';
    // set the buffer m_normalizedData as copy
    m_normalizedPxData = m_rawData;
    // deltas represent the absolute difference in each basis {dx, dy, dz}
    std::vector<float> deltas {};
    deltas.reserve(m_normalizedPxData.cols());

    // iterate over each column of m_normalizedData ({0,1,2} ~ {x,y,z})
    for (auto col : m_normalizedPxData.colwise()) {
        // get min and max of each columns and find difference
        const float maxval = col.maxCoeff();
        const float minval = col.minCoeff();
        const float delta = maxval-minval;
        const float translate = minval+delta/2.f;
        col = col.array() - translate;
        deltas.push_back(maxval-minval);
    }

    const auto maxel = max_element(deltas.begin(), deltas.end());
    // max proportion of window in any direction
    const float arbitrary_window_scale = 0.25;
    const float scale = arbitrary_window_scale*2/deltas.at(std::distance(deltas.begin(), maxel));
    const float get_dim = m_width > m_height ? m_height : m_width;
    // below is the full data as represented with the pixel coords
    m_normalizedPxData = (get_dim*scale*m_normalizedPxData.array()).matrix();
    // populate a similar list with Gl coords (for direct plotting)
    Eigen::MatrixXf normalizedGlData_proto = m_normalizedPxData;
    for (auto row : normalizedGlData_proto.rowwise()) {
        row[0] /= m_height;
        row[1] /= m_height;
        row[2] /= m_height;
    }
    m_normalizedGlData = Geometry::VectorFromEigen(normalizedGlData_proto);
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

/*
 * Apply current projection to get the data on a 2d slice
 * Send the data to ReduceSize() to reduce the number of rows
 * At this point we want the planar data to exist in the pixel context coords
 */
void Geometry::CollectPlanarData() {
    m_planarData = Eigen::MatrixXf(m_normalizedPxData.rows(), 3);
    Eigen::Matrix3f rotation = CollectRotationMatrix<Eigen::Vector3f>(m_rotation_vals);
    Eigen::MatrixXf out = m_normalizedPxData * rotation.transpose();
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
    m_reducedPlanarPxData = Geometry::ReduceSize(m_planarData.rows()/10, m_planarData);
    Eigen::MatrixXf reducedPlanarGlData_proto = m_reducedPlanarPxData;
    for (auto row : reducedPlanarGlData_proto.rowwise()) {
        row[0] /= m_height;
        row[1] /= m_height;
        row[2] /= m_height;
    }
    m_reducedPlanarGlData = Geometry::VectorFromEigen(reducedPlanarGlData_proto);
}

/*
 * Create a new matrix from matrix of size: size
 * Randomly generates indices within size, and extracts from input
 */
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

/*
 * Runs the k nearest neighbors algorithm to determine the outline
 * For now, just call the external function ./a.out
 */
void Geometry::CollectBoundaryPoints(const int k) {
    const Eigen::MatrixXf& obj = GetReducedPlanarPxData();
    std::ofstream o("./dat/filtered2d.dat");
    for (const auto& row : obj.rowwise()) {
        o << row << '\n';
    }
    o.close();
    std::string command{ "./a.out ./dat/filtered2d.dat -k "+ std::to_string(k) + " -out ./dat/filteredBoundary.dat" };
    std::system(command.c_str());
    std::this_thread::sleep_for (std::chrono::milliseconds(200));

    // start off with matrix input
    std::ifstream ifs("./dat/filteredBoundary.dat");
    std::string buffer;
    std::getline(ifs, buffer, '\n');
    std::stringstream lineStream(buffer);

    std::vector<float> container;

    while (std::getline(ifs, buffer, '\n')) {
        std::stringstream ss(buffer);
        float temp = 0;
        for (int i=0; i<3; ++i) {
            ss >> temp;
            container.push_back(temp);
        }
    }
    for (int i=0; i<3; ++i) {
        container.push_back(container[i]);
    }
    Eigen::MatrixXf boundaryPointPxData_proto = Eigen::Map<const Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
        (container.data(), container.size()/3, 3);
    Eigen::MatrixXf boundaryPointGlData_proto = boundaryPointPxData_proto;
    for (auto row : boundaryPointGlData_proto.rowwise()) {
        row[0] /= m_height;
        row[1] /= m_height;
        row[2] /= m_height;
    }
    m_boundaryPointPxData = Geometry::VectorFromEigen(boundaryPointPxData_proto);
    m_boundaryPointGlData = Geometry::VectorFromEigen(boundaryPointGlData_proto);

}

// returns the circle, in pixel coordinates
// sets m_primitiveCircle and m_primitiveCircleScaled
std::vector<glm::vec3> Geometry::GetCirclePrimitive(const int n_partitions) {
    float r = 100;


    std::vector<glm::vec3> out;
    for (float i=0; i<=2*std::numbers::pi+0.00001; i+= 2*std::numbers::pi/n_partitions) {
        out.push_back(glm::vec3(r*std::cos(i), r*std::sin(i), 0));
    }
    m_primitiveCircle = out;
    m_primitiveCircleScaled = out;
    std::for_each(m_primitiveCircleScaled.begin(), m_primitiveCircleScaled.end(), [m_height]<typename T>(T& pt){
        pt[0] *= 1/m_height;
        pt[1] *= 1/m_height;
    });

    return out;
}

// returns the circle, in pixel coordinates
// sets m_primitiveCircle and m_primitiveCircleScaled
std::vector<glm::vec3> Geometry::TestShapePrimitive() {
    m_testShapeScaled;
    std::for_each(m_testShapeScaled.begin(),
                  m_testShapeScaled.end(),
                  [m_height]<typename T>(T& pt){
        pt[0] *= 1/m_height;
        pt[1] *= 1/m_height;
    });

    return {};
}

// full 3d representation
const std::vector<glm::vec3>& Geometry::GetNormalizedGlData() const {
    return m_normalizedGlData;
}
const Eigen::MatrixXf& Geometry::GetNormalizedPxData() const {
    return m_normalizedPxData;
}

// filtered 2d representation
const Eigen::MatrixXf& Geometry::GetReducedPlanarPxData() const {
    return m_reducedPlanarPxData;
}
const std::vector<glm::vec3>& Geometry::GetReducedPlanarGlData() const {
    return m_reducedPlanarGlData;
}

// boundary representation
const std::vector<glm::vec3>& Geometry::GetBoundaryPointPxData() const {
    return m_boundaryPointPxData;
}
const std::vector<glm::vec3>& Geometry::GetBoundaryPointGlData() const {
    return m_boundaryPointGlData;
}