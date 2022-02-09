#include "state.hpp"

#include <fstream>
#include "functions.hpp"

// static initializations
std::set<char> State::seen = {'0'};
std::map<char, State* const> State::masterkey = {};
Geometry* State::geom = nullptr;
Mesh* State::mesh = nullptr;
Transform* State::transform = nullptr;

void State::setGeom(Geometry* const _geom) {
    geom = _geom;
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