#include <SDL3/SDL.h>

#include <algorithm>
#include <cmath>

#include "Application.h"

int main() {
  Core::ApplicationSpecification appSpec;
  appSpec.Name = "Sentinel";
  appSpec.WindowSpec.Width = 1280;
  appSpec.WindowSpec.Height = 720;

  Core::Application application(appSpec);
  SDL_FRect player{860.0f, 540.0f, 80.0f, 80.0f};
  application.Run([&player](float deltaTime, Core::Renderer& renderer) {
    const bool* keys = SDL_GetKeyboardState(nullptr);

    float vertical = 0.0f;
    float horizontal = 0.0f;
    if (keys[SDL_SCANCODE_A]) {
      horizontal -= 1.0f;
    }
    if (keys[SDL_SCANCODE_D]) {
      horizontal += 1.0f;
    }
    if (keys[SDL_SCANCODE_W]) {
      vertical -= 1.0f;
    }
    if (keys[SDL_SCANCODE_S]) {
      vertical += 1.0f;
    }

    constexpr float speed = 400.0f;
    const float directionLength = std::hypot(horizontal, vertical);
    if (directionLength > 0.0f) {
      horizontal /= directionLength;
      vertical /= directionLength;
    }
    player.x += horizontal * speed * deltaTime;
    player.y += vertical * speed * deltaTime;
    player.x = std::clamp(player.x, 0.0f, 1200.0f);
    player.y = std::clamp(player.y, 0.0f, 640.0f);

    renderer.DrawFilledRect(player);
  });
}