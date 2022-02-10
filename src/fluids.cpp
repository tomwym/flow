#include "fluids.hpp"

void Fluids::MoveToInlet(std::vector<Particle>& particles) {
	int x = X_INIT;
	int y = 0;
	int _y = 0;
	int sign_bit = 1;
	int y_i = m_height/(4*H);
	for (auto it=particles.begin(); it!=particles.end(); ++it) {
		if (y_i <= 0) {
			y_i = m_height/(4*H);
			y = 0;
			_y = 0;
			x += 2*H;
		}
		--y_i;
		it->position(0) = x;
		_y += sign_bit * y;
		it->position(1) = _y;
		y += 2*H;
		sign_bit *= -1;
	}
}

void Fluids::MoveToInlet(std::vector<Particle*>& particles) {
	int x = X_INIT;
	int y = 0;
	int _y = 0;
	int sign_bit = 1;
	int y_i = m_height/(4*H);
	for (auto it=particles.begin(); it!=particles.end(); ++it) {
		Particle* itp = *it;
		itp->velocity = {300,0,0};
		if (y_i <= 0) {
			y_i = m_height/(4*H);
			y = 0;
			_y = 0;
			x += 2*H;
		}
		--y_i;
		itp->position(0) = x;
		_y += sign_bit * y;
		itp->position(1) = _y;
		y += 2*H;
		sign_bit *= -1;

	}
}


void Fluids::InitializeSPH() {
    //m_particles.reserve(MAX_PARTICLES);
    m_particles = std::vector<Particle>(MAX_PARTICLES, Particle());
	MoveToInlet(m_particles);
}

void Fluids::ComputeRhoP() {
	for (auto iti=m_particles.begin(); iti!=m_particles.end(); ++iti)
	{
		iti->rho = 0.f;
		for (auto itj=m_particles.begin(); itj!=m_particles.end(); ++itj)
		{
			Eigen::Vector3f rij = itj->position - iti->position;
			float r2 = rij.squaredNorm();

			if (r2 < HSQ)
			{
				// this computation is symmetric
				iti->rho += MASS * POLY6 * pow(HSQ - r2, 3.f);
			}
		}
		iti->p = GAS_CONST * (iti->rho - REST_DENS);
	}
}


void Fluids::ComputeForces(void)
{
	for (auto iti=m_particles.begin(); iti!=m_particles.end(); ++iti)
	{
		Eigen::Vector3f fpress(0.f, 0.f, 0.f);
		Eigen::Vector3f fvisc(0.f, 0.f, 0.f);
		for (auto itj=m_particles.begin(); itj!=m_particles.end(); ++itj) {
			if (iti == itj)
				continue;

			Eigen::Vector3f rij = itj->position - iti->position;
			float r = rij.norm();
			if (r < H) {
				// compute pressure force contribution
				fpress += -rij.normalized() * MASS * (iti->p + itj->p) / (2.f * itj->rho) * SPIKY_GRAD * pow(H - r, 2.f);
				// compute viscosity force contribution
				fvisc += VISC * MASS * (itj->velocity - iti->velocity) / itj->rho * VISC_LAP * (H - r);
			}
		}
		iti->force = fpress + fvisc;
	}
}


void Fluids::Integrate()
{
	std::vector<Particle*> particlePtrs;
	for (auto& p : m_particles) {
		// forward Euler integration
		p.position += DT * p.velocity;
		p.velocity += DT * p.force / p.rho;

		// enforce boundary conditions
		// left x: -m_width/2 -> 0
		if (p.position(0) - EPS < -m_width/2)
		{
			p.velocity(0) *= BOUND_DAMPING;
			particlePtrs.push_back(&p);
		}
		// right x: 0 -> m_width/2
		if (p.position(0) + EPS > m_width/2)
		{
			p.velocity(0) *= BOUND_DAMPING;
			particlePtrs.push_back(&p);
		}
		// bottom y: -m_height/2 -> 0
		if (p.position(1) - EPS < -m_height/2)
		{
			p.velocity(1) *= BOUND_DAMPING;
			particlePtrs.push_back(&p);
		}
		// top y: 0 -> m_height/2
		if (p.position(1) + EPS > m_height/2)
		{
			p.velocity(1) *= BOUND_DAMPING;
			particlePtrs.push_back(&p);
		}
		//std::cout << p.position(0) << ", " << p.position(1) << '\n';
	}
    MoveToInlet(particlePtrs);
}


std::vector<glm::vec3> Fluids::MakeParticlesDrawable() {
    std::vector<glm::vec3> out;
    out.reserve(m_particles.size()*Particle::NUM_POINTS_PER_PARTICLE);

    for (const auto& p : m_particles) {
        float x = p.position(0);
        float y = p.position(1);

        float normalized_x = 2*x/m_width;
        float normalized_y = 2*y/m_height;
        float rx = 2*H/m_width;
        float ry = 2*H/m_height;

        out.push_back({normalized_x,normalized_y,0});

        for (float i=0; i<=2*std::numbers::pi; i+= 2*std::numbers::pi/Particle::NUM_POINTS_PER_PARTICLE) {
            out.push_back({rx*std::cos(i) + normalized_x, ry*std::sin(i) + normalized_y, 0});
        }
    }
    return out;
}
