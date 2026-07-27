#include <SDL3/SDL.h>

#include <algorithm>
#include <cmath>

#include "Application.h"
#include "game/ArenaGame.h"

int main() {
  Core::ApplicationSpecification appSpec;
  appSpec.Name = "Sentinel";
  appSpec.WindowSpec.Width = 1280;
  appSpec.WindowSpec.Height = 720;

  Core::Application application(appSpec);
  Game::ArenaGame game;

  application.Run([&game](float deltaTime, Core::Renderer& renderer) {
    game.Frame(deltaTime, renderer);
  });
}