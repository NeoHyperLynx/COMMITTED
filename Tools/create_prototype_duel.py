"""Create COMMITTED's duel and agility arena with wall-run surfaces in Unreal Editor."""
import unreal

level_path = "/Game/Maps/PrototypeDuel"
levels = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
if unreal.EditorAssetLibrary.does_asset_exist(level_path):
    raise RuntimeError(f"Refusing to replace existing level: {level_path}")
if not levels.new_level(level_path, False):
    raise RuntimeError("Could not create PrototypeDuel")

cube = unreal.load_asset("/Engine/BasicShapes/Cube.Cube")
if not cube:
    raise RuntimeError("Engine basic cube mesh is missing")

def block(label, position, scale):
    actor = actors.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(*position))
    actor.set_actor_label(label)
    actor.get_component_by_class(unreal.StaticMeshComponent).set_static_mesh(cube)
    actor.set_actor_scale3d(unreal.Vector(*scale))
    return actor

# Main arena floor
block("Arena Floor", (0, 0, -50), (24, 18, 1))

# Boundaries - elevated for wall running and traversal
block("North Boundary Wall", (0, 900, 150), (24, .35, 3.2))
block("South Boundary Wall", (0, -900, 150), (24, .35, 3.2))
block("East Boundary Wall", (1200, 0, 150), (.35, 18, 3.2))
block("West Boundary Wall", (-1200, 0, 150), (.35, 18, 3.2))

# Interior wall-run obstacles and parkour barriers
block("Parkour Wall Left", (-250, 450, 150), (8, .35, 3.0))
block("Parkour Wall Right", (250, -450, 150), (8, .35, 3.0))

# Slide obstacle (low clearance beam)
block("Slide Low Beam", (0, 200, 95), (6, .35, .6))

for x in (-1000, 1000):
    for y in (-750, 750):
        block("Corner Post", (x, y, 160), (.5, .5, 3.2))

start = actors.spawn_actor_from_class(
    unreal.PlayerStart, unreal.Vector(-350, 0, 110), unreal.Rotator(0, 0, 0))
start.set_actor_label("Player Spawn - Face Rival")
sun = actors.spawn_actor_from_class(
    unreal.DirectionalLight, unreal.Vector(0, 0, 500), unreal.Rotator(-48, -28, 0))
sun.set_actor_label("Duel Sun")
skylight = actors.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 250))
skylight.set_actor_label("Duel Ambient Light")

if not levels.save_current_level():
    raise RuntimeError("Could not save PrototypeDuel")
unreal.log("COMMITTED PrototypeDuel created with arena, wall-run surfaces, lights, and player start")
