#pragma once

#include <vector>
#include <list>
#include <concepts>
#include <cmath>
#include <iostream>
#include <limits>
#include <ranges>
#include <queue>
#include <glm/glm.hpp>

#include "display.hpp"
#include "particle.hpp"

template<typename T>
concept ArrayLike = requires(T v) {
    {v[0]};
    requires (sizeof(T)/sizeof(v[0]) >= 2);
};

using arr3 = std::array<float,3>;

template <ArrayLike T, ArrayLike S>
T operator+(const T& a, const S& b) {
    return T({a[0]+b[0],a[1]+b[1],a[2]+b[2]});
}

template <ArrayLike T, ArrayLike S>
T operator-(const T& a, const S& b) {
    return T({a[0]-b[0],a[1]-b[1],a[2]-b[2]});
}
/*
template <ArrayLike T, typename S>
T operator*(const T& a, const S& b) {
    return ({a[0]*b, a[1]*b, a[2]*b});
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

template<ArrayLike T>
float Dot(const T& a, const T& b) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

template<ArrayLike T>
T Cross(const T& a, const T& b) {
    return {
        a[0]*b[2] - a[2]*b[0],
        a[2]*b[0] - a[0]*b[2],
        a[0]*b[1] - a[1]*b[0]};
}


template <ArrayLike T>
struct Segment : Display {
    T p1;
    T p2;
    T vector;
    T midpoint;
    float length;
    T inner_norm;
    T outter_norm;
    static constexpr float slice_width = 0.001;

    Segment(const T& _p1, const T& _p2)
    : p1(_p1)
    , p2(_p2)
    , vector({0,0,0})
    , midpoint({0,0,0})
    , length(0)
    , outter_norm({0,0,0}) {
        const float dx = (p2[0] - p1[0]);
        const float dy = (p2[1] - p1[1]);

        vector = {dx, dy, 0};
        midpoint = {p1[0]+dx/2, p1[0]+dy/2, 0};
        length = std::sqrt(dx*dx + dy*dy);

        const float normx = dy/length;
        const float normy = dx/length;
        // get cross product to determine which vec points in, and which out
        // cross vector with a_norm, if cross[2] (z) is > 0 then it's inner
        const T a_norm = {normx, -normy, 0};
        const T b_norm = {-normx, normy, 0};
        const T cross = Cross<T>(vector, a_norm);
        if (cross[2] > 0) {
            inner_norm = a_norm;
            outter_norm = b_norm;
        } else {
            inner_norm = b_norm;
            outter_norm = a_norm;
        }
    }
};

struct DistanceNodeP {
    float dist;
    Particle* particle;

    DistanceNodeP(const float _dist, Particle* _particle)
    : dist(_dist)
    , particle(_particle) {}

    DistanceNodeP(const DistanceNodeP& other) {
        this->dist = other.dist;
        this->particle = other.particle;
    }

    DistanceNodeP& operator=(DistanceNodeP&& other) {
        if (this != &other) {
            std::swap(this->dist, other.dist);
            std::swap(this->particle, other.particle);
        }
        return *this;
    }
};

template <ArrayLike T>
class FlowObject {
public:
    // points list should be a closed loop of points, going COUNTERCLOCKWISE
    // the first and last point should be identical
    FlowObject(const std::vector<T>& pointsList, const float _characteristic_length)
    : m_pointsList(pointsList), characteristic_length(_characteristic_length) {
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

    // OUTDATED DEPRACATED DO NOT USE
    T FindClosestSeeegment(const T& pt) {
        float min_dist = std::numeric_limits<float>::max();
        T dir_to_nearest {0,0,0};
        int idx = 0;
        for (auto it=m_pointsList.begin(); it!=m_pointsList.end()-1; ++it) {
            T p1 = *it;
            T p2 = *(it+1);

            const T v1 = p2-p1; // the line segment itself
            const T v2 = pt-p1; // the segment from line to point
            const T v3 = v1*Dot<T>(v1, v2)/(Norm<T>(v1)*Norm<T>(v1));
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

    // returns a vector to move the particle to the boundary
    T FindClosestSegment(const T& pt) {
        float min_dist = std::numeric_limits<float>::max();
        T dir_to_nearest {0,0,0};
        for (auto& seg : m_segmentsList) {
            const T v1 = seg.vector; // the line segment itself
            const T v2 = pt-seg.p1; // the segment from line to point
            const T v3 = v1*Dot<T>(v1, v2)/(Norm<T>(v1)*Norm<T>(v1));
            const T v4 = v3-v2;
            const float dist = Norm<T>(v4);

            if (dist < min_dist) {
                min_dist = dist;
                dir_to_nearest = v4;
            }
        }
        return dir_to_nearest;
    }

    T GetBodyForce(const std::vector<Particle*>& nearfieldParticles) {
        T force = {0,0,0};
        for (const auto& seg : m_segmentsList) {
            // sort in increasing order
            auto cmp = [](const DistanceNodeP& left, const DistanceNodeP& right) { return (left.dist) < (right.dist); };
            std::priority_queue<DistanceNodeP, std::vector<DistanceNodeP>, decltype(cmp)> q(cmp);

            for (const auto& pp : nearfieldParticles) {
                // grab the vector between the particle and the midpoint of the segment
                //std::cout << pp->position[0] << ' ' << pp->position[1] << "//"
                //          << seg.midpoint[0] << ' ' << seg.midpoint[1] << '\n';

                auto temp2 = pp->position - seg.midpoint;
                //std::cout << temp2[0] << ' ' << temp2[1] << '\n';
                const float dist = Norm<Eigen::Vector3f>(temp2);
                q.push(DistanceNodeP(dist, pp));

                if (q.size() > 8) {
                    q.pop();
                }
            }

            float p = 0;
            // naive pressure implementation
            while (!q.empty()) {
                p += q.top().particle->p;
                q.pop();
            }
            p /= 8.0f;
            const T force_on_area = seg.inner_norm * p * seg.slice_width;
            force = force + force_on_area;
        }
        return force;
    }
protected:
private:
    FlowObject() = delete;
    std::vector<T> m_pointsList;
    std::vector<Segment<T>> m_segmentsList;
    float characteristic_length;
};