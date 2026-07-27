#include "Application.h"

#include <SDL3/SDL.h>
#include <assert.h>

#include <iostream>
#include <ranges>

namespace Core {

static Application* s_Application = nullptr;

Application::Application(const ApplicationSpecification& specification)
    : m_Specification(specification) {
  s_Application = this;

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
  }

  if (m_Specification.WindowSpec.Title.empty()) {
    m_Specification.WindowSpec.Title = m_Specification.Name;
  }

  m_Window = std::make_shared<Window>(m_Specification.WindowSpec);
  m_Window->Create();

  m_Renderer = std::make_shared<Renderer>(m_Window->GetHandle());
}

Application::~Application() {
  m_Renderer.reset();
  m_Window->Destroy();
  SDL_Quit();
  s_Application = nullptr;
}

void Application::Run(const FrameCallback& onFrame) {
  m_Running = true;
  float lastTime = GetTime();

  while (m_Running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT) {
        m_Window->SetShouldClose(true);
      }
    }

    if (m_Window->ShouldClose()) {
      Stop();
      break;
    }

    float currentTime = GetTime();
    float timestep = SDL_clamp(currentTime - lastTime, 0.001f, 0.1f);
    lastTime = currentTime;

    // This is our draw frame
    m_Renderer->BeginFrame();
    onFrame(timestep, *m_Renderer);
    m_Renderer->EndFrame();

    m_Window->Update();
  }
}

void Application::Stop() { m_Running = false; }

Application& Application::Get() {
  assert(s_Application);
  return *s_Application;
}

float Application::GetTime() {
  return static_cast<float>(SDL_GetTicks()) / 1000.f;
}

}  // namespace Core