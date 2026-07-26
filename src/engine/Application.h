#pragma once

#include <memory>
#include <string>
#include <vector>

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

  void Run();
  void Stop();

  static Application& Get();
  static float GetTime();

 private:
  ApplicationSpecification m_Specification;
  std::shared_ptr<Window> m_Window;
  bool m_Running = false;
};

}  // namespace Core
