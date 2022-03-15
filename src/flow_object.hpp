#pragma once

#include <vector>
#include <concepts>
#include <cmath>
#include <iostream>
#include <limits>
#include <glm/glm.hpp>

#include "display.hpp"
#include "particle.hpp"

template<typename T>
concept ArrayLike = requires(T v) {
    {v[0]};
    requires (sizeof(T)/sizeof(v[0]) >= 2);
};

using arr3 = std::array<float,3>;
/*
template <ArrayLike T, ArrayLike S>
T operator+(const T& a, const S& b) {
    return T{a[0]+b[0],a[1]+b[1],a[2]+b[2]};
}

template <ArrayLike T, ArrayLike S>
T operator-(const T& a, const S& b) {
    return T{a[0]-b[0],a[1]-b[1],a[2]-b[2]};
}

template <ArrayLike T, typename S>
T operator*(const T& a, const S& b) {
    return T{a[0]*b,a[1]*b,a[2]*b};
}
*/

template<ArrayLike T>
float Norm(const T& a, const T& b) {
    const float dx = a[0] - b[0];
    const float dy = a[1] - b[1];
    const float dz = a[2] - b[2];
    return std::sqrt(dx*dx + dy*dy + dz*dz);
}

template<ArrayLike T>
float Norm(const T& a) {
    const float dx = a[0];
    const float dy = a[1];
    const float dz = a[2];
    return std::sqrt(dx*dx + dy*dy + dz*dz);
}

template<typename T>
float Dot(const T& a, const T& b) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

template <ArrayLike T>
struct Segment : Display {
    T p1;
    T p2;
    T vector;
    float length;
    T outter_norm;
    Segment(const T& _p1, const T& _p2)
    : p1(_p1)
    , p2(_p2)
    , vector({0,0,0})
    , length(0)
    , outter_norm({0,0,0}) {
        const float dx = (p2[0] - p1[0]);
        const float dy = (p2[1] - p1[1]);
        vector[0] = dx;
        vector[1] = dy;
        length = std::sqrt(dx*dx + dy*dy);
        outter_norm[0] = dy/length;
        outter_norm[1] = -dx/length;
    }
};

template <ArrayLike T>
class FlowObject {
public:
    FlowObject(const std::vector<T>& pointsList)
    : m_pointsList(pointsList) {
        m_segmentsList.reserve(m_pointsList.size()-1);
        for (auto it=m_pointsList.begin(); it!=m_pointsList.end()-1; ++it) {
            m_segmentsList.push_back(Segment<T>(*it, *(it+1)));
        }
    }
    bool ParticleInObject(const Particle& p) {
        float testx = p.position[0];
        float testy = p.position[1];
        bool c = false;
        const int nvert = m_pointsList.size();
        int i = 0, j = nvert-1;
        for (; i < nvert; j = i++) {
            const float vertx_now = m_pointsList[i][0];
            const float vertx_prev = m_pointsList[j][0];
            const float verty_now = m_pointsList[i][1];
            const float verty_prev = m_pointsList[j][1];
            if ( ((verty_now>testy) != (verty_prev>testy)) &&
                (testx < (vertx_prev-vertx_now) * (testy-verty_now) /
                        (verty_prev-verty_now) + vertx_now) )
                c = !c;
        }
        return c;
    }

    T FindClosestSeeegment(const T& pt) {
        float min_dist = std::numeric_limits<float>::max();
        T dir_to_nearest {0,0,0};
        int idx = 0;
        for (auto it=m_pointsList.begin(); it!=m_pointsList.end()-1; ++it) {
            T p1 = *it;
            T p2 = *(it+1);

            const T v1 = p2-p1;
            const T v2 = pt-p1;
            const float v2_proj_v1 = Dot<T>(v1, v2)/(Norm<T>(v1)*Norm<T>(v1));
            const T v3 = v1*v2_proj_v1;
            const T v4 = v3-v2;
            const float dist = Norm<T>(v4);

            if (dist < min_dist) {
                min_dist = dist;
                dir_to_nearest = v4;
            }
            ++idx;
        }
        return dir_to_nearest;
    }

    T FindClosestSegment(const T& pt) {
        float min_dist = std::numeric_limits<float>::max();
        T dir_to_nearest {0,0,0};
        int index = -1;
        for (int idx=0; const auto& seg : m_segmentsList) {
            const T& v1 = seg.vector;
            const T v2 = pt - seg.p1;
            const float v2_proj_v1 = Dot<T>(v1, v2)/Norm(v1);
            const T v3 = v1*v2_proj_v1;
            const T v4 = v2-v3;
            const float dist = Norm<T>(v4);

            if (dist < min_dist) {
                index = idx;
                min_dist = dist;
                dir_to_nearest = v4;
            }
            ++idx;
        }
        return dir_to_nearest;
    }
protected:
private:
    FlowObject() = delete;
    std::vector<T> m_pointsList;
    std::vector<Segment<T>> m_segmentsList;
};