#pragma once

#include <SDL3/SDL.h>

namespace Core {
class Renderer {
 public:
  explicit Renderer(SDL_Window* window);
  ~Renderer();

  void BeginFrame();
  void DrawFilledRect(const SDL_FRect& rect, SDL_Color color);
  void EndFrame();

 private:
  SDL_Renderer* m_Handle = nullptr;
};
}  // namespace Core