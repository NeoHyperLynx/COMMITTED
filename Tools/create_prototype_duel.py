"""Create COMMITTED's first separate gray-box duel arena in Unreal Editor."""
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

block("Arena Floor", (0, 0, -50), (18, 12, 1))
block("North Boundary", (0, 600, 80), (18, .25, 1.6))
block("South Boundary", (0, -600, 80), (18, .25, 1.6))
block("East Boundary", (900, 0, 80), (.25, 12, 1.6))
block("West Boundary", (-900, 0, 80), (.25, 12, 1.6))
for x in (-700, 700):
    for y in (-420, 420):
        block("Corner Post", (x, y, 95), (.35, .35, 1.9))

start = actors.spawn_actor_from_class(
    unreal.PlayerStart, unreal.Vector(-280, 0, 110), unreal.Rotator(0, 0, 0))
start.set_actor_label("Player Spawn - Face Rival")
sun = actors.spawn_actor_from_class(
    unreal.DirectionalLight, unreal.Vector(0, 0, 500), unreal.Rotator(-48, -28, 0))
sun.set_actor_label("Duel Sun")
skylight = actors.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 250))
skylight.set_actor_label("Duel Ambient Light")

if not levels.save_current_level():
    raise RuntimeError("Could not save PrototypeDuel")
unreal.log("COMMITTED PrototypeDuel created with arena, lights, and player start")
