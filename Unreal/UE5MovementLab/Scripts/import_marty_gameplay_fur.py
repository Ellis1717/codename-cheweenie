"""Add distance-readable coat materials to the current mesh; retain old assets."""
import json
import hashlib
import pathlib
import unreal

script = pathlib.Path(__file__).resolve()
ROOT = script.parent.parent
LIB = unreal.EditorAssetLibrary
TOOLS = unreal.AssetToolsHelpers.get_asset_tools()
pet = LIB.load_asset('/Game/Adventure/MartyPhotoReference/Marty_RealCoat')
assert pet, 'Import the photo-guided model first'
FIBRE_CODE = (script.parent/'shaders'/'marty_gameplay_fur.ush').read_text()

# Reuse the established palette decoder, cloth attributes and normal-map wiring.
# Execute only the material builder, never the older FBX import or slot assignment.
source = (script.parent/'import_marty_softcoat.py').read_text()
builder = source[source.index("FOLDER="):source.index("FIBRE_CODE=r'''")]
builder = builder.replace('MartySoftCoat/DirectionalFibres', 'MartyPhotoReference/GameplayFibres')
revision = 'gameplay-fur-' + hashlib.sha256(FIBRE_CODE.encode()).hexdigest()[:12]
builder = builder.replace('softcoat-fuzz-v4', revision)
exec(compile(builder, str(script), 'exec'), globals())
material_source = material_source.replace('    EDIT.set_material_usage(m,',
    '    EDIT.delete_all_material_expressions(m)\n    EDIT.set_material_usage(m,')
exec(compile(material_source, str(script), 'exec'), globals())

spec = {'Marty_Coat': ((.58,.31,.105),.74), 'Marty_Cream': ((.74,.59,.37),.78),
        'Marty_Ears': ((.30,.125,.042),.76), 'Marty_FineFur': ((.58,.31,.105),.72)}
slots = pet.get_editor_property('materials')
updated = []
for i, slot in enumerate(slots):
    name = str(slot.material_slot_name)
    if name in spec:
        color, roughness = spec[name]
        slot.material_interface = material(name, color, roughness, True)
        slots[i] = slot
        updated.append(slot.material_interface.get_path_name())
assert len(updated) == 4
pet.set_editor_property('materials', slots)
assert LIB.save_loaded_asset(pet, only_if_is_dirty=False)
report = {'ready': True, 'materials': updated, 'bands_uv': [[180,720],[46,156],[14,48]],
          'shader_revision': revision,
          'filter': 'Each band smoothly fades over 0.35–1.15 UV cells per pixel',
          'mesh_reimported': False, 'earlier_materials_preserved': True,
          'scope': 'Material import only; rendered appearance and gameplay require runtime checks'}
(ROOT/'Saved'/'marty-gameplay-fur-import.json').write_text(json.dumps(report, indent=2))
print('MARTY_GAMEPLAY_FUR_READY')
