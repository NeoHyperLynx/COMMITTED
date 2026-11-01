# Kumite Roster Design & Fighter Registry Specification

## 1. Reference Asset Notice
The image stored in `Docs/Images/reference/Kumite_Roster_Moodboard.jpg` is an AI-generated conceptual mood board created for marketing and visual inspiration. It contains duplicate, fictionalized, or conflated entries and is **strictly reference-only, not canon**. It must never be used directly in shipping UI or treated as an authoritative roster list.

For the production vertical slice, the game defines **15 real, documented martial arts traditions**, each thoroughly researched with verified mechanical profiles and distinct cultural lineages.

---

## 2. Four Core Exchange Identities
Instead of fifteen separate mechanic sets, each fighter is mapped to one of four shared **Exchange Identities**, ensuring tight balance and high depth:

1. **Grapplers** (`EFighterExchangeIdentity::Grappler`):
   - *Traditions*: Judo, Laamb, Sambo.
   - *Exchange Advantage*: Dedicated grab (`G`) → Directional throw into hazards → Wall Splat grounded chains. Extended grab hold window and bonus defense during grabs.

2. **Stick & Blade Fighters** (`EFighterExchangeIdentity::StickWeapon`):
   - *Traditions*: Arnis / Kali, Tahtib, Nguni Stick Fighting, Khridoli.
   - *Exchange Advantage*: Superior weapon reach, unblockable tip attacks, and automatic weapon disarm posture chunking on perfect deflects, balanced by higher recovery on whiffs. High base posture pool, but lower Flow generation rate.

3. **Bare-Hand Strikers** (`EFighterExchangeIdentity::BareHandStriker`):
   - *Traditions*: Muay Thai, Karate (Shotokan), Taekwondo, Cuban Boxing.
   - *Exchange Advantage*: Rapid deflect-to-counter strings, high hit-confirms, fast frame startup, and high Flow accumulation on hits. Moderate posture thresholds.

4. **Kicking & Flow Acrobats** (`EFighterExchangeIdentity::KickingFlow`):
   - *Traditions*: Capoeira, Savate, Wing Chun, Pencak Silat.
   - *Exchange Advantage*: Flash-dash-cancelable recovery windows directly tied to the agile movement suite. Seamless transition from evasive sweeps and feints into heavy strikes. Low posture max, but explosive Flow generation.

---

## 3. Verified 15-Fighter Vertical Slice Roster

| # | Fighter / Tradition | Region | Exchange Identity | Archetype | Posture Max | Flow Gain Rate | Signature Move | Style Tags |
|---|---|---|---|---|---|---|---|---|
| 1 | **Muay Thai** | Thailand | BareHandStriker | Striker | 95.0 | 14.0 | Flying Clinch Knee | `Style.Barehand`, `Style.KneeElbow` |
| 2 | **Karate (Shotokan)** | Japan | BareHandStriker | Striker | 100.0 | 12.0 | Gyaku-Zuki Reverse Punch | `Style.Barehand`, `Style.LinearStrike` |
| 3 | **Taekwondo** | South Korea | BareHandStriker | Striker | 85.0 | 15.0 | Tornado 540 Roundhouse | `Style.Barehand`, `Style.Kick` |
| 4 | **Cuban Boxing** | Cuba | BareHandStriker | Striker | 105.0 | 13.0 | Cross Counter Lead Hook | `Style.Barehand`, `Style.Boxing` |
| 5 | **Judo** | Japan | Grappler | Grappler | 115.0 | 9.0 | Ippon Seoi Nage (Shoulder Throw) | `Style.Grapple`, `Style.Throw` |
| 6 | **Laamb** | Senegal | Grappler | Grappler | 125.0 | 8.0 | Mbapat Sweep Slam | `Style.Grapple`, `Style.Barehand` |
| 7 | **Sambo** | Eastern Europe | Grappler | Grappler | 110.0 | 10.0 | Rolling Kneebar Leg Lock | `Style.Grapple`, `Style.LegLock` |
| 8 | **Arnis / Kali** | Philippines | StickWeapon | Weapon | 120.0 | 7.0 | Sinawali Double-Stick Flurry | `Style.Stick`, `Style.Weapon` |
| 9 | **Tahtib** | Egypt | StickWeapon | Weapon | 115.0 | 8.0 | Asaya Overhead Monolith Strike | `Style.Stick`, `Style.Weapon` |
| 10 | **Nguni Stick Fighting** | Southern Africa | StickWeapon | Weapon | 120.0 | 8.0 | Induku Shield-Bash & Crack | `Style.Stick`, `Style.Shield` |
| 11 | **Khridoli** | Georgia | StickWeapon | Weapon | 110.0 | 9.0 | Sabatoba Saber Blade Slash | `Style.Weapon`, `Style.Blade` |
| 12 | **Capoeira** | Brazil | KickingFlow | Hybrid | 80.0 | 18.0 | Meia Lua de Compasso | `Style.Kick`, `Style.Acrobatic` |
| 13 | **Savate** | France | KickingFlow | Striker | 85.0 | 16.0 | Fouetté High Whip Kick | `Style.Kick`, `Style.Boxing` |
| 14 | **Wing Chun** | China | KickingFlow | Hybrid | 90.0 | 15.0 | Chain Punch Centerline Blitz | `Style.Barehand`, `Style.CloseQuarter` |
| 15 | **Pencak Silat** | Indonesia | KickingFlow | Hybrid | 85.0 | 17.0 | Harimau Low Sweep & Snap | `Style.Blade`, `Style.Acrobatic` |
