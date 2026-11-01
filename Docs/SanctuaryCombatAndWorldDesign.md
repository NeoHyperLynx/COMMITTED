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
  - **What it does**: Flow represents the fighter's martial momentum, focus, and tempo. Flow is gained purely through proactive execution: landing clean strikes (+12), timing perfect deflects (+18), and executing fluid parkour/wall runs (+8).
  - **Out-of-Combat Parkour Flow Cap**: Parkour-generated Flow is **hard-capped at 50 Flow (Tier 1)** when outside of combat. Tiers 2 (70+) and 3 (100) are combat-exclusive and cannot be pre-farmed in traversal before engaging boss or scrimmage encounters.
  - **Flow Tier Perks & Tier 3 "Execution Readiness"**:
    - **Tier 1 (30+ Flow)**: Enables instant Flash Dash cancels out of committed strike whiff/recovery animations without suffering recovery lock.
    - **Tier 2 (70+ Flow)**: Grants +25% guard penetration and posture damage on strikes.
    - **Tier 3 (100 Flow / "Phantom Surge")**: Does **NOT bypass the Posture loop** for instant lethal executions on healthy targets. Instead, the next heavy attack (`Q` Committed Strike) executed at 100 Flow inflicts a guaranteed structural guard pierce that immediately chunks **50% of the target's current remaining Posture bar**, triggering an instant Posture Break if the target was at $\ge 50$ Posture. Additionally grants +15% attack speed and zero-cost dash cancels for 6.0s.
  - **Presence in Duels vs. 1vN**: **Both player and enemies possess Flow in all combat modes**. In 1vN, Flow allows the player to continuously reposition and burst through crowds without stamina constraints. In 1v1 duels, maintaining high Flow prevents the boss from resetting neutral and allows the player to cancel committed whiff recoveries into immediate defensive deflects.

### 1.2 Player Posture Break Consequence & Anti-Loop Grace Period
When the player's Posture reaches 100, the player's guard suffers a catastrophic collapse. The player enters a **1.40-second helpless stagger state** accompanied by a distinct audio-visual guard shatter:
1. All incoming attacks deal **1.75× unmitigated damage** directly to Health.
2. Deflect (`E`), Flash Dash (`Alt`), and attacks are **hard-locked** (cannot be triggered or buffered until 0.20s before the stagger ends).
3. Movement speed is reduced by **70%**.
4. Secondary flanker enemies gain immediate clearance from the Crowd Director to execute a coordinated punish strike.
5. **Anti-Loop Grace Period**: Upon recovering, the player's Posture resets to **60 points**, and the player receives a **1.20-second Posture Break Immunity Grace Period**. During this 1.20s window, taking hits accumulates posture or health damage, but Posture is **hard-capped at 99.9 points** and cannot trigger a secondary break, guaranteeing a legitimate window to deflect, dash, or slide to safety.

### 1.3 Deflect Multiplier Formula, Retreat Definition & Intent
The posture damage inflicted upon an attacking opponent during a successful deflect string scales deterministically according to the following formula:

$$\text{PostureDamage} = \text{BasePostureDamage} \times \min\left(1.0 + (\text{ConsecutiveDeflectCount} - 1) \times 0.25, \; 2.50\right)$$

Where:
- $\text{BasePostureDamage} = 24.0$ points.
- $\text{ConsecutiveDeflectCount}$ starts at $1$ and increments with each consecutive deflect executed within $0.80\text{s}$ of the previous deflect without receiving damage, missing a deflect input, or retreating.
- **Definition of "Retreat"**: A retreat occurs if the distance between the player and their primary opponent increases by more than **350 units (3.5m) within a single 0.80s chain window**, or if the player performs a backwards Flash Dash away from the opponent's strike radius. Doing so resets `ConsecutiveDeflectCount` back to 1.
- **Boss-Rhythm Driven Scaling Intent**: The 0.80s window is explicitly designed to reward mastering multi-hit boss clash strings (e.g. 3-to-5 hit rhythmic boss combos). By standing ground and deflecting in rhythm, the player rapidly escalates posture damage ($24 \to 30 \to 36 \to 42 \to 48$), turning the boss's most aggressive flurry into their quickest path to a Posture Break.
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
| **Neutral / Active Enemy (Posture < 100)** | Target within 1.8m (2.2m for Grappler), not posture broken | **`[G] Dedicated Grab`** (Head / Body lock) | If holding Forward (`W`): **Head Grab**.<br>If Neutral or Sideways (`A`/`D`/`S`): **Body / Limb Lock**. | Target immobilized in grab hold for up to **1.80s** (2.50s for Grappler). Player has **50% damage reduction** during hold. | **0 i-frames during hold**. Minions deal halved chip damage and cannot flinch player; unblockables break hold. Applies **0.35s minor hesitation** to nearby foes. |
| **Grab Held → Directed toward Hazard** | In Grab hold + Directional Input held | **`[G] + Direction`** (Throw into Hazard) | Checks line-of-sight in throw direction within 2.5m: Ledge > Pillar > Wall > Open Floor Throw. | Target launched into surface: Wall Splat inflicts 35% HP damage, 40 posture damage, and 2.0s prone state. | **0.85s i-frames upon throw release**. Thrown enemy becomes an active collision projectile, staggering any flankers hit in flight. |
| **Grab Held → Within 2.0m of Hazard** | Target Grabbed AND within 2.0m of surface | **`[F] Environmental Slam`** (Grab Chain) | Overrides normal grab hold. Prioritizes surface directly behind or to the flank of the grabbed victim. | Chains directly from grab into environmental smash (head slam on pillar or torso slam on wall) for 50% HP damage. | Seamless transition into **0.85s execution i-frames**; triggers full 0.85s crowd pause. |
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

## 4. Boss Duel Mode & Adds Specification

- **Arena**: The Sacred Dueling Ring at the lower basin of Sanctuary, surrounded by Shinto torii pillars, fallen stone lanterns, and high cliff walls.
- **Scout Lifecycle (Sanctuary Boss Arena Encounter)**:
  - Perimeter Scouts are **strictly finite (exactly 2 perimeter Scouts total, NO respawns)**.
  - Once eliminated by the player (e.g. by deflecting arrows back, throwing them off ledges, or using dash strikes), no additional adds spawn for the remainder of the encounter.
- **DuelArena Mode Adds Policy**:
  - **Zero Adds.** In dedicated `DuelArena` mode, it is a pure, sacred **1v1 duel** between the player and the Boss Rival / Kensei. The Crowd Director is inactive and no adds are present.
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
