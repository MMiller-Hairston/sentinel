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

constexpr SDL_Color kHealthBackColor{52, 65, 78, SDL_ALPHA_OPAQUE};
constexpr SDL_Color kHealthFillColor{129, 230, 169, SDL_ALPHA_OPAQUE};
constexpr SDL_Color kDefeatedColor{78, 86, 94, SDL_ALPHA_OPAQUE};

constexpr float kTargetMaxHealth = 100.0f;
constexpr float kAttackDamage = 25.0f;
constexpr float kAttackRange = 180.0f;

constexpr float kAttackCooldownDuration = 0.75f;

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

  UpdateAttack(keys, deltaTime);
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

  constexpr float cooldownHeight = 8.0f;
  constexpr float cooldownGap = 8.0f;
  const SDL_FRect cooldownBack{
      m_Player.x,
      m_Player.y + m_Player.h + cooldownGap,
      m_Player.w,
      cooldownHeight,
  };
  const float readyRatio =
      1.0f - m_AttackCooldownRemaining / kAttackCooldownDuration;
  SDL_FRect cooldownFill = cooldownBack;
  cooldownFill.w *= readyRatio;

  renderer.DrawFilledRect(cooldownBack, kHealthBackColor);
  renderer.DrawFilledRect(cooldownFill, kDestinationColor);

  const SDL_Color targetColor =
      m_TargetHealth > 0.0f ? kTargetColor : kDefeatedColor;
  renderer.DrawFilledRect(m_Target, targetColor);

  constexpr float barHeight = 10.0f;
  constexpr float barGap = 8.0f;
  const SDL_FRect healthBack{
      m_Target.x,
      m_Target.y - barGap - barHeight,
      m_Target.w,
      barHeight,
  };
  const float healthRatio = m_TargetHealth / kTargetMaxHealth;
  SDL_FRect healthFill = healthBack;
  healthFill.w *= healthRatio;

  renderer.DrawFilledRect(m_Player, kPlayerColor);
  renderer.DrawFilledRect(healthBack, kHealthBackColor);
  renderer.DrawFilledRect(healthFill, kHealthFillColor);
}

void ArenaGame::UpdateAttack(const bool* keys, float deltaTime) {
  m_AttackCooldownRemaining =
      std::max(0.0f, m_AttackCooldownRemaining - deltaTime);

  const bool attackHeld = keys[SDL_SCANCODE_SPACE];
  const bool attackPressed = attackHeld && !m_WasAttackHeld;
  m_WasAttackHeld = attackHeld;

  if (!attackPressed || m_TargetHealth <= 0.0f ||
      m_AttackCooldownRemaining > 0.0f) {
    return;
  }

  const SDL_FPoint playerCenter{
      m_Player.x + m_Player.w * 0.5f,
      m_Player.y + m_Player.h * 0.5f,
  };
  const SDL_FPoint targetCenter{
      m_Target.x + m_Target.w * 0.5f,
      m_Target.y + m_Target.h * 0.5f,
  };
  const float distance = std::hypot(targetCenter.x - playerCenter.x,
                                    targetCenter.y - playerCenter.y);

  if (distance <= kAttackRange) {
    m_TargetHealth =
        std::clamp(m_TargetHealth - kAttackDamage, 0.0f, kTargetMaxHealth);
    m_AttackCooldownRemaining = kAttackCooldownDuration;
  }
}
}  // namespace Game