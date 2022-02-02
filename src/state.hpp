#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <SDL2/SDL.h>

#include "window.hpp"

class State {
public:
    State(const std::string& name, const char thiskey)
        : m_name(name)
        , key(thiskey) {}        
    State(const std::string& name, const char thiskey, State* const next)
        : m_name(name)
        , key(thiskey)
        , nexts({{(char)(thiskey+1), next}}) {}

    static std::set<char> seen;
    static std::map<char, State*> masterkey;
    std::string m_name;
    char key;
    std::map<char, State*> nexts;

    virtual State* Transition(const char nextkey);
    virtual State* Spin(Window& wind);
    virtual void StateSpecific(const SDL_Keycode k) {}
protected:
private:
};


class State0 : public State {
public:
    State0(const std::string& name, const char thiskey)
        : State(name, thiskey) {}
    State0(const std::string& name, const char thiskey, State* const next)
        : State(name, thiskey, next) {}

    State* Transition(const char nextkey);
    // State* Spin(Window& wind);
    void Orient();
    void Resize();
    void StateSpecific(const SDL_Keycode k);
protected:
private:
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
