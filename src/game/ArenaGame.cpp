#include "ArenaGame.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cmath>

#include "Renderer.h"

namespace Game {
namespace {
constexpr SDL_Color kPlayerColor{109, 228, 195, SDL_ALPHA_OPAQUE};
constexpr SDL_Color kTargetColor{255, 159, 122, SDL_ALPHA_OPAQUE};
constexpr SDL_Color kDestinationColor{122, 182, 255, SDL_ALPHA_OPAQUE};

constexpr float kArenaWidth = 1280.0f;
constexpr float kArenaHeight = 720.0f;
constexpr float kPlayerSpeed = 400.0f;
}  // namespace

void ArenaGame::Frame(float deltaTime, Core::Renderer& renderer) {
  Update(deltaTime);
  Render(renderer);
}

void ArenaGame::Update(float deltaTime) {
  const bool* keys = SDL_GetKeyboardState(nullptr);
  UpdateControlMode(keys);

  if (m_ControlMode == ControlMode::Wasd) {
    UpdateWasd(keys, deltaTime);
  } else {
    UpdateClickToMove(deltaTime);
  }
}

void ArenaGame::UpdateControlMode(const bool* keys) {
  if (keys[SDL_SCANCODE_1]) {
    m_ControlMode = ControlMode::Wasd;
    m_HasMoveDestination = false;
  } else if (keys[SDL_SCANCODE_2]) {
    m_ControlMode = ControlMode::ClickToMove;
  }
}

void ArenaGame::UpdateWasd(const bool* keys, float deltaTime) {
  SDL_FPoint direction{};
  if (keys[SDL_SCANCODE_A]) direction.x -= 1.0f;
  if (keys[SDL_SCANCODE_D]) direction.x += 1.0f;
  if (keys[SDL_SCANCODE_W]) direction.y -= 1.0f;
  if (keys[SDL_SCANCODE_S]) direction.y += 1.0f;

  const float length = std::hypot(direction.x, direction.y);
  if (length > 0.0f) {
    direction.x /= length;
    direction.y /= length;
  }

  MovePlayer(direction, kPlayerSpeed * deltaTime);
}

void ArenaGame::UpdateClickToMove(float deltaTime) {
  float mouseX = 0.0f;
  float mouseY = 0.0f;
  const SDL_MouseButtonFlags buttons = SDL_GetMouseState(&mouseX, &mouseY);

  if ((buttons & SDL_BUTTON_LMASK) != 0) {
    m_MoveDestination.x =
        std::clamp(mouseX, m_Player.w * 0.5f, kArenaWidth - m_Player.w * 0.5f);
    m_MoveDestination.y =
        std::clamp(mouseY, m_Player.h * 0.5f, kArenaHeight - m_Player.h * 0.5f);
    m_HasMoveDestination = true;
  }

  if (!m_HasMoveDestination) {
    return;
  }

  const SDL_FPoint playerCenter{
      m_Player.x + m_Player.w * 0.5f,
      m_Player.y + m_Player.h * 0.5f,
  };

  SDL_FPoint direction{
      m_MoveDestination.x - playerCenter.x,
      m_MoveDestination.y - playerCenter.y,
  };

  const float remainingDistance = std::hypot(direction.x, direction.y);

  if (remainingDistance == 0.0f) {
    m_HasMoveDestination = false;
    return;
  }

  direction.x /= remainingDistance;
  direction.y /= remainingDistance;

  const float requestedDistance = kPlayerSpeed * deltaTime;
  const float travelDistance = std::min(requestedDistance, remainingDistance);

  MovePlayer(direction, travelDistance);

  if (travelDistance == remainingDistance) {
    m_HasMoveDestination = false;
  }
}

void ArenaGame::MovePlayer(SDL_FPoint direction, float distance) {
  m_Player.x += direction.x * distance;
  m_Player.y += direction.y * distance;
  m_Player.x = std::clamp(m_Player.x, 0.0f, kArenaWidth - m_Player.w);
  m_Player.y = std::clamp(m_Player.y, 0.0f, kArenaHeight - m_Player.h);
}

void ArenaGame::Render(Core::Renderer& renderer) {
  if (m_ControlMode == ControlMode::ClickToMove && m_HasMoveDestination) {
    constexpr float markerSize = 12.0f;
    const SDL_FRect marker{
        m_MoveDestination.x - markerSize * 0.5f,
        m_MoveDestination.y - markerSize * 0.5f,
        markerSize,
        markerSize,
    };
    renderer.DrawFilledRect(marker, kDestinationColor);
  }

  renderer.DrawFilledRect(m_Player, kPlayerColor);
  renderer.DrawFilledRect(m_Target, kTargetColor);
}
}  // namespace Game