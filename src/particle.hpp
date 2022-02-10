#pragma once

class Particle {
public:
    Particle()
    : position({0,0,0})
    , velocity({300,0,0})
    , force({0,0,0})
    , rho(0)
    , p(0) {}
    Eigen::Vector3f position;
    Eigen::Vector3f velocity;
    Eigen::Vector3f force;
    float rho;   // density
    float p;     // pressure

    static const int NUM_POINTS_PER_PARTICLE = 8; // -> +1 on peterimeter
protected:
private:

};