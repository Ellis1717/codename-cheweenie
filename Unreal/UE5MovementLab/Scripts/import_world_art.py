"""Import original PBR scenery into Adventure only; comparison assets stay intact."""
import unreal, pathlib, json, hashlib
ROOT=pathlib.Path(__file__).resolve().parents[1];SOURCE=ROOT/'SourceArt'/'WorldRealism'
FOLDER='/Game/Adventure/Scenery';LIB=unreal.EditorAssetLibrary;EDIT=unreal.MaterialEditingLibrary;TOOLS=unreal.AssetToolsHelpers.get_asset_tools()
manifest=json.loads((SOURCE/'manifest.json').read_text());materials={};textures={}

def texture(name,role):
    task=unreal.AssetImportTask();task.filename=str(SOURCE/(name+'_'+role+'.png'));task.destination_path=FOLDER+'/Textures';task.destination_name=name+'_'+role
    task.automated=True;task.save=True;task.replace_existing=True;TOOLS.import_asset_tasks([task])
    t=LIB.load_asset(task.destination_path+'/'+task.destination_name);assert t,'Missing texture '+name+' '+role
    t.set_editor_property('srgb',role=='Base')
    if role=='Normal':t.set_editor_property('compression_settings',unreal.TextureCompressionSettings.TC_NORMALMAP)
    if role=='Rough':t.set_editor_property('compression_settings',unreal.TextureCompressionSettings.TC_GRAYSCALE)
    LIB.set_metadata_tag(t,'WorldSourceSha256',hashlib.sha256((SOURCE/(name+'_'+role+'.png')).read_bytes()).hexdigest())
    LIB.save_loaded_asset(t);textures[name+'_'+role]=[t.blueprint_get_size_x(),t.blueprint_get_size_y()];return t

def material(name,spec,world=False):
    label=name+('_World' if world else '')
    old=LIB.load_asset(FOLDER+'/Materials/'+label) if LIB.does_asset_exist(FOLDER+'/Materials/'+label) else None
    # Reproducible new named assets; never delete expression graphs rooted by a live native CDO.
    if old and LIB.get_metadata_tag(old,'WorldArtVersion')=='v2':return old
    m=old or TOOLS.create_asset(label,FOLDER+'/Materials',unreal.Material,unreal.MaterialFactoryNew())
    if old:EDIT.delete_all_material_expressions(m)
    EDIT.set_material_usage(m,unreal.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES)
    m.set_editor_property('two_sided',name in ('Leaf','IvoryLinen','Grass'))
    def node(cls):return EDIT.create_material_expression(m,cls)
    def wire(a,out,b,input):assert EDIT.connect_material_expressions(a,out,b,input)
    def plug(a,out,p):assert EDIT.connect_material_property(a,out,p)
    def scalar(v):
        s=node(unreal.MaterialExpressionConstant);s.set_editor_property('r',v);return s
    coords=node(unreal.MaterialExpressionTextureCoordinate)
    if world:
        p=node(unreal.MaterialExpressionWorldPosition);n=node(unreal.MaterialExpressionVertexNormalWS)
        uv=node(unreal.MaterialExpressionCustom);uv.set_editor_property('code','float3 a=abs(N); return (a.z>0.7 ? P.xy : (a.x>a.y ? P.yz : P.xz)) / '+str(float(spec['tile_cm']))+';')
        uv.set_editor_property('output_type',unreal.CustomMaterialOutputType.CMOT_FLOAT2)
        inputs=[]
        for label in ('P','N'):
            value=unreal.CustomInput();value.set_editor_property('input_name',label);inputs.append(value)
        uv.set_editor_property('inputs',inputs);wire(p,'',uv,'P');wire(n,'',uv,'N');coords=uv
    if spec['maps']:
        for role,prop in [('Base',unreal.MaterialProperty.MP_BASE_COLOR),('Rough',unreal.MaterialProperty.MP_ROUGHNESS),('Normal',unreal.MaterialProperty.MP_NORMAL)]:
            path=FOLDER+'/Textures/'+name+'_'+role
            t=LIB.load_asset(path) if LIB.does_asset_exist(path) else None
            if not t:t=texture(name,role)
            sample=node(unreal.MaterialExpressionTextureSample);sample.set_editor_property('texture',t)
            if role=='Normal':sample.set_editor_property('sampler_type',unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
            elif role=='Rough':sample.set_editor_property('sampler_type',unreal.MaterialSamplerType.SAMPLERTYPE_LINEAR_GRAYSCALE)
            wire(coords,'',sample,'UVs')
            if role=='Normal':
                amount=.25 if name in ('SofaLinen','RugWool','IvoryLinen','OchreLinen') else .5
                scale=node(unreal.MaterialExpressionConstant3Vector);scale.set_editor_property('constant',unreal.LinearColor(amount,amount,1,1))
                mult=node(unreal.MaterialExpressionMultiply);wire(sample,'RGB',mult,'A');wire(scale,'',mult,'B');unit=node(unreal.MaterialExpressionNormalize);wire(mult,'',unit,'');plug(unit,'',prop)
            else:plug(sample,'R' if role=='Rough' else 'RGB',prop)
    else:
        c=node(unreal.MaterialExpressionConstant3Vector);c.set_editor_property('constant',unreal.LinearColor(*spec['color'],1));plug(c,'',unreal.MaterialProperty.MP_BASE_COLOR)
        plug(scalar(spec['roughness']),'',unreal.MaterialProperty.MP_ROUGHNESS)
    plug(scalar(1 if spec['kind']=='metal' else 0),'',unreal.MaterialProperty.MP_METALLIC)
    LIB.set_metadata_tag(m,'WorldArtVersion','v2');EDIT.recompile_material(m);LIB.save_loaded_asset(m);return m

for name,spec in manifest['materials'].items():
    if spec['maps']:
        for role in ('Base','Normal','Rough'):
            path=FOLDER+'/Textures/'+name+'_'+role
            if LIB.does_asset_exist(path):
                old=LIB.load_asset(path);sha=hashlib.sha256((SOURCE/(name+'_'+role+'.png')).read_bytes()).hexdigest()
                if LIB.get_metadata_tag(old,'WorldSourceSha256')!=sha:texture(name,role)
    materials[name]=material(name,spec)
for name in ('Grass','Brick','Concrete','Asphalt','RoofSlate','Bark','Paint','BluePaint'):
    material(name,manifest['materials'][name],world=True)
for name,spec in manifest['materials'].items():
    if spec['maps']:
        for role in ('Base','Normal','Rough'):
            t=LIB.load_asset(FOLDER+'/Textures/'+name+'_'+role);assert t
            textures[name+'_'+role]=[t.blueprint_get_size_x(),t.blueprint_get_size_y()]
report={'materials':list(materials),'textures':textures,'meshes':{}}
for name,spec in manifest['meshes'].items():
    task=unreal.AssetImportTask();task.filename=str(SOURCE/(name+'.fbx'));task.destination_path=FOLDER+'/Meshes';task.destination_name=name
    task.automated=True;task.save=True;task.replace_existing=True
    options=unreal.FbxImportUI();options.import_mesh=True;options.import_as_skeletal=False;options.automated_import_should_detect_type=False
    options.mesh_type_to_import=unreal.FBXImportType.FBXIT_STATIC_MESH;options.import_materials=False;options.import_textures=False
    options.static_mesh_import_data.set_editor_property('combine_meshes',True);options.static_mesh_import_data.set_editor_property('auto_generate_collision',False)
    options.static_mesh_import_data.set_editor_property('convert_scene',True);options.static_mesh_import_data.set_editor_property('force_front_x_axis',False)
    options.static_mesh_import_data.set_editor_property('normal_import_method',unreal.FBXNormalImportMethod.FBXNIM_IMPORT_NORMALS)
    task.options=options;task.factory=unreal.FbxFactory();TOOLS.import_asset_tasks([task])
    mesh=LIB.load_asset(FOLDER+'/Meshes/'+name);assert isinstance(mesh,unreal.StaticMesh),name
    for i,slot in enumerate(mesh.get_editor_property('static_materials')):
        key=str(slot.material_slot_name).split('_')[0];assert key in materials,'Unexpected slot '+str(slot.material_slot_name)
        mesh.set_material(i,materials[key])
    LIB.save_loaded_asset(mesh)
    bounds=mesh.get_bounds();report['meshes'][name]={'origin':[bounds.origin.x,bounds.origin.y,bounds.origin.z],'extent':[bounds.box_extent.x,bounds.box_extent.y,bounds.box_extent.z],'slots':len(mesh.get_editor_property('static_materials'))}
LIB.save_directory(FOLDER,only_if_is_dirty=False,recursive=True)
(ROOT/'Saved'/'world-art-import.json').write_text(json.dumps(report,indent=2))
unreal.log('CHEWEENIE_WORLD_ART_IMPORTED')
