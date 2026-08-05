# Enemy Attack Telegraph Design

## Objective

Replace the enemy's instant melee damage with a readable, dodgeable wind-up.
The player should see an attack coming, understand when it will resolve, and be
able to cancel it by moving or dashing out of range before damage lands.

This is Lesson 0016's single visible win and advances Sentinel toward readable,
League-of-Legends-like arena combat while retaining the current rectangle-based
prototype.

## Player-Facing Contract

- When the living enemy is ready and reaches melee range, it begins a
  0.45-second wind-up.
- During the wind-up, the enemy stops pursuing, changes to a yellow warning
  color, and displays a thin progress bar that fills toward impact.
- If the player leaves melee range before the wind-up completes, the attack is
  cancelled. It deals no damage and starts no cooldown.
- If the player remains in range until the wind-up reaches zero, the attack
  applies its existing bounded damage and enters its existing recovery cooldown.
- A finished encounter cancels all combat updates, and encounter reset clears
  any pending wind-up.

## Action State Model

The existing remaining-duration values encode three mutually exclusive phases;
an enum would duplicate this state.

| Phase | Wind-up remaining | Cooldown remaining | Behavior |
| --- | ---: | ---: | --- |
| Ready | `0` | `0` | May begin an attack in range |
| Wind-up | `> 0` | `0` | Telegraph, stop pursuit, validate escape |
| Recovery | `0` | `> 0` | Cannot begin another attack |

Add one authoritative member,
`m_TargetAttackWindupRemaining`, initialized to zero, and one constant,
`kTargetAttackWindupDuration`, set to `0.45f` seconds.

## Update Flow

`ArenaGame::Update` keeps its current high-level order: player actions, target
pursuit, then target attack resolution.

`UpdateTarget` returns without moving while the wind-up timer is positive. This
gives movement and dash a stable escape window. If an attack is cancelled this
Frame, pursuit resumes on the following Frame.

`UpdateTargetAttack` performs these steps:

1. Count the recovery cooldown toward zero using `deltaTime`.
2. If either actor is defeated, clear any wind-up and return.
3. Calculate center-to-center distance once for the current Frame.
4. If already winding up:
   - cancel and return when distance exceeds melee range;
   - otherwise count the wind-up toward zero;
   - return if time remains;
   - if it reached zero, apply bounded damage and commit recovery cooldown.
5. If not winding up, reject while recovery cooldown remains or range fails.
6. Otherwise start the full wind-up duration without applying damage.

Only completed damage commits recovery cooldown. A cancelled warning leaves the
enemy ready to pursue and try again once it returns to range.

## Rendering

The target's existing defeated color retains priority. A living target uses the
new yellow warning color while its wind-up timer is positive and its normal
color otherwise.

During wind-up, reuse `DrawCooldownBar` beneath the target. Its existing
`1 - remaining / duration` calculation produces the desired empty-to-full
impact progress without adding another drawing helper.

## Encounter Reset

`ResetEncounter` assigns `0.0f` to `m_TargetAttackWindupRemaining` alongside the
other combat timers. The selected control mode remains preserved exactly as in
Lesson 0015.

## Invariants and Boundaries

- Wind-up and cooldown durations never become negative.
- Damage occurs only at successful wind-up completion.
- Cancelled wind-ups cause neither damage nor recovery cooldown.
- Defeated actors cannot begin or complete attacks.
- Target pursuit and target wind-up are not active during the same Frame.
- Restarted encounters contain no pending enemy attack.

## Verification

Build with the existing debug preset and test these boundaries manually:

1. Stay in range: the target turns yellow, the bar fills for 0.45 seconds,
   damage lands once, and recovery begins.
2. Walk out before impact: the warning clears with no damage or cooldown.
3. Dash out before impact: the warning clears with no damage or cooldown.
4. Re-enter after cancellation: pursuit resumes and a new full wind-up begins.
5. Defeat the target during wind-up: no delayed damage lands.
6. Finish and restart an encounter during any attack phase: the next encounter
   begins ready, with no stale warning.

## Out of Scope

- Attack animations, facing direction, sprites, sound, particles, or screen
  shake.
- Multiple enemy attack types or an explicit reusable ability state machine.
- A spatial range circle or collision-system changes.
