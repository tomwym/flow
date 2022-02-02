#pragma once

#include <string>
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "display.hpp"

class State;

class Window : public Display {
public:
    Window(const std::string& title);
    void ClearScreen(const float r, const float g, const float b, const float a);
    void Update();
    void UpdateTransitions();
    void LimitFrames();
    bool IsClosed() const;
    void SetIsClosed(bool val);
    bool isClosed;
    virtual ~Window();
protected:
private:
    void operator=(const Window& Window);
    Window(const Window& Window);

    static const int m_fps = 60;
    constexpr static float m_frameDelay = 1000/m_fps;
    GLuint m_frameStart = 0;
    int m_frameTime = 0;

    SDL_Window* m_window;
    SDL_GLContext m_glContext;
};

