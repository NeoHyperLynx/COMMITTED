# COMMITTED

An Unreal Engine martial arts action game built around **reversing commitment**. This repository is at the first gray-box combat prototype stage. The first goal is one player, one rival, and a fight whose outcome can change through a well-timed, directional reversal.

## Current prototype

- Third-person fighter with keyboard, mouse, and gamepad input mappings.
- Quick strike, limb strike, and slow committed strike. The committed strike defeats on a clean hit, but its long startup and recovery invite a reversal or punish.
- A 0.22-second directional reversal window. An attempt at the wrong time has recovery; a successful reversal staggers the attacker and lets the defender follow up.
- Leg impairment slows movement. Health, attack timings, ranges, and recovery are exposed to Blueprints for tuning.
- A simple, predictable rival AI for testing reads and timing.

This is a **code prototype**, not a finished playable level. There are no character models, animations, VFX, UI, map, or packaged build yet. Unreal Editor is required to compile and set up the test level. The C++ editor target compiled successfully in Unreal Engine 5.8 on Windows. Play feel still needs a Blueprint level, characters, and animation.

## Open it in Unreal

1. Install a recent Unreal Engine 5 release with C++ tooling on Windows. Open `COMMITTED.uproject` and let the editor generate and compile the project files.
2. Create a new Basic level with a floor and lighting. Save it under `Content/Maps/PrototypeDuel` and set it as the editor and game startup map in Project Settings.
3. Create a Blueprint child of `CommittedFighterCharacter` named `BP_Player`. Assign a mannequin skeletal mesh and animation blueprint. Set this Blueprint as the Default Pawn Class in a Blueprint child of `CommittedGameMode`, then set that Game Mode in World Settings. The camera and combat component already exist on the C++ class.
4. Place another `BP_Player` child in the level as `BP_Rival`. Set `Prototype Rival AI` to true and `Auto Possess AI` to Placed in World or Spawned. The rival will approach and use simple strikes.
5. Add visual reactions to `Combat` events `OnCombatStateChanged` and `OnHitReceived` in the Blueprints. For early timing tests, use distinct material colors or `Print String` for Startup, Active, Recovery, Reversal, Staggered, and Defeated. Animation and VFX should follow these state events rather than determine hit timing in this first test.
6. Press Play. Use WASD and mouse; left click is quick strike, right click is limb strike, Q is committed strike, and E is reversal. Gamepad mappings are in `Config/DefaultInput.ini`.

The default game mode spawns the raw C++ fighter if no Blueprint Game Mode is selected. It has no mesh; use the Blueprint setup above to see characters.

## Combat rules to validate

1. Spamming reversal should lose to a delayed strike because a miss enters recovery.
2. A visible committed strike should be reversible from the defender's front and punishable on a miss.
3. A strike that connects can injure a leg or end a fight; an injury changes movement until the duel resets.
4. A reverse at the correct time should interrupt the attack and leave a follow-up opportunity.
5. The same timing rules must later survive a shift from third-person play to a side-view duel.

## Working direction

Levels combine third-person traversal, chases, and focused side-view fights while keeping the same combat rules. Fighters represent martial traditions and places through movement, tactics, and character, with supernatural signature abilities that have readable limits and counters. The initial code uses neutral gray-box strikes until the first fighter and visual tone are selected.

**Guard cancel and an attack-cancel system are experiments**, not core requirements. They will be added only if playtests show more expressive decisions without making committed lethal strikes safe. See `Docs/CombatVision.md` for the design boundaries.
