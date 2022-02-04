#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <set>
#include "Eigen/Dense"

template<typename T>
Eigen::MatrixXf KMeans(const Eigen::MatrixXf& points) {
    const int epochs = 100;
    const int k = 10;
    Eigen::MatrixXf centroids = Eigen::MatrixXf::Random(k, 3);
    centroids.col(2) *= 0;

    for (int i=0; i<epochs; i++) {
        std::vector<std::set<int>> assignments(k, std::set<int>{});

        int row_index = 0;
        for (const auto& row : points.rowwise()) {
            // distance to closest centroid
            float distance = std::numeric_limits<float>::max();
            // index of closest centroid (row of centroids)
            int distance_index = 0;

            int centroid_index = 0;
            for (const auto& centroid : centroids.rowwise()) {
                const Eigen::Matrix<float, 1, 3> diff = centroid - row;
                const float delta = diff.norm();
                if (delta < distance) {
                    distance = delta;
                    distance_index = centroid_index;
                }
                ++centroid_index;
            }
            // insert the row index of points into appropriate set
            assignments[distance_index].insert(row_index);
            ++row_index;
        }
        int centroid_index = 0;
        Eigen::Matrix<float, 1, 3> average = {0,0,0};
        // for each centroid, sum up all points associated
        for (auto cent : centroids.rowwise()) {
            for (const auto& key : assignments[centroid_index]) {
                average += points.row(key);
            }
            cent = average / (assignments[centroid_index].empty() ? 1 : assignments[centroid_index].size());
            ++centroid_index;
        }
    }
    std::cout << centroids << '\n';
    return centroids;
}