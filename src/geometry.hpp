#pragma once

#include <glm/glm.hpp>
#include "Eigen/Dense"

#include "e57handle.hpp"

/**
 * Container class for processed data
 * 
 */

class Geometry {
public:
    Geometry(const E57Handle* const _e57objp);
    /* [0] */   void CollectRawData();
    /* [1] */   void CollectNormalizedData();
    /* [2] */   Eigen::Matrix3f CollectRotationMatrix();
    /* [3] */   void CollectTranslationVector();
    /* [4] */   void CollectTransformedData();
    /* [5] */   void CollectPlanarData();
    /* [6] */   void CollectReducedData();
    /* [7] */   void CollectBoundaryPoints();
    
    static std::vector<glm::vec3> VectorFromEigen(const Eigen::MatrixXf& matrix);
    const Eigen::MatrixXf& GetNormalizedData() const;
    const Eigen::MatrixXf& GetPlanarData() const;

    Eigen::Vector3f m_rotation_vals = {0, 0, 0};
    int current_axis = 0;
protected:
private:
    Geometry();
    const E57Handle* const e57objp;
    int m_itemcount = 0;

    /* [0] */   Eigen::MatrixXf m_rawData; //.[L]
    /* [1] */   Eigen::MatrixXf m_normalizedData; //.[L]
    /* [2] */   Eigen::Matrix3f m_rotationMatrix;
                Eigen::Vector3f m_view_normal = {0, 0, 1};
    /* [4] */   Eigen::MatrixXf m_transformedData; //.[L]
    /* [5] */   Eigen::MatrixXf m_planarData; //.[L]
    /* [6] */   Eigen::MatrixXf m_reducedData;
    /* [6] */   Eigen::MatrixXf m_boundaryPoints;
};