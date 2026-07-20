#pragma once

#include <SDL3/SDL.h>

#include <functional>
#include <string>

namespace Core {
struct WindowSpecification {
  std::string Title;
  uint32_t Width = 1280;
  uint32_t Height = 720;
  bool IsResizable = true;
  bool VSync = true;
};

class Window {
 public:
  explicit Window(
      const WindowSpecification& specification = WindowSpecification());
  ~Window();

  void Create();
  void Destroy();

  void Update();

  bool ShouldClose() const;
  void SetShouldClose(bool close) { m_ShouldClose = close; }

  SDL_Window* GetHandle() const { return m_Handle; }

 private:
  WindowSpecification m_Specification;
  bool m_ShouldClose = false;
  SDL_Window* m_Handle = nullptr;
};
}  // namespace Core