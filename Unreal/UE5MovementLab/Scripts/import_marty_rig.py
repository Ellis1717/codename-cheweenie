"""Import the owned skinned FBX and original physically based coat materials into native lab only."""
import unreal, os, json
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
FOLDER='/Game/Adventure/Marty'
TOOLS=unreal.AssetToolsHelpers.get_asset_tools()
EDIT=unreal.MaterialEditingLibrary
LIB=unreal.EditorAssetLibrary
def texture(name,normal=False):
    t=unreal.AssetImportTask();t.filename=os.path.join(ROOT,'SourceArt','Marty',name+'.png')
    t.destination_path=FOLDER;t.destination_name=name;t.automated=True;t.save=True;t.replace_existing=True
    TOOLS.import_asset_tasks([t]);asset=LIB.load_asset(FOLDER+'/'+name);assert asset,'Missing '+name
    if normal:
        asset.set_editor_property('srgb',False);asset.set_editor_property('compression_settings',unreal.TextureCompressionSettings.TC_NORMALMAP)
        LIB.save_loaded_asset(asset)
    return asset
grain=texture('CoatGrain');normalmap=texture('CoatNormal',True)
task=unreal.AssetImportTask()
task.filename=os.path.join(ROOT,'SourceArt','Marty','Marty_CoverRig.fbx')
task.destination_path=FOLDER;task.destination_name='Marty_CoverRig'
task.automated=True;task.save=True;task.replace_existing=True
options=unreal.FbxImportUI();options.import_mesh=True;options.import_as_skeletal=True
options.automated_import_should_detect_type=False;options.mesh_type_to_import=unreal.FBXImportType.FBXIT_SKELETAL_MESH
options.import_materials=False;options.import_textures=False;options.import_animations=False
options.skeletal_mesh_import_data.set_editor_property('convert_scene',True)
options.skeletal_mesh_import_data.set_editor_property('force_front_x_axis',False)
options.skeletal_mesh_import_data.set_editor_property('import_uniform_scale',1.0)
task.options=options;TOOLS.import_asset_tasks([task])
pet=LIB.load_asset(FOLDER+'/Marty_CoverRig')
assert isinstance(pet,unreal.SkeletalMesh),'Skinned mesh not imported'

def material(name,color,roughness,fur=False):
    path=FOLDER+'/'+name
    # Preserve existing material edits. UE5.8 can root expressions referenced by the native CDO;
    # deleting those expressions in a rendered import asserts inside the engine.
    if LIB.does_asset_exist(path):return LIB.load_asset(path)
    m=TOOLS.create_asset(name,FOLDER,unreal.Material,unreal.MaterialFactoryNew())
    EDIT.set_material_usage(m,unreal.MaterialUsage.MATUSAGE_SKELETAL_MESH)
    def node(kind):return EDIT.create_material_expression(m,kind)
    def scalar(v):
        n=node(unreal.MaterialExpressionConstant);n.set_editor_property('r',v);return n
    def color_node(rgb):
        n=node(unreal.MaterialExpressionConstant3Vector);n.set_editor_property('constant',unreal.LinearColor(*rgb,1));return n
    base=color_node(color)
    if fur:
        # Rest-space grain stays attached to the moving skin rather than swimming in world space.
        coords=node(unreal.MaterialExpressionTextureCoordinate);coords.set_editor_property('u_tiling',10.0);coords.set_editor_property('v_tiling',8.0)
        noise=node(unreal.MaterialExpressionTextureSample);noise.set_editor_property('texture',grain)
        EDIT.connect_material_expressions(coords,'',noise,'Coordinates')
        lerp=node(unreal.MaterialExpressionLinearInterpolate)
        EDIT.connect_material_expressions(color_node(tuple(c*.82 for c in color)),'',lerp,'A')
        EDIT.connect_material_expressions(base,'',lerp,'B');EDIT.connect_material_expressions(noise,'R',lerp,'Alpha');base=lerp
        normal=node(unreal.MaterialExpressionTextureSample);normal.set_editor_property('texture',normalmap)
        normal.set_editor_property('sampler_type',unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
        EDIT.connect_material_expressions(coords,'',normal,'Coordinates')
        EDIT.connect_material_property(normal,'RGB',unreal.MaterialProperty.MP_NORMAL)
    EDIT.connect_material_property(base,'',unreal.MaterialProperty.MP_BASE_COLOR)
    EDIT.connect_material_property(scalar(roughness),'',unreal.MaterialProperty.MP_ROUGHNESS)
    EDIT.connect_material_property(scalar(.38 if fur else .5),'',unreal.MaterialProperty.MP_SPECULAR)
    EDIT.recompile_material(m);LIB.save_loaded_asset(m,only_if_is_dirty=False);return m

spec={'Marty_Coat':((.46,.205,.065),.73,True),'Marty_Cream':((.64,.40,.20),.8,True),
      'Marty_Ears':((.24,.075,.022),.78,True),'Marty_Iris':((.12,.045,.012),.15,False),
      'Marty_Nose':((.012,.008,.006),.28,False),'Marty_Pupil':((.002,.002,.002),.12,False)}
slots=pet.get_editor_property('materials')
for i,s in enumerate(slots):
    key=str(s.material_slot_name)
    assert key in spec,'Unexpected material '+key
    rgb,r,f=spec[key];s.material_interface=material(key,rgb,r,f);slots[i]=s
pet.set_editor_property('materials',slots);LIB.save_loaded_asset(pet,only_if_is_dirty=False)
report={'asset':pet.get_path_name(),'materials':len(slots),'material_paths':[s.material_interface.get_path_name() if s.material_interface else None for s in pet.get_editor_property('materials')],
        'skeletal_usage':[EDIT.has_material_usage(s.material_interface,unreal.MaterialUsage.MATUSAGE_SKELETAL_MESH) for s in slots],
        'imported':list(task.get_editor_property('imported_object_paths'))}
with open(os.path.join(ROOT,'Saved','marty-import.json'),'w') as f:json.dump(report,f,indent=2)
unreal.log('MARTY_SKIN_IMPORT_PASSED')
world_path='/Game/Adventure/World/NeighbourhoodSurface'
if not LIB.does_asset_exist(world_path):
    m=TOOLS.create_asset('NeighbourhoodSurface','/Game/Adventure/World',unreal.Material,unreal.MaterialFactoryNew())
    c=EDIT.create_material_expression(m,unreal.MaterialExpressionVectorParameter);c.set_editor_property('parameter_name','Color')
    c.set_editor_property('default_value',unreal.LinearColor(.2,.4,.1,1))
    EDIT.connect_material_property(c,'',unreal.MaterialProperty.MP_BASE_COLOR)
    r=EDIT.create_material_expression(m,unreal.MaterialExpressionConstant);r.set_editor_property('r',.82)
    EDIT.connect_material_property(r,'',unreal.MaterialProperty.MP_ROUGHNESS)
    EDIT.set_material_usage(m,unreal.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES)
    EDIT.recompile_material(m);LIB.save_loaded_asset(m,only_if_is_dirty=False)
