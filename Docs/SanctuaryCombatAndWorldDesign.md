# Sanctuary Combat & World Design Specification

This document details the combat architecture, mechanical tables, AI director states, and world generation guidelines for the **Sanctuary** map — combining Anthem's vertical Bastion landscape with Ghost of Tsushima's aesthetic, Sifu/Sekiro posture and deflect rhythm, Sleeping Dogs environmental takedowns, and Urban Reign fighting game depth.

---

## 1. Posture & Flow System Specifications

### 1.1 Dual-Meter Mechanics
The combat system separates structural integrity (**Posture**) from offensive rhythm and momentum (**Flow**).

- **Posture (Range: 0 – 100)**:
  - Measures defensive guard integrity and structural balance.
  - At **0 Posture**, fighter is stable.
  - As attacks are guarded or received, Posture rises toward **100**.
  - At **100 Posture**, the fighter experiences **Posture Break**:
    - **Enemy Break**: Enters a 2.0s vulnerable Stun state with white-hot outline, susceptible to lethal critical strikes or 2m Environmental Takedowns.
    - **Player Break**: Complete guard collapse, 1.4s staggered recovery where incoming attacks deal 1.75× damage and cannot be parried or dodged.
  - **Posture Regeneration**:
    - Begins after a 1.2s delay without taking hits or guarding.
    - Base regen rate: **18 points/sec**.
    - If health is below 50%, regen drops by 50% (**9 points/sec**).
    - Holding Guard increases Posture regen rate by 2.2× when unthreatened.

- **Flow (Range: 0 – 100)**:
  - Offensive kinetic meter built through clean hits (+12 Flow), timed deflects (+18 Flow), and fluid parkour transitions (+8 Flow).
  - Decays by 6 Flow/sec after 3.0s of disengagement.
  - **Flow Milestones**:
    - **Flow Tier 1 (30+)**: Unlocks Flash Dash canceling out of any recovery frame without spending stamina.
    - **Flow Tier 2 (70+)**: Attacks gain +25% posture damage against guarding opponents; deflect timing window gains +0.03s leniency.
    - **Flow Tier 3 (100 - Surge)**: Activates *Phantom State* for 6s: attacks cannot be interrupted by light enemy strikes; environmental takedowns instantly grant full dash charges and restore 20% health.

### 1.2 Deflect Windows & Multipliers
- **Perfect Deflect Window**: **0.14 seconds** (starts upon pressing Reversal/Deflect `E`).
  - Enemy attack interrupted, attacker receives **24 Posture damage** + **18 Flow** to player.
  - Multi-hit string deflect combo: Consecutive deflects multiply attacker posture damage by 1.25× up to a 2.5× cap.
- **Imperfect Guard / Block Window**: **0.15s – 0.40s**.
  - Attack blocked (no health damage), but defender absorbs **15 Posture damage**.
- **Deflect Whiff Penalty**: Missed deflect attempt triggers a **0.32s recovery lockout** where attacks cannot be deflected.

---

## 2. Environmental Takedowns Decision Table

Environmental interactions blend **Urban Reign**'s dedicated grab with head/body target selection and **Sleeping Dogs / Path of Neo** cinematic environmental lethality.

### 2.1 Environmental Takedown Decision Matrix

| Input | Trigger Condition | Overlapping Priority Order | Window Duration | Follow-up Guarantee | Interruption Rules |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Contextual Interact (`F` / `Alt`)** | Target Posture Broken (100) **AND** within 2.0m of Wall, Pillar, or Ledge. | 1. **Ledge / Chasm** (Instant Ejection)<br>2. **Torii / Pillar** (Structural Splat)<br>3. **Wall** (Wall Slam & Stun) | **2.0s** vulnerability window before enemy recovers 30% posture | Guarantees instant defeat (Ledge) or 75% max HP damage + Wall Splat Stumble | **Invulnerable i-frames** during the execution animation (0.85s). Other enemies freeze their swing until completion. |
| **Dedicated Grab Tap (`G` / `Gamepad LB`)** | Target not posture broken, distance < 1.8m, defender not in active attack frame. | 1. **Head Grab** (Frontal approach / high input)<br>2. **Body / Limb Lock** (Flank/rear or neutral input) | Hold lasts up to **1.8s** before target breaks free | Opens target to knee strike, limb break, or throw into adjacent hazard | Enemy can tech-escape if player takes external hit; player has 40% damage reduction while holding. |
| **Dedicated Grab Hold + Direction (`G` + Direction)** | In Grab hold state within 2.5m of obstacle or ledge. | 1. Directional thrown target into obstacle (Wall Splat)<br>2. Directional toss off cliff | Continuous while held | Wall Splat leaves enemy in grounded prone state for 2.0s | Broken if hit by heavy unblockable strike from secondary enemy. |
| **No Prompt / Default** | Distance > 2.0m OR Posture not broken OR Grab not initiated | N/A | None | Standard melee strikes | Standard combat collision rules apply. |

---

## 3. Crowd Token AI Director Rules

The AI director manages 1vN encounters to avoid chaotic dogpiling while maintaining tension and aggressive pacing.

### 3.1 Token System State Machine
- **Maximum Concurrent Attackers**:
  - Normal Combat: **2 Attack Tokens** active simultaneously (1 Melee Aggressor, 1 Flanker/Prober).
  - Boss Encountes: **1 Boss Token** (Boss has full token priority; minions act purely as stage distractors).
- **Token Acquisition**:
  - Eligible enemies within 8m request a token every 0.4s.
  - Distance, line-of-sight, and readiness score determine token assignment.
- **Token Release**:
  - Released immediately when an attack finishes recovery, target is deflected, or attacker is staggered/knocked down.
  - If player performs an environmental takedown or deflects an attack, non-attacking enemies enter a **0.6s hesitate state**.
- **Parry-Interruption Policy**:
  - When the player executes a perfect deflect, all incoming secondary attacks within a 2.5m radius are soft-parried with an acoustic shockwave, giving the player space for the counter-followup.

### 3.2 Enemy Archetypes & Behaviors

1. **Grunt (Striker)**:
   - *Behavior*: Approaches in front arc, executes readable 2-hit quick strike strings, and retreats to 3m perimeter when token is lost.
2. **Bruiser (Grappler & Heavy)**:
   - *Behavior*: Uses slow armored overhead swings and forward body checks; immune to front light-attack flinch; requires limb strikes or posture break to stagger.
3. **Skirmisher (Agile Flanker)**:
   - *Behavior*: Circles rapidly around the player's blind spots (sides/rear), lunging with sudden sweep kicks to trip up the player during other engagements.
4. **Scout (Ranged Disruptor)**:
   - *Behavior*: Maintains 9m–14m distance on elevated perimeters or rocks, firing telegraphed kunai/arrows that can be deflected back or dodged with flash dash.
5. **The Sanctuary Boss Rival (Kensei / Shadow Vanguard)**:
   - *Behavior*: Duel master with multi-hit rhythmic attack sequences, stance shifts, feints, and environmental wall-kick counters; dominates the arena floor.

---

## 4. Boss Duel Mode Specification

- **Arena**: The Sacred Dueling Ring at the lower basin of Sanctuary, surrounded by Shinto torii pillars, fallen stone lanterns, and high cliff walls.
- **Phase 1: Deflect Rhythm & Pressure**:
  - Multi-hit attack chains (3 to 5 hit combinations) demanding rhythmic deflects (`E`, `E`, `E...`).
  - Boss deflects predictable player spam; player must use flash dashes, slide sweeps, or limb strikes to open defenses.
- **Phase 2: Cinematic Clash & Environmental Counter**:
  - Boss attempts aerial dive strikes and wall rebounds from the surrounding cliffs.
  - At 50% HP or upon posture break, triggers a cinematic clash where camera swings to dynamic low-angle framing.
  - Player can execute contextual environmental slams against the ancient Shaper pillars to defeat the boss.

---

## 5. Sanctuary Level Blockout (Anthem + Tsushima)

- **Blockout Generator Script (`Tools/create_sanctuary_map.py`)**:
  - Uses Unreal Python Editor Scripting to generate geometry primitives, spawn locations, and takedown prop tags. Final art assets will be hand-authored.
- **Landmarks**:
  1. *Vista Peak (Spawn)*: 120m high cliff overlook with a view of the canyon and distant Shinto shrine.
  2. *Shinto Canyon Chasm & Wooden Bridges*: Narrow traversal paths over vertical depths, ideal for ledge takedowns and air dashes.
  3. *Shaper Relic Monolith Walls*: Massive vertical surfaces for wall running, wall kicking, and descent agility.
  4. *Torii Approach*: Colonnade of red torii arches leading to the central courtyard.
  5. *Sacred Dueling Ring (Boss Arena)*: Sunken basin with weathered stone pillars, boundary walls, and environmental props.
