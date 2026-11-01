"""Import a separate combed coat and fibre materials; retain every older asset."""
import pathlib, json
script=pathlib.Path(__file__).resolve()
base=(script.parent/'import_marty_connected.py').read_text()
old="[('MartyConnected','Marty_CoverRig','Marty'),('MartyConnectedRealism','Marty_RealCoat','MartyRealism')]"
assert old in base
base=base.replace(old,"[('MartySoftCoat','Marty_RealCoat','MartyRealism')]")
base=base.replace("'marty-connected-import.json'","'marty-softcoat-mesh-import.json'")
base=base.replace('options.skeletal_mesh_import_data.import_uniform_scale=1',
    'options.skeletal_mesh_import_data.import_uniform_scale=1\n    options.skeletal_mesh_import_data.normal_import_method=unreal.FBXNormalImportMethod.FBXNIM_IMPORT_NORMALS')
exec(compile(base,str(script),'exec'),globals())
FOLDER='/Game/Adventure/MartySoftCoat'
EDIT=unreal.MaterialEditingLibrary
grain=LIB.load_asset('/Game/Adventure/MartyRealism/CoatGrain');assert grain
normalmap=LIB.load_asset('/Game/Adventure/MartyRealism/CoatNormal');assert normalmap

# Reuse the proven UV palette decoder; FBX's green-channel flip is intentional.
existing=(script.parent/'import_marty_realcoat.py').read_text()
material_source=existing[existing.index('\ndef material(')+1:existing.index('\nspec=')]
material_source=material_source.replace('uv-rgb-v3','softcoat-fuzz-v1')
# CustomData0 is hidden from UE5.8's Python enum. MakeMaterialAttributes exposes
# the same native input as ClearCoat; on the cloth BRDF this is the fuzz weight.
start=material_source.index('    def plug(');end=material_source.index('    def scalar(')
material_source=material_source[:start]+"""    attributes=node(unreal.MaterialExpressionMakeMaterialAttributes)
    m.set_editor_property('use_material_attributes',True)
    def plug(a,output,prop):
        names={unreal.MaterialProperty.MP_BASE_COLOR:'BaseColor',unreal.MaterialProperty.MP_NORMAL:'Normal',
               unreal.MaterialProperty.MP_ROUGHNESS:'Roughness',unreal.MaterialProperty.MP_SPECULAR:'Specular',
               unreal.MaterialProperty.MP_SUBSURFACE_COLOR:'SubsurfaceColor'}
        wire(a,output,attributes,names[prop])
"""+material_source[end:]
material_source=material_source.replace('    EDIT.recompile_material(m);',
    "    assert EDIT.connect_material_property(attributes,'',unreal.MaterialProperty.MP_MATERIAL_ATTRIBUTES)\n    EDIT.recompile_material(m);")
material_source=material_source.replace("'u_tiling',4.0","'u_tiling',6.0").replace("'v_tiling',4.0","'v_tiling',6.0")
material_source=material_source.replace('wire(scalar(.20)','wire(scalar(.08)').replace('wire(scalar(.85)','wire(scalar(.96)')
old="wire(coords,'',normal,'');plug(normal,'',unreal.MaterialProperty.MP_NORMAL)"
assert old in material_source
material_source=material_source.replace(old,"""wire(coords,'',normal,'')
        softened=node(unreal.MaterialExpressionLinearInterpolate)
        wire(rgb((0,0,1)),'',softened,'A');wire(normal,'',softened,'B');wire(scalar(.20),'',softened,'Alpha')
        unit=node(unreal.MaterialExpressionNormalize);wire(softened,'',unit,'');plug(unit,'',unreal.MaterialProperty.MP_NORMAL)""")
material_source=material_source.replace('unreal.MaterialShadingModel.MSM_HAIR','unreal.MaterialShadingModel.MSM_CLOTH')
material_source=material_source.replace("        tangent=node(unreal.MaterialExpressionVertexTangentWS);plug(tangent,'',unreal.MaterialProperty.MP_TANGENT)\n",'')
old="    plug(base,'',unreal.MaterialProperty.MP_BASE_COLOR)"
assert old in material_source
material_source=material_source.replace(old,"""    if fur:
        # UE's cloth BRDF supplies physical microfibre fuzz, without emissive glow
        # or any cloth simulation. Fine strands retain their smooth surface normals.
        m.set_editor_property('shading_model',unreal.MaterialShadingModel.MSM_CLOTH)
        fuzz=node(unreal.MaterialExpressionMultiply);wire(base,'',fuzz,'A');wire(scalar(.28),'',fuzz,'B')
        plug(fuzz,'',unreal.MaterialProperty.MP_SUBSURFACE_COLOR)
        wire(scalar(.35 if name=='Marty_FineFur' else .18),'',attributes,'ClearCoat')
    plug(base,'',unreal.MaterialProperty.MP_BASE_COLOR)""")
exec(compile(material_source,str(script),'exec'),globals())
spec={'Marty_Coat':((.58,.31,.105),.74),'Marty_Cream':((.74,.59,.37),.78),
      'Marty_Ears':((.30,.125,.042),.76),'Marty_FineFur':((.58,.31,.105),.72)}
slots=pet.get_editor_property('materials')
for i,slot in enumerate(slots):
    name=str(slot.material_slot_name)
    if name in spec:
        color,roughness=spec[name];slot.material_interface=material(name,color,roughness,True);slots[i]=slot
pet.set_editor_property('materials',slots)
assert LIB.save_loaded_asset(pet,only_if_is_dirty=False)
models={name:str(LIB.load_asset(FOLDER+'/'+name).get_editor_property('shading_model')) for name in spec}
assert all(LIB.load_asset(FOLDER+'/'+name).get_editor_property('shading_model')==unreal.MaterialShadingModel.MSM_CLOTH for name in spec)
report={'ready':True,'import':reports,'fur_strands':32000,'maximum_authored_strand_length_cm':.28,
        'material_models':models,'coat_shading':'Cloth microfibre BRDF, not cloth simulation or Groom',
        'materials':[s.material_interface.get_path_name() for s in slots],
        'old_assets_preserved':True,'original_texture_assets_reused':True}
with open(os.path.join(ROOT,'Saved','marty-softcoat-import.json'),'w') as f:json.dump(report,f,indent=2)
print('MARTY_SOFTCOAT_READY')
