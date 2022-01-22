#pragma once

#include <string>
#include <SDL2/SDL.h>

class Display {
public:
    Display(const int width, const int height, const std::string& title);
    void ClearScreen(const float r, const float g, const float b, const float a);
    void Update();
    bool IsClosed() const;
    virtual ~Display();
protected:
private:
    void operator=(const Display& display);
    Display(const Display& display);

    SDL_Window* m_window;
    SDL_GLContext m_glContext;
    bool isClosed;
};