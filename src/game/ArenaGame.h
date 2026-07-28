#pragma once

#include <SDL3/SDL.h>

namespace Core {
class Renderer;
}

namespace Game {

enum class ControlMode {
  Wasd,
  ClickToMove,
};

class ArenaGame {
 public:
  void Frame(float deltaTime, Core::Renderer& renderer);

 private:
  // Movement
  void Update(float deltaTime);
  void UpdateControlMode(const bool* keys);
  void UpdateWasd(const bool* keys, float deltaTime);
  void UpdateClickToMove(float deltaTime);
  void MoveActor(SDL_FRect& actor, SDL_FPoint direction, float distance);
  void Render(Core::Renderer& renderer);

  ControlMode m_ControlMode = ControlMode::Wasd;
  SDL_FRect m_Player{600.0f, 320.0f, 80.0f, 80.0f};
  SDL_FRect m_Target{920.0f, 320.0f, 80.0f, 80.0f};
  SDL_FPoint m_MoveDestination{};
  bool m_HasMoveDestination = false;

  // Fighting
  void UpdateAttack(const bool* keys, float deltaTime);

  float m_TargetHealth = 100.0f;
  bool m_WasAttackHeld = false;
  float m_AttackCooldownRemaining = 0.0f;

  struct Projectile {
    SDL_FRect bounds{};
    SDL_FPoint velocity{};
    bool active = false;
  };

  void UpdateSkillShot(const bool* keys, float deltaTime);

  Projectile m_SkillShot;
  bool m_WasSkillShotHeld = false;
  float m_SkillShotCooldownRemaining = 0.0f;

  // Dash
  void UpdateDash(const bool* keys, float deltaTime);
  bool m_WasDashHeld = false;
  float m_DashCooldownRemaining = 0.0f;

  // Enemy
  void UpdateTarget(float deltaTime);
};
}  // namespace Game