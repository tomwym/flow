#include "state.hpp"

// static initializations
std::set<char> State::seen = {'0'};
std::map<char, State*> State::masterkey = {};

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
            std::cout << "keypress" << ' ';
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
                std::cout << "not recognized\n";
            }
            StateSpecific(e.key.keysym.sym);
        }
    }
    return temp;
}


State* State0::Transition(const char nextkey) {
    State* temp = State::Transition(nextkey);
    std::cout << "state0 transition" << '\n';
    return temp;
}


void State0::StateSpecific(const SDL_Keycode k) {
    switch (k) {
    case SDLK_o:
        this->Orient();
        break;
    case SDLK_r:
        this->Resize();
        break;
    default:
        std::cout << "state specific not found\n";
    }
}

void State0::Orient() {
    std::cout << "state 0 orient" << '\n';
}

void State0::Resize() {
    std::cout << "state 0 resize" << '\n';
}