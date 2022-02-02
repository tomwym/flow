#include "geometry.hpp"

#include <iostream>
#include <vector>
#include <algorithm>

Geometry::Geometry(const E57Handle* const _e57objp) 
    : e57objp(_e57objp) {
}

void Geometry::CollectRawData() {
    std::cerr << "in collect raw data"  << '\n';
    std::pair<int64_t, int64_t> dimensions{e57objp->GetDataDims()};
    m_rawData = Eigen::Map<const Eigen::MatrixXf>
        (e57objp->HandleLinearData().data(),  dimensions.first, 3);
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
    const float scale = 2/deltas.at(std::distance(deltas.begin(), maxel));
    m_normalizedData = (scale*m_normalizedData.array()).matrix();
}

void Geometry::CollectRotationMatrix() {

}

void Geometry::CollectTranslationVector() {

}

void Geometry::CollectTransformedData() {

}

void Geometry::CollectPlanarData() {

}

void Geometry::CollectReducedData() {

}

void Geometry::CollectBoundaryPoints() {

}


const Eigen::MatrixXf& Geometry::GetNormalizedData() const {
    return m_normalizedData;
}