# COMMITTED

An Unreal Engine martial arts action game built around **reversing commitment**. This repository is at the first gray-box combat prototype stage. The first goal is one player, one rival, and a fight whose outcome can change through a well-timed, directional reversal.

## Current prototype

- Third-person fighter with keyboard, mouse, and gamepad input mappings.
- Quick strike, limb strike, and slow committed strike. The committed strike defeats on a clean hit, but its long startup and recovery invite a reversal or punish.
- A 0.22-second directional reversal window. An attempt at the wrong time has recovery; a successful reversal staggers the attacker and lets the defender follow up.
- Leg impairment slows movement. Health, attack timings, ranges, and recovery are exposed to Blueprints for tuning.
- A simple, predictable rival AI for testing reads and timing.

This is a **gray-box combat prototype**, not a finished game. `Content/Maps/PrototypeDuel` is a test arena with block fighters, a simple training rival, status labels, and a controls HUD. There are no authored combat animations, VFX, sound effects, or packaged build yet. The C++ editor target was compiled in Unreal Engine 5.8 on Windows; the playable arena needs a fresh build of the current source.

## Open it in Unreal

1. Open `COMMITTED.uproject` in Unreal Engine 5.8 with Windows C++ tooling installed. Let Unreal compile the editor target after pulling new source.
2. Open `Content/Maps/PrototypeDuel` if another level is showing. The project's startup map is the duel arena for new editor sessions; the open Untitled landscape is a separate level.
3. Press **Play**. Use WASD and mouse; left click is quick strike, right click is limb strike, Q is the risky committed strike, and E is a timed reversal. The HUD displays these controls. Gamepad mappings are in `Config/DefaultInput.ini`.
4. Read the rival's state label. Reversal has a short front-facing window; a missed attempt leaves recovery. Restart Play to reset the duel.

The fighters are block figures for testing timing and spacing. The rival is spawned by `CommittedGameMode` only in `PrototypeDuel`, so other levels are free to use their own encounters. Build and reopen the editor after C++ changes; an editor session already running does not automatically load a newly compiled module.

## Combat rules to validate

1. Spamming reversal should lose to a delayed strike because a miss enters recovery.
2. A visible committed strike should be reversible from the defender's front and punishable on a miss.
3. A strike that connects can injure a leg or end a fight; an injury changes movement until the duel resets.
4. A reverse at the correct time should interrupt the attack and leave a follow-up opportunity.
5. The same timing rules must later survive a shift from third-person play to a side-view duel.

## Working direction

Levels combine third-person traversal, chases, and focused side-view fights while keeping the same combat rules. Fighters represent martial traditions and places through movement, tactics, and character, with supernatural signature abilities that have readable limits and counters. The initial code uses neutral gray-box strikes until the first fighter and visual tone are selected.

**Guard cancel and an attack-cancel system are experiments**, not core requirements. They will be added only if playtests show more expressive decisions without making committed lethal strikes safe. See `Docs/CombatVision.md` for the design boundaries.
