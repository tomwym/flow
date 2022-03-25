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
    void CollectObjData(const std::string& fname);
    void CollectNormalizedData();
    template<typename T>
    Eigen::Matrix3f CollectRotationMatrix(const T& rotations);
    void CollectPlanarData();
    void CollectClusteredData(const Eigen::MatrixXf& input);
    void CollectBoundaryPoints(const int k);

    std::vector<glm::vec3> GetCirclePrimitive(const int n_partitions);
    std::vector<glm::vec3> TestShapePrimitive();

    static std::vector<glm::vec3> VectorFromEigen(const Eigen::MatrixXf& matrix);
    static Eigen::MatrixXf EigenFromVector(const std::vector<glm::vec3>& vvec3);
    static Eigen::MatrixXf ReduceSize(const size_t size, const Eigen::MatrixXf& input);
    // after normalizing to fit [1 -1]
    const std::vector<glm::vec3>& GetNormalizedGlData() const;
    const Eigen::MatrixXf& GetNormalizedPxData() const;

    const Eigen::MatrixXf& GetPlanarPxData() const;

    const Eigen::MatrixXf& GetReducedPlanarPxData() const;
    const std::vector<glm::vec3>& GetReducedPlanarGlData() const;

    const std::vector<glm::vec3>& GetBoundaryPointPxData() const;
    const std::vector<glm::vec3>& GetBoundaryPointGlData() const;

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
    /*
     * *Px* naming convention refers to data in the Px coordinate system {m_width, m_height, 1}
     * *Gl* naming convention refers to data in the Gl coordinate system {1, 1, 1}

     */
    Eigen::MatrixXf m_rawData; //.[L] {arbitrary}
    Eigen::MatrixXf m_normalizedPxData; //.[L] {m_width, m_height, 1}
    std::vector<glm::vec3> m_normalizedGlData; //.[L] {1, 1, 1}
    Eigen::MatrixXf m_planarData; //.[L] {m_width, m_height, 1}
    Eigen::MatrixXf m_reducedPlanarPxData; //.[l] {m_width, m_height, 1}
    std::vector<glm::vec3> m_reducedPlanarGlData; //.[l] {1, 1, 1}
    // note: m_boundaryPointPxData is list of boundary pts w initial pt REPEATED
    std::vector<glm::vec3> m_boundaryPointPxData; //.[s] {m_width, m_height, 1}
    std::vector<glm::vec3> m_boundaryPointGlData; //.[s] {m_width, m_height, 1}
};