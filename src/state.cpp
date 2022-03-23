#include "state.hpp"

#include <fstream>
#include <algorithm>
#include <numeric>
#include <future>
#include "fluids.hpp"
#include "functions.hpp"

// static initializations
std::set<char> State::seen = {'0'};
std::map<char, State* const> State::masterkey = {};
Geometry* State::geom = nullptr;
Window* State::wind = nullptr;
Mesh* State::mesh = nullptr;
Transform* State::transform = nullptr;

void State::setGeom(Geometry* const _geom) {
    geom = _geom;
}

void State::setWind(Window* const _wind) {
    wind = _wind;
}

void State::setMesh(Mesh* const _mesh) {
    mesh = _mesh;
}

void State::setTransform(Transform* const _transform) {
    transform = _transform;
}

void State::setMasterkey(std::map<char, State* const> mep) {
    std::swap(masterkey, mep);
}

State* State::Transition(const char nextkey) {
    bool transiioned = false;
    State* out = this;
    if (this->nexts.contains(nextkey)) {
        out = this->nexts[nextkey];
        transiioned = true;
    } else if (this->key>nextkey || this->seen.contains(nextkey)) {
        this->nexts[nextkey] = masterkey[nextkey];
        out = this->nexts[nextkey];
        transiioned = true;
    }
    if (transiioned) {
        this->seen.insert(this->key);
        std::cout << out->m_name << '\n';
    } else {
        std::cout << "did not transition for " << nextkey << '\n';
    }
    return out;
}

State* State::Spin(Window& wind) {
    SDL_Event e;
    State* temp = this;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            wind.SetIsClosed(true);
        } else if (e.type == SDL_KEYDOWN) {
            // std::cout << "keypress" << ' ';
            switch (e.key.keysym.sym) {
            case SDLK_0:
                temp = this->Transition('0');
                break;
            case SDLK_1:
                temp = this->Transition('1');
                break;
            case SDLK_2:
                temp = this->Transition('2');
                break;
            case SDLK_3:
                temp = this->Transition('3');
                break;
            default:
                {} //std::cout << "not recognized\n";
            }
            this->StateSpecific(e.key.keysym.sym);
        }
    }
    return temp;
}


void State0::StateSpecific(const SDL_Keycode k) {
    //std::cout << "in state 0 statespecific" << '\n';
    switch (k) {
    case SDLK_p:
        this->Show3D();
        break;
    case SDLK_o:
        this->Show2D();
        break;
    case SDLK_i:
        // this->ReducePoints();
        break;
    case SDLK_u:
        this->SearchForBoundary();
        break;
    case SDLK_UP:
        this->IncrementAxis();
        break;
    case SDLK_DOWN:
        this->DecrementAxis();
        break;
    case SDLK_RIGHT:
        this->IncrementRotation();
        break;
    case SDLK_LEFT:
        this->DecrementRotation();
        break;
    default:
        {} //std::cout << "state specific not found\n";
    }
}

// p
void State0::Show3D() {
    transform->UpdateRotation<Eigen::Vector3f>(geom->m_rotation_vals);
    std::cout << geom->m_rotation_vals.transpose() << '\n';
    mesh->UpdateStatic(Geometry::VectorFromEigen(geom->GetNormalizedData()));
    mesh->SetDrawStaticPrimitive(GL_POINTS);
}

// o
void State0::Show2D() {
    geom->CollectPlanarData();
    transform->UpdateRotation<std::vector<float>>({0,0,0});
    std::cout << geom->m_rotation_vals.transpose() << '\n';
    // mesh->UpdateStatic(Geometry::VectorFromEigen(geom->GetPlanarData()));
    mesh->UpdateStatic(Geometry::VectorFromEigen(Geometry::ReduceSize(5000, geom->GetPlanarData())));
    mesh->SetDrawStaticPrimitive(GL_POINTS);

    /*
    const Eigen::MatrixXf obj = Geometry::ReduceSize(5000, geom->GetPlanarData());
    std::ofstream o("temp.dat");
    for (const auto& row : obj.rowwise()) {
        o << row << '\n';
    }
    o.close();
    */
}

// i // DOES NOT WORK PROPERLY
void State0::ReducePoints() {
    geom->CollectClusteredData(geom->GetReducedPlanarData());
    transform->UpdateRotation<std::vector<float>>({0,0,0});
    mesh->UpdateStatic(Geometry::VectorFromEigen(geom->GetClusteredData()));
    mesh->SetDrawStaticPrimitive(GL_POINTS);
}

// u
void State0::SearchForBoundary() {
    geom->CollectBoundaryPoints();
    transform->UpdateRotation<std::vector<float>>({0,0,0});
    mesh->UpdateStatic(Geometry::VectorFromEigen(geom->GetBoundaryPoints()));
    mesh->SetDrawStaticPrimitive(GL_LINE_STRIP);
}

void State0::IncrementAxis() {
    const size_t axis_max_index = geom->m_rotation_vals.size()-1;
    const size_t incremented = geom->GetCurrentAxis()+1;
    if (incremented > axis_max_index) {
        geom->SetCurrentAxis(0);
    } else {
        geom->SetCurrentAxis(incremented);
    }
}

void State0::DecrementAxis() {
    const size_t axis_max_index = geom->m_rotation_vals.size()-1;
    const int decremented = geom->GetCurrentAxis()-1;
    if (decremented < 0) {
        geom->SetCurrentAxis(axis_max_index);
    } else {
        geom->SetCurrentAxis(decremented);
    }
}

void State0::IncrementRotation() {
    geom->m_rotation_vals[geom->GetCurrentAxis()] += this->rotation_step;
    transform->UpdateRotation<Eigen::Vector3f>(geom->m_rotation_vals);
}

void State0::DecrementRotation() {
    geom->m_rotation_vals[geom->GetCurrentAxis()] -= rotation_step;
    transform->UpdateRotation<Eigen::Vector3f>(geom->m_rotation_vals);
}

void State1::StateSpecific(const SDL_Keycode k) {
    //std::cout << "in state 0 statespecific" << '\n';
    switch (k) {
    case SDLK_l:
        this->Hello();
        break;
    default:
        {} //std::cout << "state specific not found\n";
    }
}

// l
void State1::Hello() {
    transform->UpdateRotation<std::vector<float>>({0,0,0});
    geom->GetCirclePrimitive();
    mesh->UpdateStatic(geom->m_primitiveCircleScaled);
    // mesh->UpdateStatic(Geometry::VectorFromEigen(geom->GetBoundaryPoints()));
    mesh->SetDrawStaticPrimitive(GL_LINE_STRIP);

    mesh->SetDrawDynamicPrimitive(GL_TRIANGLE_FAN);
    // initialize the flow object with the geometry of boundary nodes
    // FlowObject<glm::vec3> fo(geom->m_primitiveCircle, 0.1);
    // auto temp = Geometry::VectorFromEigen(geom->GetBoundaryPoints());
    std::vector<glm::vec3> temp = geom->m_primitiveCircle;
    // std::cout << " temp 1 " << '\n';
    // for (const auto& v : temp) {
    //     std::cout << v[0] << ' ' << v[1] << '\n';
    // }
    // std::for_each(temp.begin(), temp.end(),
    //               [m_width, m_height](glm::vec3& v){ v[0] *= m_width/2; v[1] *= m_height/2; });
    // std::cout << " temp 2 " << '\n';
    // for (const auto& v : temp) {
    //     std::cout << v[0] << ' ' << v[1] << '\n';
    // }
    //auto temp = geom->m_primitiveCircle;
    FlowObject<glm::vec3> fo(temp, 0.1);
    Fluids fluidhandle(fo);
    fluidhandle.InitializeSPH();

    const int seconds = 3;
    const int iterations = m_fps*seconds;

    for (int i=0; i<iterations; ++i) {
        auto calcforce = [i](){return (i % 60 == 0 && i > 0);};
        std::future<glm::vec3> calcf;
        if (calcforce()) {
            calcf = std::async(std::launch::async, &FlowObject<glm::vec3>::GetBodyForce, fo, fluidhandle.getNearfieldPtrP());
        }
        fluidhandle.ComputeRhoP();
        fluidhandle.ComputeForces();
        fluidhandle.Integrate(i);
        std::vector<glm::vec3> fdsa = fluidhandle.MakeParticlesDrawable();
		wind->ClearScreen("light blue");
        mesh->UpdateDynamic(fdsa);
        mesh->DrawNArrays(fdsa.size(), Particle::NUM_POINTS_PER_PARTICLE+2);
        wind->Update();
        wind->LimitFrames();
        if (calcforce()) {
            const glm::vec3 body_force = calcf.get();
            std::cout << "body force: " << body_force[0] << ' ' << body_force[1] << '\n';
        }
    }
    mesh->UpdateDynamic({});
}