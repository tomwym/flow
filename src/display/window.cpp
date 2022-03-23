#include "window.hpp"

#include <iostream>
#include <array>

// printing function for associated colors
template <typename T, size_t val>
std::ostream& operator<<(std::ostream &ostr, const std::array<T, val>& arr) {
    for (const auto& v : arr) {
        ostr << v << ' ';
    }
    ostr << '\n';
    return ostr;
}


const std::map<std::string, std::array<float, 4>> Window::m_colormap = {
    {"red", {1.f, 0.f, 0.f, 1.f}},
    {"green", {0.f, 1.f, 0.f, 1.f}},
    {"blue", {0.f, 0.f, 1.f, 1.f}},
    {"light blue", {173.f/255.f, 216.f/255.f, 230.f/255.f, 1.f}},
    {"black", {0.f, 0.f, 0.f, 1.f}}
};

Window::Window(const std::string& title) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    m_window = SDL_CreateWindow(title.c_str(),
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                m_width,
                                m_height,
                                SDL_WINDOW_OPENGL);
    m_glContext = SDL_GL_CreateContext(m_window);

    GLenum res = glewInit();
    if (res != GLEW_OK) {
        std::cerr << "Glew failed to initalize" << std::endl;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    isClosed = false;

}

Window::~Window() {
    SDL_GL_DeleteContext(m_glContext);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Window::ClearScreen(const int r, const int g, const int b, const float a) {
    m_frameStart = SDL_GetTicks();
    glClearColor(r/255.f, g/255.f, b/255.f, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::ClearScreen(const std::string& color_name) {
    const std::array<float, 4>& ca = Window::m_colormap.at(color_name);
    m_frameStart = SDL_GetTicks();
    glClearColor(ca[0], ca[2], ca[3], ca[4]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::Update() {
    SDL_GL_SwapWindow(m_window);
}

void Window::UpdateTransitions() {
    SDL_GL_SwapWindow(m_window);
SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            isClosed = true;
        } else if (e.type == SDL_KEYDOWN) {
            std::cout << "keypress" << ' ';
            switch (e.key.keysym.sym) {
            case SDLK_0: std::cout << "0\n";
                    break;
            case SDLK_1: std::cout << "1\n";
                    break;
            case SDLK_2: std::cout << "2\n";
                    break;
            case SDLK_3: std::cout << "3\n";
                    break;
            default: std::cout << "not recognized\n";
            }
        }
    }
}

void Window::LimitFrames() {
    m_frameTime = SDL_GetTicks() - m_frameStart;
    if (m_frameDelay > m_frameTime) {
        SDL_Delay(m_frameDelay - m_frameTime);
    }
}

bool Window::IsClosed() const {
    return isClosed;
}

void Window::SetIsClosed(bool val) {
    isClosed = val;
}
