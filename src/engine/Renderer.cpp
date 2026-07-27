#include "Renderer.h"

#include <stdexcept>
#include <string>

namespace Core {

Renderer::Renderer(SDL_Window* window)
    : m_Handle(SDL_CreateRenderer(window, nullptr)) {
  if (!m_Handle) {
    throw std::runtime_error(std::string{"SDL_CreateRenderer failed: "} +
                             SDL_GetError());
  }
}

Renderer::~Renderer() {
  if (m_Handle) {
    SDL_DestroyRenderer(m_Handle);
  }
}

void Renderer::BeginFrame() {
  if (!SDL_SetRenderDrawColor(m_Handle, 12, 20, 40, SDL_ALPHA_OPAQUE) ||
      !SDL_RenderClear(m_Handle)) {
    throw std::runtime_error(std::string{"SDL frame setup failed: "} +
                             SDL_GetError());
  }
}

void Renderer::DrawFilledRect(const SDL_FRect& rect, SDL_Color color) {
  if (!SDL_SetRenderDrawColor(m_Handle, color.r, color.g, color.b, color.a) ||
      !SDL_RenderFillRect(m_Handle, &rect)) {
    throw std::runtime_error(std::string{"SDL rectangle draw failed: "} +
                             SDL_GetError());
  }
}

void Renderer::EndFrame() {
  if (!SDL_RenderPresent(m_Handle)) {
    throw std::runtime_error(std::string{"SDL frame present failed: "} +
                             SDL_GetError());
  }
}
}  // namespace Core