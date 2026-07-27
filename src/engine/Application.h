#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "Renderer.h"
#include "Window.h"

namespace Core {

struct ApplicationSpecification {
  std::string Name = "Application";
  WindowSpecification WindowSpec;
};

class Application {
 public:
  explicit Application(const ApplicationSpecification& specification =
                           ApplicationSpecification());
  ~Application();

  using FrameCallback = std::function<void(float, Renderer&)>;

  void Run(const FrameCallback& onFrame);
  void Stop();

  static Application& Get();
  static float GetTime();

 private:
  ApplicationSpecification m_Specification;

  std::shared_ptr<Window> m_Window;
  std::shared_ptr<Renderer> m_Renderer;

  bool m_Running = false;
};

}  // namespace Core
