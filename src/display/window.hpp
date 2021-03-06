#pragma once

#include <string>
#include <map>
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "display.hpp"

class State;

class Window : public Display {
public:
    Window(const std::string& title);
    void ClearScreen(const int r, const int g, const int b, const float a);
    void ClearScreen(const std::string& color_name);
    void Update();
    void UpdateTransitions();
    void LimitFrames();
    bool IsClosed() const;
    void SetIsClosed(bool val);
    bool isClosed;
    virtual ~Window();

    static const std::map<std::string, std::array<float, 4>> m_colormap;

protected:
private:
    void operator=(const Window& Window);
    Window(const Window& Window);

    constexpr static float m_frameDelay = 1000/m_fps;
    GLuint m_frameStart = 0;
    int m_frameTime = 0;

    SDL_Window* m_window;
    SDL_GLContext m_glContext;
};

