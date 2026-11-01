# COMMITTED

An Unreal Engine martial arts action game featuring high-speed agility inspired by **Phantom Blade Zero**, **Anthem (Bastion)** verticality, **Ghost of Tsushima** aesthetics, **Sifu** posture and 1vN crowd flow, **Sleeping Dogs** / **The Matrix: Path of Neo** environmental takedowns, **Urban Reign** grabs and fighting depth, and **Sekiro** deflect rhythm.

---

## Kumite Master Roster (Data-Driven Fighter Registry)

The vertical slice features a data-driven fighter registry (`FCommittedFighterDefinition`) representing **15 verified global martial arts traditions** mapped to **4 distinct Exchange Identities**:

1. **Bare-Hand Strikers**: *Muay Thai, Karate (Shotokan), Taekwondo, Cuban Boxing*. Fast deflect-to-counter strings and rapid Flow generation.
2. **Grapplers**: *Judo, Laamb (Senegalese wrestling), Sambo*. Dedicated grab → directional throws into hazards → Wall Splat prone chains with extended grab hold resistance.
3. **Stick & Blade Weapons**: *Arnis / Kali, Tahtib, Nguni Stick Fighting, Khridoli*. High reach, tip strikes, and disarm posture chunking on deflects with high base posture.
4. **Kicking & Flow Acrobats**: *Capoeira, Savate, Wing Chun, Pencak Silat*. Flash-dash-cancelable recovery windows directly tied into the movement suite and explosive Flow generation.

*(Note: The mood board image in `Docs/Images/reference/Kumite_Roster_Moodboard.jpg` is an AI-generated conceptual reference for marketing/mood; the vertical slice focuses strictly on the 15 verified traditions above).*

---

## 1. World & Level: Sanctuary

The startup map `Sanctuary` (`Content/Maps/Sanctuary.umap`, generated via `Tools/create_sanctuary_map.py`) unites Anthem's sheer verticality with Tsushima's serene shrine atmosphere:
- **Vista Peak (Overlook Spawn)**: Spawns the player 120m high above the canyon with a vista overlook.
- **Canyon Chasm & Shinto Bridges**: Suspension bridges spanning deep chasms with Torii gates and narrow paths.
- **Shaper Monolith Walls**: Towering sheer monolith walls designed for high-speed wall running and parkour descent.
- **Sacred Dueling Ring (Boss Arena)**: Sunken arena surrounded by Shinto stone pillars, Torii structures, and monolith barriers.

---

## 2. Combat Pillars & Mechanics

### Posture & Flow System (Sifu & Sekiro)
- **Posture (0–100)**: Guarding or taking strikes increases Posture. Reaching 100 Posture triggers **Posture Break**:
  - Enemies enter a 2.0s vulnerable state susceptible to lethal criticals and environmental slams.
  - Player break causes a 1.4s stagger taking 1.75× damage.
- **Deflect Window (0.14s)**: Pressing `E` precisely deflects incoming strikes, dealing 24 posture damage to the attacker and awarding 18 Flow. Consecutive deflects scale posture damage up to 2.5×.
- **Flow (0–100)**: Offensive kinetic momentum built from clean strikes (+12), deflects (+18), and parkour (+8). Unlocks free dash cancels, bonus posture damage, and Phantom State at 100 Flow.

### Environmental Takedowns (Sleeping Dogs & Path of Neo)
- Within 2.0m of an environmental obstacle when an opponent is posture-broken:
  - **Ledge / Chasm**: Instant ejection off the cliff.
  - **Pillar / Torii Post**: Structural slam dealing 75% max HP and wall splat stumble.
  - **Wall**: High-impact wall smash with i-frames during execution.
- Press `F` when prompted to execute.

### Dedicated Grab & Throws (Urban Reign)
- Press `G` to initiate a dedicated grab targeting head or body.
- Hold `G` + Direction to hurl the enemy toward adjacent walls (Wall Splat prone) or off ledges.

### 1vN Crowd Director (Sifu / Arkham Rhythm)
- The Crowd Director enforces attack tokens (max 2 concurrent attackers in standard skirmishes).
- Enemy archetypes:
  1. **Grunt (Striker)**: Frontal 2-hit quick strikes, retreats to perimeter when token is lost.
  2. **Bruiser (Grappler & Heavy)**: Armored overhead swings, immune to light flinch.
  3. **Skirmisher (Agile Flanker)**: Rapid flank circling and slide sweep kicks.
  4. **Scout (Ranged Disruptor)**: Distant perimeter harasser.
  5. **The Sanctuary Boss Rival**: Multi-hit clash strings and phase transitions.

---

## 3. Controls

| Action | Keyboard / Mouse | Gamepad |
| --- | --- | --- |
| Move | `W` `A` `S` `D` | Left Stick |
| Look | Mouse | Right Stick |
| Sprint | `Left Shift` | Left Stick Click |
| Slide | `Left Ctrl` / `C` | `B` / `Circle` |
| Flash Dash | `Left Alt` | `Left Trigger` |
| Jump / Wall Kick | `Spacebar` | `A` / `Cross` |
| Quick / Movement Strike | Left Mouse Button | `X` / `Square` |
| Limb Strike | Right Mouse Button | `Y` / `Triangle` |
| Committed Strike | `Q` | `Right Trigger` |
| Deflect / Reversal | `E` | `Left Shoulder` |
| Environmental Takedown | `F` | `B` / `Circle` |
| Grab / Throw | `G` | `Left Shoulder` |

---

## 4. Open it in Unreal

1. Open `COMMITTED.uproject` in Unreal Engine 5.8 with Windows C++ tooling installed.
2. The project's startup map is `Sanctuary`.
3. Press **Play** to start at Vista Peak and traverse through the Shinto canyon to duel the boss in the Sacred Dueling Ring!
