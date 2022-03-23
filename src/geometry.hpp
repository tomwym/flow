#pragma once

#include <glm/glm.hpp>
#include "Eigen/Dense"

#include "display.hpp"
#include "e57handle.hpp"
#include "algorithms.hpp"

/**
 * Container class for processed data
 *
 */

class Geometry : Display {
public:
    Geometry(const E57Handle* const _e57objp);

    // ideall, reduce the need to type
    // geom->somefunction();
    // geom->Update*(arg);
    // geom->SetDraw*Primitive*(arg);
    void Interface( void(Geometry::*mainf)() );
    void CollectRawData();
    void CollectNormalizedData();
    template<typename T>
    Eigen::Matrix3f CollectRotationMatrix(const T& rotations);
    void CollectPlanarData();
    void CollectClusteredData(const Eigen::MatrixXf& input);
    void CollectBoundaryPoints();

    std::vector<glm::vec3> GetCirclePrimitive();
    std::vector<glm::vec3> TestShapePrimitive();

    static std::vector<glm::vec3> VectorFromEigen(const Eigen::MatrixXf& matrix);
    static Eigen::MatrixXf EigenFromVector(const std::vector<glm::vec3>& vvec3);
    static Eigen::MatrixXf ReduceSize(const size_t size, const Eigen::MatrixXf& input);
    const Eigen::MatrixXf& GetNormalizedData() const;
    const Eigen::MatrixXf& GetPlanarData() const;
    const Eigen::MatrixXf& GetReducedPlanarData() const;
    const Eigen::MatrixXf& GetClusteredData() const;
    const Eigen::MatrixXf& GetBoundaryPoints() const;

    Eigen::Vector3f m_rotation_vals = {0, 0, 0};
    const int GetCurrentAxis() const { return current_axis; }
    void SetCurrentAxis(const int _current_axis) {
        current_axis = _current_axis;
    }
    int current_axis = 0;

    std::vector<glm::vec3> m_testShape;
    std::vector<glm::vec3> m_testShapeScaled;
    std::vector<glm::vec3> m_primitiveCircle;
    std::vector<glm::vec3> m_primitiveCircleScaled;

protected:
private:
    Geometry() = delete;
    const E57Handle* const e57objp;
    int m_itemcount = 0;
    Eigen::Vector3f m_view_normal = {0, 0, 1};

    Eigen::MatrixXf m_rawData; //.[L] {arbitrary}
    Eigen::MatrixXf m_normalizedData; //.[L] {m_width, m_height, 1}
    Eigen::MatrixXf m_planarData; //.[L] {m_width, m_height, 1}
    Eigen::MatrixXf m_reducedPlanarData; //.[l] {m_width, m_height, 1}
    // note: m_boundaryPoints is list of boundary pts w initial pt REPEATED
    Eigen::MatrixXf m_boundaryPoints; //.[s] {m_width, m_height, 1}
};