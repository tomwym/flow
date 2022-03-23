#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <numbers>
#include <random>
#include <glm/glm.hpp>
#include "Eigen/Dense"

#include "flow_object.hpp"
#include "display.hpp"
#include "particle.hpp"

class Fluids : public Display {
public:
    Fluids(FlowObject<glm::vec3> flowObj)
    : m_flowObj(flowObj) {}
    //void MoveToInlet(std::vector<Particle>& particles);
    void MoveToInlet(std::vector<Particle*>& particles);
    void InitializeSPH();
    void ComputeRhoP();
    void ComputeForces();
    void Integrate(const int iteration);

    const std::vector<Particle*>& getNearfieldPtrP() const {
        return m_nearfieldPtrP;
    }

    // container containing all particles
    std::vector<Particle> m_particles;
    // container containing particle pointers for all particles in view
    std::vector<Particle*> m_pparticles;
    // buffer of emptied particles
    std::vector<Particle*> m_emplacePtrP;
    // store the nearfield particles for integration
    std::vector<Particle*> m_nearfieldPtrP;

    std::vector<glm::vec3> MakeParticlesDrawable();
    FlowObject<glm::vec3> m_flowObj;
protected:
private:
    // interaction
    const int MAX_PARTICLES = 1000;

    // "Particle-Based Fluid Simulation for Interactive Applications"
    // solver parameters
    const Eigen::Vector3f G = {0,0,0};
    const float REST_DENS = 100.f;		 // rest density
    const float GAS_CONST = 500.f;		 // const for equation of state
    const float H = 4.f;				 // kernel radius
    const float HSQ = H * H;				 // radius^2 for optimization
    const float MASS = 8.f;				 // assume all particles have the same mass
    const float VISC = 200.f;			 // viscosity constant
    const float DT = 1.f/m_fps;			 // integration timestep

    // smoothing kernels defined in Müller and their gradients
    const float POLY6 = 4.f / (std::numbers::pi * pow(H, 8.f));
    const float SPIKY_GRAD = -10.f / (std::numbers::pi * pow(H, 5.f));
    const float VISC_LAP = 40.f / (std::numbers::pi * pow(H, 5.f));

    // simulation parameters
    const float EPS = H; // boundary epsilon
    const float BOUND_DAMPING = 0.8f;
    const int X_INIT = -m_width/2+5*H;

    Fluids() = delete;
    Fluids(const Fluids& other) = delete;
    void operator=(const Fluids& other) = delete;

};