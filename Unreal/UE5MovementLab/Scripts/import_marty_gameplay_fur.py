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
revision = 'gameplay-fur-' + hashlib.sha256(FIBRE_CODE.encode()).hexdigest()[:12]

# Reuse the established palette decoder, cloth attributes and normal-map wiring.
# Execute only the material builder, never the older FBX import or slot assignment.
source = (script.parent/'import_marty_softcoat.py').read_text()
builder = source[source.index("FOLDER="):source.index("FIBRE_CODE=r'''")]
builder = builder.replace('MartySoftCoat/DirectionalFibres', 'MartyPhotoReference/GameplayFibres/'+revision)
builder = builder.replace('softcoat-fuzz-v4', revision)
exec(compile(builder, str(script), 'exec'), globals())
# Preserve the UV-encoded regional markings, including the darker ear roots.
# A restrained warm tint follows the owner's indoor close-up, not a colour scan.
old = "    plug(base,'',unreal.MaterialProperty.MP_BASE_COLOR)"
assert material_source.count(old) == 1
material_source = material_source.replace(old, """    if fur:
        # Decode the existing brown-to-tan transition in every slot, including
        # geometric hairs. Slot-based tinting would expose the welded ear seam.
        warm=node(unreal.MaterialExpressionCustom)
        color_input=unreal.CustomInput();color_input.set_editor_property('input_name','Coat')
        warm.set_editor_property('inputs',[color_input])
        warm.set_editor_property('output_type',unreal.CustomMaterialOutputType.CMOT_FLOAT3)
        warm.set_editor_property('code','float brown=1.0-smoothstep(0.43,0.53,Coat.g/max(Coat.r,0.001)); return Coat*lerp(float3(1.035,1.015,0.96),float3(0.86,0.74,0.66),brown);')
        wire(base,'',warm,'Coat');base=warm
    plug(base,'',unreal.MaterialProperty.MP_BASE_COLOR)""")
material_source = material_source.replace(".35 if name=='Marty_FineFur' else .18",
                                          ".20 if name=='Marty_FineFur' else .10")
exec(compile(material_source, str(script), 'exec'), globals())

spec = {'Marty_Coat': ((.58,.31,.105),.64), 'Marty_Cream': ((.74,.59,.37),.70),
        'Marty_Ears': ((.30,.125,.042),.66), 'Marty_FineFur': ((.58,.31,.105),.64)}
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
# Low uses the same close-lying surface shading without geometric hair.
# Do not reimport geometry, skeletons, eyes, collar or tags.
smooth = LIB.load_asset('/Game/Adventure/MartyPhotoSmooth/Marty_RealCoat')
assert smooth, 'Import the retained Low photo model first'
shared = {str(slot.material_slot_name): slot.material_interface for slot in slots
          if str(slot.material_slot_name) in spec}
smooth_slots = smooth.get_editor_property('materials')
smooth_updated = []
for i, slot in enumerate(smooth_slots):
    name = str(slot.material_slot_name)
    if name in shared:
        slot.material_interface = shared[name]
        smooth_slots[i] = slot
        smooth_updated.append(name)
assert set(smooth_updated) == set(spec) - {'Marty_FineFur'}, smooth_updated
smooth.set_editor_property('materials', smooth_slots)
assert LIB.save_loaded_asset(smooth, only_if_is_dirty=False)
report = {'ready': True, 'materials': updated, 'bands_uv': [[220,820],[64,220],[14,40]],
          'photo_profile': 'owner-closeup-2026-11-02-v3',
          'reference_policy': 'Indoor photo interpretation; no calibrated colour/length measurements',
          'low_coat_materials_updated': smooth_updated,
          'private_photo_embedded': False,
          'shader_revision': revision,
          'filter': 'Each band smoothly fades over 0.35–1.15 UV cells per pixel',
          'mesh_reimported': False, 'earlier_materials_preserved': True,
          'scope': 'Material import only; rendered appearance and gameplay require runtime checks'}
(ROOT/'Saved'/'marty-gameplay-fur-import.json').write_text(json.dumps(report, indent=2))
print('MARTY_GAMEPLAY_FUR_READY')

# Keep the optional higher-density model on the same immutable material revision.
link_script=pathlib.Path(__file__).with_name('link_marty_strand_materials.py')
if link_script.exists() and LIB.does_asset_exist('/Game/Adventure/MartyVisibleStrands/Marty_RealCoat'):
    exec(compile(link_script.read_text(),str(link_script),'exec'),globals())
