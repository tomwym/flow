#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <SDL2/SDL.h>

#include "window.hpp"
#include "geometry.hpp"
#include "mesh.hpp"
#include "transform.hpp"

class State {
public:
    State(const std::string& name, const char thiskey)
        : m_name(name)
        , key(thiskey) {}        
    State(const std::string& name, const char thiskey, State* const next)
        : m_name(name)
        , key(thiskey)
        , nexts({{(char)(thiskey+1), next}}) {}

    State* Transition(const char nextkey);
    State* Spin(Window& wind);
    virtual void StateSpecific(const SDL_Keycode k) {}

    static void setGeom(Geometry* const _geom);
    static void setMesh(Mesh* const _mesh);
    static void setTransform(Transform* const _transform);
    static void setMasterkey(std::map<char, State* const> mep);
protected:
    static Geometry* geom;
    static Mesh* mesh;
    static Transform* transform;
private:
    static std::set<char> seen;
    static std::map<char, State* const> masterkey;
    std::string m_name;
    char key;
    std::map<char, State*> nexts;
};


class State0 : public State {
public:
    State0(const std::string& name, const char thiskey)
        : State(name, thiskey) {}
    State0(const std::string& name, const char thiskey, State* const next)
        : State(name, thiskey, next) {}

    // State* Spin(Window& wind);
    void Orient();
    void Resize();
    void Show3D();
    void Show2D();
    void IncrementAxis();
    void DecrementAxis();
    void IncrementRotation();
    void DecrementRotation();
    void StateSpecific(const SDL_Keycode k);
protected:
private:
    const float rotation_step = M_PI/8;
};

class State1 : public State {
public:
    State1(const std::string& name, const char thiskey)
        : State(name, thiskey) {}
    State1(const std::string& name, const char thiskey, State* const next)
        : State(name, thiskey, next) {}
protected:
private:
};

class State2 : public State {
public:
    State2(const std::string& name, const char thiskey)
        : State(name, thiskey) {}
    State2(const std::string& name, const char thiskey, State* const next)
        : State(name, thiskey, next) {}
protected:
private:
};

class State3 : public State {
public:
    State3(const std::string& name, const char thiskey)
        : State(name, thiskey) {}
    State3(const std::string& name, const char thiskey, State* const next)
        : State(name, thiskey, next) {}
protected:
private:
};
