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
  void UpdateTargetAttack(float deltaTime);

  float m_PlayerHealth = 100.0f;
  float m_TargetAttackCooldownRemaining = 0.0f;

  static constexpr SDL_FRect kPlayerStart{600.0f, 320.0f, 80.0f, 80.0f};
  static constexpr SDL_FRect kTargetStart{920.0f, 320.0f, 80.0f, 80.0f};

  SDL_FRect m_Player = kPlayerStart;
  SDL_FRect m_Target = kTargetStart;

  // Encounter
  bool IsEncounterOver() const;
  void UpdateRestart(float deltaTime);
  void ResetEncounter();

  float m_RestartDelayRemaining = 0.0f;

  float m_TargetAttackWindupRemaining = 0.0f;
};
}  // namespace Game