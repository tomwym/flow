#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <numbers>
#include <glm/glm.hpp>
#include "Eigen/Dense"

#include "display.hpp"
#include "particle.hpp"

class Fluids : public Display {
public:
    Fluids() {};
    void MoveToInlet(std::vector<Particle>& particles);
    void MoveToInlet(std::vector<Particle*>& particles);
    void InitializeSPH();
    void ComputeRhoP();
    void ComputeForces();
    void Integrate();
    std::vector<Particle> m_particles;

    std::vector<glm::vec3> MakeParticlesDrawable();
protected:
private:
    // interaction
    const int MAX_PARTICLES = 30;
    const int DAM_PARTICLES = 100;
    const int BLOCK_PARTICLES = 100;

    // "Particle-Based Fluid Simulation for Interactive Applications"
    // solver parameters
    const float REST_DENS = 1000.f;		 // rest density
    const float GAS_CONST = 2000.f;		 // const for equation of state
    const float H = 16.f;				 // kernel radius
    const float HSQ = H * H;				 // radius^2 for optimization
    const float MASS = 65.f;				 // assume all particles have the same mass
    const float VISC = 250.f;			 // viscosity constant
    const float DT = 1.f/60.f;			 // integration timestep

    // smoothing kernels defined in Müller and their gradients
    const float POLY6 = 315.f / (65.f * std::numbers::pi * pow(H, 9.f));
    const float SPIKY_GRAD = -45.f / (std::numbers::pi * pow(H, 6.f));
    const float VISC_LAP = 45.f / (std::numbers::pi * pow(H, 6.f));

    // simulation parameters
    const float EPS = H; // boundary epsilon
    const float BOUND_DAMPING = -0.2f;
    const int X_INIT = -m_width/2+2*H;

    Fluids(const Fluids& other) = delete;
    void operator=(const Fluids& other) = delete;

};