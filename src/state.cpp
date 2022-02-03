#include "state.hpp"

#include "functions.hpp"

// static initializations
std::set<char> State::seen = {'0'};
std::map<char, State* const> State::masterkey = {};
Geometry* State::geom = nullptr;
Mesh* State::mesh = nullptr;

void State::setGeom(Geometry* const _geom) {
    geom = _geom;
}

void State::setMesh(Mesh* const _mesh) {
    mesh = _mesh;
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
    case SDLK_o:
        this->Orient();
        break;
    case SDLK_r:
        this->Resize();
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

void State0::Orient() {
    std::cout << "state 0 orient" << '\n';
}

void State0::Resize() {
    geom->CollectPlanarData();
    mesh->UpdateStatic(Geometry::VectorFromEigen(geom->GetPlanarData()));
}

void State0::IncrementAxis() {
    const size_t n_axis = geom->m_rotation_vals.size()-1;
    const size_t incremented = ++(geom->current_axis);
    if (incremented > n_axis) {
        geom->current_axis = 0;
    }
}

void State0::DecrementAxis() {
    const size_t n_axis = geom->m_rotation_vals.size()-1;
    const int decremented = --(geom->current_axis);
    if (decremented < 0) {
        geom->current_axis = n_axis;
    }
}

void State0::IncrementRotation() {
    geom->m_rotation_vals[geom->current_axis] += rotation_step;
}

void State0::DecrementRotation() {
    geom->m_rotation_vals[geom->current_axis] -= rotation_step;
}