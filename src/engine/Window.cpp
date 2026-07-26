#include "Window.h"

#include <assert.h>

#include <iostream>
#include <vector>

namespace Core {
Window::Window(const WindowSpecification& specification)
    : m_Specification(specification) {}

Window::~Window() { Destroy(); }

void Window::Create() {
  uint32_t flags = SDL_WINDOW_RESIZABLE;

  m_Handle =
      SDL_CreateWindow(m_Specification.Title.c_str(), m_Specification.Width,
                       m_Specification.Height, flags);

  if (!m_Handle) {
    std::cerr << "Failed to create SDL window!" << SDL_GetError() << "\n";
    assert(false);
  }
}

void Window::Destroy() {
  if (m_Handle) SDL_DestroyWindow(m_Handle);

  m_Handle = nullptr;
}

void Window::Update() {}

bool Window::ShouldClose() const { return m_ShouldClose; }

}  // namespace Core