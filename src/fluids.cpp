#include "fluids.hpp"


void Fluids::MoveToInlet(std::vector<Particle*>& particles) {
	static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(-10, 10);
	int x = X_INIT;
	int y = 0;
	int sign_bit = 1;
	int ii = 0;
	for (auto it=particles.begin(); it!=particles.end(); ++it) {
		Particle* itp = *it;
		itp->InitVelocity();
		itp->force = {0,0,0};
        y += sign_bit*2*(ii++)*H;
        if (std::abs(y) > m_height/2*0.8) {
            y = 0;
			x += 2*H;
            ii = 1;
        }
		itp->position(0) = x + dis(gen);
		itp->position(1) = y + dis(gen);
        sign_bit *= -1;
	}
}


void Fluids::InitializeSPH() {
    //m_particles.reserve(MAX_PARTICLES);
    m_particles = std::vector<Particle>(MAX_PARTICLES, Particle());
	m_pparticles.reserve(MAX_PARTICLES);
	for (auto& p : m_particles) {
		m_pparticles.push_back(&p);
	}
	MoveToInlet(m_pparticles);
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
		// std::cout << iti->p << ' ';
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
		//const Eigen::Vector3f GForce = G*MASS/iti->rho;
		iti->force = fpress + fvisc; // + GForce;
	}
}


void Fluids::Integrate(const int iteration)
{
	const auto temp = *m_pparticles[0];
	//std::cout << temp.position(1) << ' ' << temp.position(1) << ' ' <<  temp.position(2) << '\n';
	std::vector<Particle*> keepPtrs;
	std::vector<Particle*> removePtrs;
	std::vector<Particle*> nearfieldPtrs;
	for (auto& p : m_pparticles) {
		bool keep = true;
		// forward Euler integration
		p->position += DT * p->velocity;
		p->velocity += DT * p->force / p->rho;

		// enforce boundary conditions
		// object interaction
		if (p->position(0) < 1*m_height && p->position(0) > -1*m_height &&
			p->position(1) < 1*m_height && p->position(1) > -1*m_height) {
			nearfieldPtrs.push_back(p);
			bool pnp = m_flowObj.ParticleInObject(*p);
			if (pnp) {
				auto dir = m_flowObj.FindClosestSegment({p->position(0), p->position(1),0});
				p->position(0) += dir[0];
				p->position(1) += dir[1];
			}
		}
		// left x: -m_width/2 -> 0
		if (p->position(0) - EPS < -m_width/2) {
			keep = false;
			removePtrs.push_back(p);
		}
		// right x: 0 -> m_width/2
		if (p->position(0) + EPS > m_width/2) {
			keep = false;
			removePtrs.push_back(p);
		}
		// bottom y: -m_height/2 -> 0
		if (p->position(1) - EPS < -m_height/2) {
			p->velocity(0) *= BOUND_DAMPING;
			p->velocity(1) = BOUND_DAMPING*std::abs(p->velocity(1));
		}
		// top y: 0 -> m_height/2
		if (p->position(1) + EPS > m_height/2) {
			p->velocity(0) *= BOUND_DAMPING;
			p->velocity(1) = -BOUND_DAMPING*std::abs(p->velocity(1));
		}
		if (keep) {
			keepPtrs.push_back(p);
		}
	}
	m_pparticles = std::move(keepPtrs);
	for (auto& p : removePtrs) {
		p->position = {0,0,0};
	}
	m_emplacePtrP.insert(m_emplacePtrP.end(), removePtrs.begin(), removePtrs.end());
	m_nearfieldPtrP = std::move(nearfieldPtrs);
    //MoveToInlet(particlePtrs);
}

/*
 * Given a vector of particles in Pixel coordinates, return the full vector of
 * OpenGL vertices
 */
std::vector<glm::vec3> Fluids::MakeParticlesDrawable() {
    std::vector<glm::vec3> out;
    out.reserve(m_pparticles.size()*Particle::NUM_POINTS_PER_PARTICLE);

    for (const auto& p : m_pparticles) {
        float x = p->position(0);
        float y = p->position(1);

        float normalized_x = x/m_height;
        float normalized_y = y/m_height;
		float r = H/m_height;

        out.push_back({normalized_x,normalized_y,0});

        for (float i=0; i<=2*std::numbers::pi; i+= 2*std::numbers::pi/Particle::NUM_POINTS_PER_PARTICLE) {
            out.push_back({r*std::cos(i) + normalized_x, r*std::sin(i) + normalized_y, 0});
        }
    }
    return out;
}

