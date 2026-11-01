"""Import the photo-guided body with its own reference skeleton and face materials."""
import pathlib,json,hashlib
script=pathlib.Path(__file__).resolve()
base=(script.parent/'import_marty_connected.py').read_text()
old="[('MartyConnected','Marty_CoverRig','Marty'),('MartyConnectedRealism','Marty_RealCoat','MartyRealism')]"
assert old in base
base=base.replace(old,"[('MartyPhotoReference','Marty_RealCoat','MartyRealism')]")
base=base.replace('options.skeleton=skeleton','options.skeleton=None')
base=base.replace("'marty-connected-import.json'","'marty-likeness-mesh-import.json'")
base=base.replace('options.skeletal_mesh_import_data.import_uniform_scale=1',
    'options.skeletal_mesh_import_data.import_uniform_scale=1\n    options.skeletal_mesh_import_data.normal_import_method=unreal.FBXNormalImportMethod.FBXNIM_IMPORT_NORMALS')
exec(compile(base,str(script),'exec'),globals())
assert pet.get_editor_property('skeleton').get_path_name().startswith('/Game/Adventure/MartyPhotoReference/'), 'Do not mutate or reuse the older bind rig'
EDIT=unreal.MaterialEditingLibrary
def face_material(name,color,roughness,specular):
    folder='/Game/Adventure/MartyPhotoReference';path=folder+'/'+name
    m=LIB.load_asset(path) if LIB.does_asset_exist(path) else TOOLS.create_asset(name,folder,unreal.Material,unreal.MaterialFactoryNew())
    if LIB.get_metadata_tag(m,'MartyPhotoFace')=='v1':return m
    EDIT.set_material_usage(m,unreal.MaterialUsage.MATUSAGE_SKELETAL_MESH)
    c=EDIT.create_material_expression(m,unreal.MaterialExpressionConstant3Vector);c.set_editor_property('constant',unreal.LinearColor(*color,1))
    assert EDIT.connect_material_property(c,'',unreal.MaterialProperty.MP_BASE_COLOR)
    for value,prop in [(roughness,unreal.MaterialProperty.MP_ROUGHNESS),(specular,unreal.MaterialProperty.MP_SPECULAR)]:
        n=EDIT.create_material_expression(m,unreal.MaterialExpressionConstant);n.set_editor_property('r',value)
        assert EDIT.connect_material_property(n,'',prop)
    LIB.set_metadata_tag(m,'MartyPhotoFace','v1');EDIT.recompile_material(m);assert LIB.save_loaded_asset(m,only_if_is_dirty=False)
    return m
slots=pet.get_editor_property('materials')
for i,slot in enumerate(slots):
    name=str(slot.material_slot_name)
    if name in ('Marty_Coat','Marty_Cream','Marty_Ears','Marty_FineFur'):
        shader=(script.parent/'shaders'/'marty_gameplay_fur.ush').read_text()
        revision='gameplay-fur-'+hashlib.sha256(shader.encode()).hexdigest()[:12]
        path='/Game/Adventure/MartyPhotoReference/GameplayFibres/'+revision+'/'+name
        if not LIB.does_asset_exist(path):path='/Game/Adventure/MartySoftCoat/DirectionalFibres/'+name
        slot.material_interface=LIB.load_asset(path);assert slot.material_interface
    elif name=='Marty_Nose':slot.material_interface=face_material(name,(.065,.035,.030),.44,.35)
    elif name=='Marty_Iris':slot.material_interface=face_material(name,(.028,.018,.010),.24,.5)
    slots[i]=slot
pet.set_editor_property('materials',slots);assert LIB.save_loaded_asset(pet,only_if_is_dirty=False)
report={'ready':True,'asset':pet.get_path_name(),'skeleton':pet.get_editor_property('skeleton').get_path_name(),
        'materials':[s.material_interface.get_path_name() for s in slots],'source_topology':reports[0]['source_topology'],
        'fur_strands':32000,'reference_policy':'Photo-informed inferred proportions, no scan or measured anatomy claim',
        'private_photos_embedded':False,'earlier_assets_preserved':True}
with open(os.path.join(ROOT,'Saved','marty-likeness-import.json'),'w') as f:json.dump(report,f,indent=2)
print('MARTY_LIKENESS_READY')
