"""Rebuild only the isolated UE5 comparison room from retained, owned assets."""
import json
import os
import unreal

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SCENERY = os.path.abspath(os.path.join(ROOT, "..", "..", "Assets", "Art", "Scenery"))
LEVEL = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
ACTORS = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
ASSETS = unreal.EditorAssetLibrary
EDIT = unreal.MaterialEditingLibrary
TOOLS = unreal.AssetToolsHelpers.get_asset_tools()
MAP = "/Game/VisualLab/Comparison"
if not LEVEL.load_level(MAP):
    raise RuntimeError("The copied UE4 comparison map must be present")
for actor in ACTORS.get_all_level_actors():
    if actor.get_class().get_name() not in ("WorldSettings", "Brush"):
        ACTORS.destroy_actor(actor)

def texture(filename):
    name = "Scenery_" + filename.replace(" ", "_")
    task = unreal.AssetImportTask()
    task.filename = os.path.join(SCENERY, filename + ".png")
    task.destination_path = "/Game/VisualLab/Textures"
    task.destination_name = name
    task.automated = True
    task.save = True
    task.replace_existing = True
    TOOLS.import_asset_tasks([task])
    paths = task.get_editor_property("imported_object_paths")
    if not paths:
        raise RuntimeError("Texture import failed: " + filename)
    return ASSETS.load_asset(paths[0])

def material(name, rgb, roughness=0.8, image=None):
    folder = "/Game/VisualLab/UE5Materials"
    path = folder + "/" + name
    mat = ASSETS.load_asset(path) if ASSETS.does_asset_exist(path) else TOOLS.create_asset(name, folder, unreal.Material, unreal.MaterialFactoryNew())
    EDIT.delete_all_material_expressions(mat)
    base = EDIT.create_material_expression(mat, unreal.MaterialExpressionConstant3Vector)
    base.set_editor_property("constant", unreal.LinearColor(*rgb, 1.0))
    if image:
        sample = EDIT.create_material_expression(mat, unreal.MaterialExpressionTextureSample)
        sample.set_editor_property("texture", image)
        multiply = EDIT.create_material_expression(mat, unreal.MaterialExpressionMultiply)
        EDIT.connect_material_expressions(base, "", multiply, "A")
        EDIT.connect_material_expressions(sample, "RGB", multiply, "B")
        base = multiply
    EDIT.connect_material_property(base, "", unreal.MaterialProperty.MP_BASE_COLOR)
    rough = EDIT.create_material_expression(mat, unreal.MaterialExpressionConstant)
    rough.set_editor_property("r", roughness)
    EDIT.connect_material_property(rough, "", unreal.MaterialProperty.MP_ROUGHNESS)
    EDIT.recompile_material(mat)
    ASSETS.save_loaded_asset(mat)
    return mat

CUBE = ASSETS.load_asset("/Engine/BasicShapes/Cube.Cube")
SPHERE = ASSETS.load_asset("/Engine/BasicShapes/Sphere.Sphere")
CYLINDER = ASSETS.load_asset("/Engine/BasicShapes/Cylinder.Cylinder")
wood = material("Oak", (0.65, 0.50, 0.34), 0.56, texture("Oak"))
cream = material("Limewash", (0.86, 0.80, 0.69), 0.94, texture("Plaster"))
cloth = material("WovenBlue", (0.38, 0.54, 0.77), 0.98, texture("Woven blue"))
sage = material("SageUpholstery", (0.24, 0.35, 0.27), 0.94)
ochre = material("OchreCushion", (0.70, 0.39, 0.13), 0.94)
ivory = material("Ivory", (0.78, 0.72, 0.61), 0.8)
leaf = material("Leaf", (0.07, 0.20, 0.06), 0.75)
pot = material("Terracotta", (0.36, 0.12, 0.055), 0.85)

def mesh(name, asset, xyz, scale, mat=None, yaw=0):
    actor = ACTORS.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(*xyz), unreal.Rotator(yaw=yaw))
    actor.set_actor_label(name)
    actor.set_actor_scale3d(unreal.Vector(*scale))
    actor.static_mesh_component.set_static_mesh(asset)
    if mat:
        actor.static_mesh_component.set_material(0, mat)
    return actor

mesh("Solid floor", CUBE, (0, 0, -12), (10, 10, 0.22), wood)
for row in range(20):
    mesh("Oak floorboard %02d" % row, CUBE, (0, -475 + row * 50, 0), (10, 0.49, 0.02), wood)
mesh("Back wall", CUBE, (490, 0, 180), (0.20, 10, 3.6), cream)
mesh("Window wall left", CUBE, (0, 490, 180), (3.0, 0.20, 3.6), cream)
mesh("Window wall right", CUBE, (400, 490, 180), (1.8, 0.20, 3.6), cream)
mesh("Window sill wall", CUBE, (230, 490, 45), (1.6, 0.20, 0.9), cream)
mesh("Window header", CUBE, (230, 490, 330), (1.6, 0.20, 0.6), cream)
for x in (145, 315):
    mesh("Oak window frame", CUBE, (x, 476, 195), (0.08, 0.14, 2.1), wood)
mesh("Window sill", CUBE, (230, 470, 95), (1.9, 0.42, 0.10), ivory)
mesh("Window lintel", CUBE, (230, 476, 300), (1.8, 0.14, 0.10), ivory)
mesh("Window divider", CUBE, (230, 476, 198), (0.05, 0.12, 2.0), ivory)
for y in (-475, 475):
    mesh("Floor trim", CUBE, (0, y, 7), (9.8, 0.06, 0.14), ivory)
mesh("Blue woven rug", CUBE, (25, -25, 3), (4.8, 6.2, 0.04), cloth)
mesh("Sofa seat", SPHERE, (340, -15, 46), (1.75, 4.4, 0.55), sage)
mesh("Sofa back", SPHERE, (407, -15, 90), (0.42, 4.5, 1.3), sage)
for y in (-230, 200):
    mesh("Sofa arm", SPHERE, (340, y, 72), (1.9, 0.40, 0.85), sage)
for y in (-100, 100):
    mesh("Cozy cushion", SPHERE, (350, y, 94), (0.33, 0.78, 0.75), ochre, 12)
mesh("Side table top", CYLINDER, (300, 320, 67), (1.0, 1.0, 0.08), wood)
mesh("Side table pedestal", CYLINDER, (300, 320, 32), (0.13, 0.13, 0.64), wood)
mesh("Plant pot", CYLINDER, (300, 320, 85), (0.32, 0.32, 0.28), pot)
for i in range(5):
    mesh("Plant leaf %d" % i, SPHERE, (285 + i * 7, 310 + (i % 2) * 20, 110 + (i % 3) * 8), (0.12, 0.30, 0.12), leaf, i * 61)
for name, y in (("Marty", -170), ("Lakitu", 0), ("Rio", 170)):
    asset = ASSETS.load_asset("/Game/VisualLab/Pets/" + name)
    if not asset:
        raise RuntimeError("Retained pet mesh missing: " + name)
    # Rebuild the UE5 copy's material graphs from retained colour/coat metadata.
    # Keep slot names rather than relying on OBJ import-order material indices.
    with open(os.path.join(ROOT, "..", "VisualLab", "SourceAssets", name + ".json"), encoding="utf-8-sig") as stream:
        spec = json.load(stream)
    pet_materials = {}
    for entry in spec["materials"]:
        color = entry["color"]
        coat = ASSETS.load_asset("/Game/VisualLab/Textures/" + name + "_" + entry["key"]) if entry["texture"] else None
        if entry["texture"] and not coat:
            raise RuntimeError("Retained coat texture missing: " + name)
        pet_materials[entry["key"]] = material(name + "_" + entry["key"], (color["r"], color["g"], color["b"]), 0.85, coat)
    for index, slot in enumerate(asset.get_editor_property("static_materials")):
        key = str(slot.material_slot_name)
        if key not in pet_materials and slot.material_interface:
            key = slot.material_interface.get_name().split("_")[-1]
        if key not in pet_materials:
            raise RuntimeError("Unmapped pet material slot: " + name + "/" + key)
        asset.set_material(index, pet_materials[key])
    ASSETS.save_loaded_asset(asset)
    mesh(name + " | static comparison", asset, (50, y, 6), (1, 1, 1), yaw=180)

sun = ACTORS.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0, 0, 400), unreal.Rotator(pitch=-38, yaw=-55))
sun.light_component.set_editor_property("intensity", 1.2)
sun.light_component.set_light_color(unreal.LinearColor(1.0, 0.86, 0.68))
sun.light_component.set_editor_property("mobility", unreal.ComponentMobility.MOVABLE)
sun.light_component.set_editor_property("atmosphere_sun_light", True)
ACTORS.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector(0, 0, 0))
sky = ACTORS.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 250))
sky.light_component.set_editor_property("intensity", 0.45)
sky.light_component.set_editor_property("mobility", unreal.ComponentMobility.MOVABLE)
sky.light_component.set_editor_property("real_time_capture", True)
window_light = ACTORS.spawn_actor_from_class(unreal.RectLight, unreal.Vector(220, 430, 220), unreal.Rotator(yaw=-90))
window_light.set_actor_label("Soft window daylight")
window_light.light_component.set_editor_property("intensity", 280.0)
window_light.light_component.set_editor_property("source_width", 170.0)
window_light.light_component.set_editor_property("source_height", 190.0)
window_light.light_component.set_editor_property("mobility", unreal.ComponentMobility.MOVABLE)
window_light.light_component.set_light_color(unreal.LinearColor(0.72, 0.84, 1.0))
lamp = ACTORS.spawn_actor_from_class(unreal.PointLight, unreal.Vector(260, 280, 160))
lamp.light_component.set_editor_property("intensity", 45.0)
lamp.light_component.set_editor_property("attenuation_radius", 500.0)
lamp.light_component.set_editor_property("mobility", unreal.ComponentMobility.MOVABLE)
lamp.light_component.set_light_color(unreal.LinearColor(1.0, 0.72, 0.38))
fill = ACTORS.spawn_actor_from_class(unreal.RectLight, unreal.Vector(-280, -240, 220), unreal.Rotator(pitch=-18, yaw=40))
fill.set_actor_label("Broad soft pet fill")
fill.light_component.set_editor_property("intensity", 12.0)
fill.light_component.set_editor_property("source_width", 260.0)
fill.light_component.set_editor_property("source_height", 180.0)
fill.light_component.set_editor_property("mobility", unreal.ComponentMobility.MOVABLE)
fill.light_component.set_light_color(unreal.LinearColor(1.0, 0.93, 0.84))
ACTORS.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(-360, -320, 120), unreal.Rotator(pitch=-6, yaw=40))
camera = ACTORS.spawn_actor_from_class(unreal.CameraActor, unreal.Vector(-360, -360, 150), unreal.Rotator(pitch=-9, yaw=42))
camera.set_actor_label("UE5 comparison camera")
camera.camera_component.set_editor_property("field_of_view", 60.0)
LEVEL.set_level_viewport_camera_info(unreal.Vector(-360, -360, 150), unreal.Rotator(pitch=-9, yaw=42), unreal.Name("None"))
world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
world.get_world_settings().set_editor_property("force_no_precomputed_lighting", True)
if not LEVEL.save_current_level():
    raise RuntimeError("UE5 map did not save")
ASSETS.save_directory("/Game/VisualLab", only_if_is_dirty=False, recursive=True)
actors = ACTORS.get_all_level_actors()
pets = [a.get_actor_label() for a in actors if a.get_actor_label().endswith("| static comparison")]
if len(pets) != 3:
    raise RuntimeError("Expected exactly Marty, Lakitu and Rio")
report = {"engine": unreal.SystemLibrary.get_engine_version(), "map_saved": True, "map": MAP, "static_pets": pets, "actor_count": len(actors), "lumen_gi": unreal.SystemLibrary.get_console_variable_int_value("r.DynamicGlobalIlluminationMethod"), "lumen_reflections": unreal.SystemLibrary.get_console_variable_int_value("r.ReflectionMethod"), "distance_fields": unreal.SystemLibrary.get_console_variable_int_value("r.GenerateMeshDistanceFields"), "scope": "Static pet and scenery graphics preview; default free-camera pawn; no Unity gameplay port"}
report["pet_materials"] = {name: [str(slot.material_interface.get_path_name()) if slot.material_interface else None for slot in ASSETS.load_asset("/Game/VisualLab/Pets/" + name).get_editor_property("static_materials")] for name in ("Marty", "Lakitu", "Rio")}
os.makedirs(os.path.join(ROOT, "Saved"), exist_ok=True)
with open(os.path.join(ROOT, "Saved", "preview-report.json"), "w") as out:
    json.dump(report, out, indent=2)
unreal.log("CHEWEENIE_UE5_PREVIEW_CREATED")
