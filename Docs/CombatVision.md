# DEVOTED — first combat slice

## Promise

Commitment creates danger and opportunity. A fighter can reverse an exchange by recognizing the opponent's intent, choosing the right angle, and acting within a readable window. A reversal is earned; it is never a guaranteed rescue from every mistake.

## Level language

The camera changes with the encounter: third-person exploration and room fights, fast pursuit through traversable spaces, and side-view duels where spacing is especially legible. Controls and damage rules remain consistent. Cinematic finishing reversals are brief rewards for winning the read, not a substitute for player input.

## Risk ladder

| Action | Commitment | Reward | Counterplay |
| --- | --- | --- | --- |
| Quick strike | Short | Interrupt and positioning | Block, distance, well-timed reverse |
| Limb strike | Moderate | Lasting impairment, such as slower footwork | Readable windup and larger recovery |
| Committed strike | High | Fight-ending clean hit | Reversal, evasion, or punish on miss |
| Reversal | Narrow timing | Interrupt and follow-up window | Delay, change angle, or bait the attempt |

Early prototype values in code are tuning placeholders. Lethal hits should be available to player and rival under the same rules. The eventual visual system must show startup, active contact, recovery, injury, and defeat clearly. No random one-hit outcomes.

## Supernatural signature abilities

Characters in `Committed.md` propose abilities such as kinetic rhythm and staff balance. Preserve their distinct identities, but put a clear cost, tell, counter, and recovery on each ability. They must create a choice or change an angle rather than erase a clean reversal or make an unsafe lethal move automatically safe. Choose the first fighter and confirm the grounded-to-fantastical visual tone before implementing character-specific powers.

## Guard cancel experiment

A guard cancel would spend a scarce resource after a successful block to reposition or break pressure. It cannot create a free reversal and should remain punishable if anticipated. First test guard, reversal, and committed strikes without it. Add it only if defense becomes passive or repetitive. The same criterion applies to the proposed `Recommit` attack cancel; neither is in the first code slice.

## First milestone acceptance

- One gray-box player and one rival in a simple room.
- Both can telegraph, land, miss, and reverse attacks under the same rules.
- A leg injury measurably slows movement; a clean committed strike ends the encounter.
- An unsuccessful reversal or committed strike leaves an exploitable recovery.
- Run a short playtest to tune timings before adding a chase or side-view camera.
