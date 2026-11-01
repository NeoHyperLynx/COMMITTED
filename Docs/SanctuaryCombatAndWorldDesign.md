# Sanctuary Combat & World Design Specification

This document details the combat architecture, mechanical tables, AI director states, and world generation guidelines for the **Sanctuary** map — combining Anthem's vertical Bastion landscape with Ghost of Tsushima's aesthetic, Sifu/Sekiro posture and deflect rhythm, Sleeping Dogs environmental takedowns, and Urban Reign fighting game depth.

---

## 1. Posture & Flow System Specifications

### 1.1 Definitions and System Separation
**Posture** and **Flow** serve distinct, complementary roles in combat:

- **Posture (Defensive Integrity & Structural Balance)**:
  - **What it does**: Posture represents structural stability and guard poise. Guarding attacks or mistiming deflects accumulates posture damage toward 100. Posture breaking a target bypasses normal health attrition and creates immediate lethal vulnerability (environmental takedowns, critical finish strikes).
  - **Presence in Duels vs. 1vN**: **Both player and enemies possess Posture in all combat modes (1v1 Duels and 1vN Scrimmages)**. In duels, Posture is the primary victory condition (Sekiro-style rhythm exchanges), where breaking the rival's posture ends the duel.

- **Flow (Offensive Kinetic Momentum & Martial Rhythm)**:
  - **What it does**: Flow represents the fighter's martial momentum, focus, and tempo. Flow is gained purely through proactive execution: landing clean strikes (+12), timing perfect deflects (+18), and executing fluid parkour/wall runs (+8). Flow spends or unlocks high-tier martial perks: instant dash-canceling out of recovery animations (Tier 1: 30+), bonus guard penetration (Tier 2: 70+), and Phantom State surge (Tier 3: 100).
  - **Presence in Duels vs. 1vN**: **Both player and enemies possess Flow in all combat modes**. In 1vN, Flow allows the player to continuously reposition and burst through crowds without stamina constraints. In 1v1 duels, maintaining high Flow prevents the boss from resetting neutral and allows the player to cancel committed whiff recoveries into immediate defensive deflects.

### 1.2 Player Posture Break Consequence
When the player's Posture reaches 100, the player's guard suffers a catastrophic collapse. The player enters a **1.40-second helpless stagger state** accompanied by a distinct audio-visual guard shatter. During this 1.40s window:
1. All incoming attacks deal **1.75× unmitigated damage** directly to Health.
2. Deflect (`E`), Flash Dash (`Alt`), and attacks are **hard-locked** (cannot be triggered or buffered until 0.20s before the stagger ends).
3. Movement speed is reduced by **70%**.
4. Secondary flanker enemies gain immediate clearance from the Crowd Director to execute a coordinated punish strike.
5. Upon recovering, the player's Posture resets to **60 points** (preventing an instant re-break loop while keeping the player in immediate danger until they regain neutral spacing).

### 1.3 Deflect Multiplier Formula
The posture damage inflicted upon an attacking opponent during a successful deflect string scales deterministically according to the following formula:

$$\text{PostureDamage} = \text{BasePostureDamage} \times \min\left(1.0 + (\text{ConsecutiveDeflectCount} - 1) \times 0.25, \; 2.50\right)$$

Where:
- $\text{BasePostureDamage} = 24.0$ points.
- $\text{ConsecutiveDeflectCount}$ starts at $1$ and increments with each consecutive deflect executed within $0.80\text{s}$ of the previous deflect without receiving damage, missing a deflect input, or disengaging.
- The multiplier steps are:
  - Hit 1: $1.00\times \implies 24.0$ posture damage.
  - Hit 2: $1.25\times \implies 30.0$ posture damage.
  - Hit 3: $1.50\times \implies 36.0$ posture damage.
  - Hit 4: $1.75\times \implies 42.0$ posture damage.
  - Hit 5: $2.00\times \implies 48.0$ posture damage.
  - Hit 7+: capped at $2.50\times \implies 60.0$ posture damage.

---

## 2. Environmental Prompts & Grab-to-Environmental Chain Table

When an enemy is in a vulnerable state (either **Posture Broken** or **Grabbed**), environmental geometry within range can be leveraged for cinematic, high-impact takedowns.

### 2.1 Environmental Prompt Disambiguation & Grab Chain Resolution Table

| Situation / State | Spatial Condition | Prompt Displayed & Input | Disambiguation / Priority Rule | Guaranteed Execution Outcome | Interruption & i-Frame Rules |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Enemy Posture Broken (100 Posture)** | Multiple surfaces within 2.0m radius | **`[F] Environmental Takedown`** (Contextual) | 1. **Ledge / Chasm**: If character faces within $\pm 60^\circ$ of a drop-off $>3\text{m}$.<br>2. **Pillar / Torii Post**: If a cylinder prop is within $1.8\text{m}$.<br>3. **Wall**: Nearest flat surface collision normal. | • **Ledge**: Instant lethal drop ejection.<br>• **Pillar**: 75% Max HP damage + Wall Splat Stumble.<br>• **Wall**: 70% Max HP damage + Grounded Knockdown. | **Full Invulnerability (0.85s i-frames)**. All crowd attack tokens pause and surrounding enemies freeze swings until animation ends. |
| **Neutral / Active Enemy (Posture < 100)** | Target within 1.8m, not posture broken | **`[G] Dedicated Grab`** (Head / Body lock) | If holding Forward (`W`): **Head Grab**.<br>If Neutral or Sideways (`A`/`D`/`S`): **Body / Limb Lock**. | Target immobilized in grab hold for up to **1.80s**. Player has 40% damage resistance during hold. | Soft interruption: Player taking a heavy attack breaks grab. Minion light strikes do not break grab. |
| **Grab Held → Directed toward Hazard** | In Grab hold + Directional Input held | **`[G] + Direction`** (Throw into Hazard) | Checks line-of-sight in throw direction within 2.5m: Ledge > Pillar > Wall > Open Floor Throw. | Target launched into surface: Wall Splat inflicts 35% HP damage and 2.0s grounded prone state. | Thrown enemy becomes an active collision projectile, staggering any secondary flankers hit in flight. |
| **Grab Held → Within 2.0m of Hazard** | Target Grabbed AND within 2.0m of surface | **`[F] Environmental Slam`** (Grab Chain) | Overrides normal grab hold. Prioritizes surface directly behind or to the flank of the grabbed victim. | Chains directly from grab into environmental smash (head slam on pillar or torso slam on wall) for 50% HP damage. | Seamless transition into execution i-frames (0.85s). |
| **No Surface in 2.0m Range** | Distance to any surface $> 2.0\text{m}$ | **`[G] Throw`** OR Standard Strikes | No environmental prompt displayed. Grab can only execute open-ground knee strike or forward throw. | Launches enemy 9m forward across open floor into neutral recovery. | Standard vulnerability. |

---

## 3. Crowd Token AI Director Rules

The AI director manages 1vN encounters to avoid chaotic dogpiling while maintaining tension and aggressive pacing.

### 3.1 Token System State Machine
- **Maximum Concurrent Attackers**:
  - Normal Combat: **2 Attack Tokens** active simultaneously (1 Melee Aggressor, 1 Flanker/Prober).
  - Boss Encounters: **1 Boss Token** (Boss has full token priority; minions act purely as stage distractors).
- **Token Acquisition**:
  - Eligible enemies within 8m request a token every 0.4s.
  - Distance, line-of-sight, and readiness score determine token assignment.
- **Token Release**:
  - Released immediately when an attack finishes recovery, target is deflected, or attacker is staggered/knocked down.
  - If player performs an environmental takedown or deflects an attack, non-attacking enemies enter a **0.65s hesitate state**.
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
