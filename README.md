# COMMITTED

An Unreal Engine martial arts action game combining **reversing commitment** with high-speed martial arts movement inspired by **Phantom Blade Zero**.

## Agility & Movement Suite (Phantom Blade Zero Inspired)

The movement mechanics deliver high-speed martial arts traversal and evasive combat fluidity:
- **Acrobatic Sprint**: Hold `Shift` (`Left Thumbstick`) to sprint at high velocity with dynamic camera FOV widening.
- **Low Slide**: Press `Ctrl` or `C` (`Gamepad B/Circle`) while moving to initiate a low slide under obstacles with decaying momentum. Connect with Left Click for a **Low Sweep Strike**.
- **Omnidirectional Flash Dash / Phantom Evade**: Press `Alt` or `F` (`Left Trigger`) with directional input for an instantaneous martial arts dash. Cancels attack recovery windows to reposition or counter-attack immediately. Connect with Left Click for a **Phantom Flash Strike**.
- **Wall Running & Wall Kick**: Jump or sprint along vertical surfaces to engage horizontal wall running. Press `Space` (`Gamepad A/Cross`) during a wall run to execute a powerful Wall Kick impulse off the surface.
- **Aerial Agility**: Air jump / double jump allows mid-air redirection and vertical repositioning.

## Combat Prototype

- **Third-person fighter** with responsive keyboard, mouse, and gamepad input mappings.
- **Quick strike, limb strike, committed strike, dash strike, and slide strike**:
  - The committed strike defeats on a clean hit, but its long startup and recovery invite a reversal or punish.
  - Dash strikes close distances swiftly after a phantom evade.
  - Slide strikes target the lower limbs.
- **0.22-second directional reversal window**: An attempt at the wrong time incurs recovery; a successful reversal staggers the attacker.
- **Dynamic leg impairment**: Leg injury measurably slows both walking and sprint velocities until the duel resets.
- **Predictable training rival AI**: Capable of closing distances with sprints, initiating attacks, and reacting with reversals or defensive flash dashes.

## Controls

| Action | Keyboard / Mouse | Gamepad |
| --- | --- | --- |
| Move | `W` `A` `S` `D` | Left Stick |
| Look | Mouse | Right Stick |
| Sprint | `Left Shift` | Left Stick Click |
| Slide | `Left Ctrl` / `C` | `B` / `Circle` |
| Flash Dash | `Left Alt` / `F` | `Left Trigger` |
| Jump / Wall Kick | `Spacebar` | `A` / `Cross` |
| Quick Strike / Dash / Slide Strike | Left Mouse Button | `X` / `Square` |
| Limb Strike | Right Mouse Button | `Y` / `Triangle` |
| Committed Strike | `Q` | `Right Trigger` |
| Timed Reversal | `E` | `Left Shoulder` |

## Open it in Unreal

1. Open `COMMITTED.uproject` in Unreal Engine 5.8 with Windows C++ tooling installed.
2. Open `Content/Maps/PrototypeDuel`.
3. Press **Play**. Test high-speed sprinting, slides, wall running along the arena boundaries, wall kicks, and flash dashing through combat exchanges.
