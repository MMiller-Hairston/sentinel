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

// Attack
constexpr float kTargetMaxHealth = 100.0f;
constexpr float kAttackDamage = 25.0f;
constexpr float kAttackRange = 180.0f;

constexpr float kAttackCooldownDuration = 0.75f;

// Abilities
constexpr SDL_Color kSkillShotColor{255, 201, 120, SDL_ALPHA_OPAQUE};
constexpr float kSkillShotSize = 24.0f;
constexpr float kSkillShotSpeed = 650.0f;
constexpr float kSkillShotDamage = 40.0f;
constexpr float kSkillShotCooldownDuration = 1.5f;

void DrawCooldownBar(Core::Renderer& renderer, const SDL_FRect& back,
                     float remaining, float duration, SDL_Color fillColor) {
  const float readyRatio = 1.0f - remaining / duration;
  SDL_FRect fill = back;
  fill.w *= readyRatio;

  renderer.DrawFilledRect(back, kHealthBackColor);
  renderer.DrawFilledRect(fill, fillColor);
}

// Dash
constexpr SDL_Color kDashColor{205, 160, 255, SDL_ALPHA_OPAQUE};
constexpr float kDashDistance = 220.0f;
constexpr float kDashCooldownDuration = 3.0f;

constexpr float kTargetSpeed = 140.0f;
constexpr float kTargetStopDistance = 110.0f;

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
  UpdateSkillShot(keys, deltaTime);
  UpdateDash(keys, deltaTime);
  UpdateTarget(deltaTime);
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

  MoveActor(m_Player, direction, kPlayerSpeed * deltaTime);
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

  MoveActor(m_Player, direction, travelDistance);

  if (travelDistance == remainingDistance) {
    m_HasMoveDestination = false;
  }
}

void ArenaGame::MoveActor(SDL_FRect& actor, SDL_FPoint direction,
                          float distance) {
  actor.x += direction.x * distance;
  actor.y += direction.y * distance;
  actor.x = std::clamp(actor.x, 0.0f, kArenaWidth - actor.w);
  actor.y = std::clamp(actor.y, 0.0f, kArenaHeight - actor.h);
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
  constexpr float cooldownTopGap = 8.0f;
  constexpr float cooldownBetweenGap = 8.0f;

  const SDL_FRect attackCooldownBack{
      m_Player.x,
      m_Player.y + m_Player.h + cooldownTopGap,
      m_Player.w,
      cooldownHeight,
  };
  SDL_FRect skillShotCooldownBack = attackCooldownBack;
  skillShotCooldownBack.y += cooldownHeight + cooldownBetweenGap;

  SDL_FRect dashCooldownBack = skillShotCooldownBack;
  dashCooldownBack.y += cooldownHeight + cooldownBetweenGap;

  DrawCooldownBar(renderer, attackCooldownBack, m_AttackCooldownRemaining,
                  kAttackCooldownDuration, kDestinationColor);
  DrawCooldownBar(renderer, skillShotCooldownBack, m_SkillShotCooldownRemaining,
                  kSkillShotCooldownDuration, kSkillShotColor);
  DrawCooldownBar(renderer, dashCooldownBack, m_DashCooldownRemaining,
                  kDashCooldownDuration, kDashColor);

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

  if (m_SkillShot.active) {
    renderer.DrawFilledRect(m_SkillShot.bounds, kSkillShotColor);
  }
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

void ArenaGame::UpdateSkillShot(const bool* keys, float deltaTime) {
  m_SkillShotCooldownRemaining =
      std::max(0.0f, m_SkillShotCooldownRemaining - deltaTime);

  const bool skillHeld = keys[SDL_SCANCODE_Q];
  const bool skillPressed = skillHeld && !m_WasSkillShotHeld;
  m_WasSkillShotHeld = skillHeld;

  if (skillPressed && m_SkillShotCooldownRemaining == 0.0f &&
      !m_SkillShot.active && m_TargetHealth > 0.0f) {
    float mouseX = 0.0f;
    float mouseY = 0.0f;
    SDL_GetMouseState(&mouseX, &mouseY);

    const SDL_FPoint playerCenter{
        m_Player.x + m_Player.w * 0.5f,
        m_Player.y + m_Player.h * 0.5f,
    };
    SDL_FPoint direction{
        mouseX - playerCenter.x,
        mouseY - playerCenter.y,
    };
    const float length = std::hypot(direction.x, direction.y);

    if (length > 0.0f) {
      direction.x /= length;
      direction.y /= length;
      m_SkillShot.bounds = {
          playerCenter.x - kSkillShotSize * 0.5f,
          playerCenter.y - kSkillShotSize * 0.5f,
          kSkillShotSize,
          kSkillShotSize,
      };
      m_SkillShot.velocity = {
          direction.x * kSkillShotSpeed,
          direction.y * kSkillShotSpeed,
      };
      m_SkillShot.active = true;
      m_SkillShotCooldownRemaining = kSkillShotCooldownDuration;
    }
  }

  if (!m_SkillShot.active) {
    return;
  }

  m_SkillShot.bounds.x += m_SkillShot.velocity.x * deltaTime;
  m_SkillShot.bounds.y += m_SkillShot.velocity.y * deltaTime;

  if (SDL_HasRectIntersectionFloat(&m_SkillShot.bounds, &m_Target)) {
    m_TargetHealth =
        std::clamp(m_TargetHealth - kSkillShotDamage, 0.0f, kTargetMaxHealth);
    m_SkillShot.active = false;
    return;
  }

  const bool outsideArena =
      m_SkillShot.bounds.x + m_SkillShot.bounds.w < 0.0f ||
      m_SkillShot.bounds.x > kArenaWidth ||
      m_SkillShot.bounds.y + m_SkillShot.bounds.h < 0.0f ||
      m_SkillShot.bounds.y > kArenaHeight;

  if (outsideArena) {
    m_SkillShot.active = false;
  }
}

void ArenaGame::UpdateDash(const bool* keys, float deltaTime) {
  m_DashCooldownRemaining = std::max(0.0f, m_DashCooldownRemaining - deltaTime);

  const bool dashHeld = keys[SDL_SCANCODE_E];
  const bool dashPressed = dashHeld && !m_WasDashHeld;
  m_WasDashHeld = dashHeld;

  if (!dashPressed || m_DashCooldownRemaining > 0.0f) {
    return;
  }

  float mouseX = 0.0f;
  float mouseY = 0.0f;
  SDL_GetMouseState(&mouseX, &mouseY);

  const SDL_FPoint playerCenter{
      m_Player.x + m_Player.w * 0.5f,
      m_Player.y + m_Player.h * 0.5f,
  };
  SDL_FPoint direction{
      mouseX - playerCenter.x,
      mouseY - playerCenter.y,
  };
  const float cursorDistance = std::hypot(direction.x, direction.y);

  if (cursorDistance == 0.0f) {
    return;
  }

  direction.x /= cursorDistance;
  direction.y /= cursorDistance;

  const float travelDistance = std::min(kDashDistance, cursorDistance);
  const SDL_FPoint previousPosition{
      m_Player.x,
      m_Player.y,
  };

  MoveActor(m_Player, direction, travelDistance);

  const bool moved =
      m_Player.x != previousPosition.x || m_Player.y != previousPosition.y;
  if (!moved) {
    return;
  }

  m_HasMoveDestination = false;
  m_DashCooldownRemaining = kDashCooldownDuration;
}

void ArenaGame::UpdateTarget(float deltaTime) {
  if (m_TargetHealth <= 0.0f) {
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
  SDL_FPoint direction{
      playerCenter.x - targetCenter.x,
      playerCenter.y - targetCenter.y,
  };
  const float distance = std::hypot(direction.x, direction.y);
  const float remainingTravel = std::max(0.0f, distance - kTargetStopDistance);

  if (remainingTravel == 0.0f) {
    return;
  }

  direction.x /= distance;
  direction.y /= distance;
  const float requestedTravel = kTargetSpeed * deltaTime;
  const float travelDistance = std::min(requestedTravel, remainingTravel);

  MoveActor(m_Target, direction, travelDistance);
}
}  // namespace Game