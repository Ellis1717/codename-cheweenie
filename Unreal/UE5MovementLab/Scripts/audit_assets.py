"""Run in the content-only UE5VisualLab before the native movement module is built.

Only reads the comparison map and retained assets; does not save or alter them.
This is an asset audit, not a compilation/gameplay test.
"""
import json
import os
import unreal

root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
level = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
assert level.load_level('/Game/VisualLab/Comparison'), 'Comparison map missing'
report = {'engine': unreal.SystemLibrary.get_engine_version(), 'scope': 'Read-only asset audit; native gameplay unverified', 'pets': {}}
for name in ('Marty', 'Lakitu', 'Rio'):
    asset = unreal.EditorAssetLibrary.load_asset('/Game/VisualLab/Pets/' + name)
    assert asset, 'Missing pet: ' + name
    slots = asset.get_editor_property('static_materials')
    assert slots and all(s.material_interface for s in slots), 'Missing pet materials: ' + name
    found = [a for a in actors.get_all_level_actors() if a.get_actor_label() == name + ' | static comparison']
    assert len(found) == 1, 'Missing/duplicate comparison pet: ' + name
    origin, extent = found[0].get_actor_bounds(False)
    report['pets'][name] = {'material_slots': len(slots), 'size_cm': [extent.x * 2, extent.y * 2, extent.z * 2], 'bottom_cm': origin.z - extent.z}
labels = {a.get_actor_label() for a in actors.get_all_level_actors()}
assert 'Solid floor' in labels and 'Blue woven rug' in labels, 'Movement room surfaces missing'
report['comparison_actors'] = len(actors.get_all_level_actors())
mesh_editor = unreal.get_editor_subsystem(unreal.StaticMeshEditorSubsystem)
report['traversal_collision'] = {}
def enum_value(kind, name):
    # UE5.8 preserves prefixes on some reflected enums (for example ECC_PAWN).
    matches = [getattr(kind, key) for key in dir(kind) if key.upper() == name or key.upper().endswith('_' + name)]
    assert matches and all(value == matches[0] for value in matches), 'Ambiguous/missing enum member: ' + name + ' in ' + repr(dir(kind))
    return matches[0]

for label in ('Solid floor', 'Blue woven rug', 'Sofa seat', 'Back wall'):
    found = [a for a in actors.get_all_level_actors() if a.get_actor_label() == label]
    assert len(found) == 1, 'Missing/duplicate traversal surface: ' + label
    component = found[0].static_mesh_component
    count = mesh_editor.get_simple_collision_count(component.static_mesh)
    enabled = component.get_collision_enabled()
    pawn = component.get_collision_response_to_channel(enum_value(unreal.CollisionChannel, 'PAWN'))
    camera = component.get_collision_response_to_channel(enum_value(unreal.CollisionChannel, 'CAMERA'))
    assert count > 0, 'Traversal surface has no simple collision: ' + label
    assert enabled in (unreal.CollisionEnabled.QUERY_ONLY, unreal.CollisionEnabled.QUERY_AND_PHYSICS), 'Traversal queries disabled: ' + label
    # CollisionResponse is also a reflected struct name; use the returned enum's actual type.
    assert pawn == enum_value(type(pawn), 'BLOCK') and camera == enum_value(type(camera), 'BLOCK'), 'Traversal/camera collision not blocking: ' + label
    report['traversal_collision'][label] = {'simple_shapes': count, 'enabled': str(enabled), 'pawn': str(pawn), 'camera': str(camera)}
os.makedirs(os.path.join(root, 'Saved'), exist_ok=True)
with open(os.path.join(root, 'Saved', 'asset-audit.json'), 'w', encoding='utf-8') as stream:
    json.dump(report, stream, indent=2)
unreal.log('CHEWEENIE_MOVEMENT_ASSET_AUDIT_PASSED')
