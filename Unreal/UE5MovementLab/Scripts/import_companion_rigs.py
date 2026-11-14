"""Import new original rigs only. Never overwrite Marty or the retained comparison pets."""
import unreal,os,json,re
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT='/Game/Adventure/CompanionRigs';TOOLS=unreal.AssetToolsHelpers.get_asset_tools();LIB=unreal.EditorAssetLibrary;EDIT=unreal.MaterialEditingLibrary
with open(os.path.join(ROOT,'SourceArt','CompanionRigs','manifest.json')) as f:DATA=json.load(f)
report={}
def material(profile,key,color):
    name=profile+'_'+key;path=OUT+'/'+name
    if LIB.does_asset_exist(path):return LIB.load_asset(path)
    m=TOOLS.create_asset(name,OUT,unreal.Material,unreal.MaterialFactoryNew());EDIT.set_material_usage(m,unreal.MaterialUsage.MATUSAGE_SKELETAL_MESH)
    def node(t):return EDIT.create_material_expression(m,t)
    c=node(unreal.MaterialExpressionConstant3Vector);c.set_editor_property('constant',unreal.LinearColor(*color));EDIT.connect_material_property(c,'',unreal.MaterialProperty.MP_BASE_COLOR)
    rough=node(unreal.MaterialExpressionConstant);rough.set_editor_property('r',.28 if key in ['Nose','Iris','Pupil','Glint'] else .84);EDIT.connect_material_property(rough,'',unreal.MaterialProperty.MP_ROUGHNESS)
    if key in ['Coat','White','Dark']:
        normal=LIB.load_asset('/Game/Adventure/Marty/CoatNormal')
        if normal:
            uv=node(unreal.MaterialExpressionTextureCoordinate);uv.set_editor_property('u_tiling',16.0);uv.set_editor_property('v_tiling',10.0)
            tex=node(unreal.MaterialExpressionTextureSample);tex.set_editor_property('texture',normal);tex.set_editor_property('sampler_type',unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
            EDIT.connect_material_expressions(uv,'',tex,'Coordinates');EDIT.connect_material_property(tex,'RGB',unreal.MaterialProperty.MP_NORMAL)
    EDIT.recompile_material(m);LIB.save_loaded_asset(m,only_if_is_dirty=False);return m
for profile,data in DATA.items():
    task=unreal.AssetImportTask();task.filename=os.path.join(ROOT,'SourceArt','CompanionRigs',profile+'_Rig.fbx');task.destination_path=OUT;task.destination_name=profile+'_Rig';task.automated=True;task.save=True;task.replace_existing=True
    opt=unreal.FbxImportUI();opt.import_mesh=True;opt.import_as_skeletal=True;opt.automated_import_should_detect_type=False;opt.mesh_type_to_import=unreal.FBXImportType.FBXIT_SKELETAL_MESH
    opt.import_materials=False;opt.import_textures=False;opt.import_animations=False;opt.skeletal_mesh_import_data.set_editor_property('convert_scene',True);opt.skeletal_mesh_import_data.set_editor_property('force_front_x_axis',False)
    task.options=opt;TOOLS.import_asset_tasks([task]);pet=LIB.load_asset(OUT+'/'+profile+'_Rig');assert isinstance(pet,unreal.SkeletalMesh),profile
    slots=pet.get_editor_property('materials')
    for i,slot in enumerate(slots):
        key=re.sub(r'[._]\d+$','',str(slot.material_slot_name));assert key in data['materials'],str(slot.material_slot_name)
        slot.material_interface=material(profile,key,data['materials'][key]);slots[i]=slot
    pet.set_editor_property('materials',slots);LIB.save_loaded_asset(pet,only_if_is_dirty=False)
    report[profile]={'mesh':pet.get_path_name(),'materials':len(slots),'skeletalUsage':all(EDIT.has_material_usage(s.material_interface,unreal.MaterialUsage.MATUSAGE_SKELETAL_MESH) for s in slots)}
with open(os.path.join(ROOT,'Saved','companion-rigs-import.json'),'w') as f:json.dump(report,f,indent=2)
unreal.log('COMPANION_RIG_IMPORT_PASSED')
