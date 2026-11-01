"""Share the current distance-filtered photo coat with the raised-strand model."""
import pathlib, json, unreal
root=pathlib.Path(__file__).resolve().parent.parent
lib=unreal.EditorAssetLibrary
reference=lib.load_asset('/Game/Adventure/MartyPhotoReference/Marty_RealCoat')
pet=lib.load_asset('/Game/Adventure/MartyVisibleStrands/Marty_RealCoat')
assert reference and pet,'Import both retained photo and raised-hair models first'
names={'Marty_Coat','Marty_Cream','Marty_Ears','Marty_FineFur'}
shared={str(slot.material_slot_name):slot.material_interface for slot in reference.get_editor_property('materials') if str(slot.material_slot_name) in names}
assert set(shared)==names
for material in shared.values():
    assert material and '/MartyPhotoReference/GameplayFibres/' in material.get_path_name(),'Import the distance-filtered materials first'
slots=pet.get_editor_property('materials')
linked=[]
for i,slot in enumerate(slots):
    if str(slot.material_slot_name) in names:
        slot.material_interface=shared[str(slot.material_slot_name)];slots[i]=slot
        linked.append(slot.material_interface.get_path_name())
assert len(linked)==4
pet.set_editor_property('materials',slots)
assert lib.save_loaded_asset(pet,only_if_is_dirty=False)
revisions={path.split('/GameplayFibres/')[1].split('/')[0] for path in linked}
assert len(revisions)==1
link_report={'ready':True,'fur_strands':64000,'asset':pet.get_path_name(),
             'materials':linked,'shader_revision':next(iter(revisions)),
             'reference_asset':reference.get_path_name(),'geometry_reimported':False,
             'earlier_materials_preserved':True}
(root/'Saved'/'marty-strand-material-link.json').write_text(json.dumps(link_report,indent=2))
print('MARTY_STRAND_MATERIAL_LINK_READY')
