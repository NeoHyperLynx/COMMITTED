"""Generate COMMITTED's Sanctuary level blockout in Unreal Editor.
Combines Anthem's vertical Bastion canyons and Shaper monoliths with Ghost of Tsushima's Shinto shrine aesthetic.
Note: This is a Python Editor Scripting blockout generator placing geometry primitives, spawn locations,
and environmental takedown props for gameplay prototyping; final art is hand-authored.
"""
import unreal

level_path = "/Game/Maps/Sanctuary"
levels = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

if unreal.EditorAssetLibrary.does_asset_exist(level_path):
    unreal.log_warning(f"Sanctuary map already exists at {level_path}. Re-generating blockout...")
else:
    if not levels.new_level(level_path, False):
        raise RuntimeError("Could not create Sanctuary map")

cube = unreal.load_asset("/Engine/BasicShapes/Cube.Cube")
cylinder = unreal.load_asset("/Engine/BasicShapes/Cylinder.Cylinder")
if not cube or not cylinder:
    raise RuntimeError("Engine basic shapes missing")

def spawn_block(label, position, scale, mesh=cube):
    actor = actors.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(*position))
    actor.set_actor_label(label)
    smc = actor.get_component_by_class(unreal.StaticMeshComponent)
    smc.set_static_mesh(mesh)
    actor.set_actor_scale3d(unreal.Vector(*scale))
    return actor

# ==============================================================================
# 1. TIER 1: VISTA PEAK (Player Spawn & Overlook - Anthem Verticality)
# ==============================================================================
spawn_block("Vista Peak - Overlook Platform", (0, 0, 1200), (14, 14, 1))
spawn_block("Vista Peak - Rear Cliff", (0, -750, 1500), (18, 2, 8))
spawn_block("Vista Peak - West Rim", (-750, 0, 1400), (2, 16, 6))

player_start = actors.spawn_actor_from_class(
    unreal.PlayerStart, unreal.Vector(0, -300, 1320), unreal.Rotator(0, 90, 0))
player_start.set_actor_label("PlayerStart - Vista Overlook")

# ==============================================================================
# 2. TIER 2: SHINTO CANYON CHASM & BRIDGES (Ghost of Tsushima Traversal)
# ==============================================================================
# Wooden suspension bridges over the deep chasm
spawn_block("Canyon Chasm - Wood Bridge 1", (0, 950, 1050), (4, 12, 0.4))
spawn_block("Canyon Bridge - Rope Railing Left", (-200, 950, 1120), (0.2, 12, 1.2))
spawn_block("Canyon Bridge - Rope Railing Right", (200, 950, 1120), (0.2, 12, 1.2))

# Intermediate cliff landing
spawn_block("Canyon Chasm - Landing Shelf", (0, 1800, 900), (16, 12, 1))

# Shinto Torii Gate 1 at the bridge entrance
spawn_block("Torii 1 - Post Left", (-220, 1400, 1020), (0.4, 0.4, 2.5), cylinder)
spawn_block("Torii 1 - Post Right", (220, 1400, 1020), (0.4, 0.4, 2.5), cylinder)
spawn_block("Torii 1 - Crossbeam", (0, 1400, 1160), (5.5, 0.5, 0.4))

# ==============================================================================
# 3. TIER 3: SHAPER MONOLITH WALLS (Anthem Verticality - Wall Run Milestones)
# ==============================================================================
# Massive sheer monolith surfaces designed for wall running down the canyon
spawn_block("Shaper Monolith Wall Run Left", (-800, 2600, 750), (1.5, 24, 12))
spawn_block("Shaper Monolith Wall Run Right", (800, 2600, 750), (1.5, 24, 12))
spawn_block("Canyon Middle Shelf", (0, 2800, 500), (12, 18, 1))

# Torii Gate 2
spawn_block("Torii 2 - Post Left", (-240, 3600, 620), (0.4, 0.4, 2.5), cylinder)
spawn_block("Torii 2 - Post Right", (240, 3600, 620), (0.4, 0.4, 2.5), cylinder)
spawn_block("Torii 2 - Crossbeam", (0, 3600, 760), (5.5, 0.5, 0.4))

# ==============================================================================
# 4. TIER 4: SACRED DUELING RING (Boss Arena & Environmental Takedown Props)
# ==============================================================================
# Sunken basin arena floor
spawn_block("Sacred Dueling Basin - Floor", (0, 4800, 50), (32, 32, 1))

# Arena Boundary Cliffs
spawn_block("Arena North Cliff Wall", (0, 6400, 400), (34, 1.5, 8))
spawn_block("Arena South Monolith Wall", (0, 3200, 400), (34, 1.5, 8))
spawn_block("Arena East Wall", (1600, 4800, 400), (1.5, 34, 8))
spawn_block("Arena West Wall", (-1600, 4800, 400), (1.5, 34, 8))

# Environmental Takedown Props (Pillars & Lanterns for Sleeping Dogs / Urban Reign mechanics)
pillar_offsets = [
    ("Pillar - Northeast", (600, 5400, 200)),
    ("Pillar - Northwest", (-600, 5400, 200)),
    ("Pillar - Southeast", (600, 4200, 200)),
    ("Pillar - Southwest", (-600, 4200, 200)),
    ("Pillar - Center Shrine Post Left", (-250, 4800, 200)),
    ("Pillar - Center Shrine Post Right", (250, 4800, 200)),
]

for name, pos in pillar_offsets:
    spawn_block(name, pos, (0.8, 0.8, 3.5), cylinder)

# Grand Master Torii framing the Sacred Shrine
spawn_block("Grand Torii - Left Pillar", (-450, 5900, 250), (0.7, 0.7, 4.5), cylinder)
spawn_block("Grand Torii - Right Pillar", (450, 5900, 250), (0.7, 0.7, 4.5), cylinder)
spawn_block("Grand Torii - Upper Crossbeam", (0, 5900, 480), (11, 0.8, 0.7))

# ==============================================================================
# 5. LIGHTING & ATMOSPHERE (Anthem Mist + Tsushima Golden Hour)
# ==============================================================================
sun = actors.spawn_actor_from_class(
    unreal.DirectionalLight, unreal.Vector(0, 0, 2200), unreal.Rotator(-35, -45, 0))
sun.set_actor_label("Sanctuary Golden Sun")

skylight = actors.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 1800))
skylight.set_actor_label("Sanctuary Ambient Sky")

fog = actors.spawn_actor_from_class(unreal.ExponentialHeightFog, unreal.Vector(0, 2400, 400))
fog.set_actor_label("Bastion Canyon Mist Fog")

if not levels.save_current_level():
    raise RuntimeError("Could not save Sanctuary map")

unreal.log("Sanctuary blockout successfully generated with Bastion verticality and Tsushima shrine landmarks.")
