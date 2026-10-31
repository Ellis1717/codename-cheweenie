"""Import only the original Ratchet static tribute; do not modify existing pets."""
import unreal, os, json
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)));FOLDER='/Game/Adventure/Ratchet'
LIB=unreal.EditorAssetLibrary;TOOLS=unreal.AssetToolsHelpers.get_asset_tools();EDIT=unreal.MaterialEditingLibrary
materials={}
for name,rgb,rough in [('Black',(.018,.014,.012),.78),('White',(.74,.72,.68),.78),('Nose',(.03,.018,.02),.4),('Iris',(.26,.29,.12),.32),('Eye',(.008,.009,.009),.15),('InnerEar',(.10,.065,.06),.8)]:
    label='Ratchet_'+name;path=FOLDER+'/'+label
    if LIB.does_asset_exist(path):
        materials[label]=LIB.load_asset(path);continue
    m=TOOLS.create_asset(label,FOLDER,unreal.Material,unreal.MaterialFactoryNew())
    EDIT.delete_all_material_expressions(m)
    c=EDIT.create_material_expression(m,unreal.MaterialExpressionConstant3Vector);c.set_editor_property('constant',unreal.LinearColor(*rgb,1))
    EDIT.connect_material_property(c,'',unreal.MaterialProperty.MP_BASE_COLOR)
    v=EDIT.create_material_expression(m,unreal.MaterialExpressionConstant);v.set_editor_property('r',rough);EDIT.connect_material_property(v,'',unreal.MaterialProperty.MP_ROUGHNESS)
    EDIT.recompile_material(m);LIB.save_loaded_asset(m);materials[label]=m
options=unreal.FbxImportUI();options.import_mesh=True;options.import_as_skeletal=False;options.automated_import_should_detect_type=False;options.mesh_type_to_import=unreal.FBXImportType.FBXIT_STATIC_MESH
options.import_materials=False;options.import_textures=False;options.static_mesh_import_data.combine_meshes=True;options.static_mesh_import_data.auto_generate_collision=False
options.static_mesh_import_data.convert_scene=True;options.static_mesh_import_data.force_front_x_axis=False
job=unreal.AssetImportTask();job.filename=os.path.join(ROOT,'SourceArt','Ratchet','Ratchet_Tribute.fbx');job.destination_path=FOLDER;job.destination_name='Ratchet_Tribute';job.automated=True;job.save=True;job.replace_existing=True;job.options=options
TOOLS.import_asset_tasks([job]);mesh=LIB.load_asset(FOLDER+'/Ratchet_Tribute');assert mesh
for i,slot in enumerate(mesh.static_materials):
    key=str(slot.material_slot_name);assert key in materials,key;mesh.set_material(i,materials[key])
LIB.save_loaded_asset(mesh)
bounds=mesh.get_bounding_box();size=bounds.max-bounds.min;assert 45<size.x<100 and 25<size.z<65,str(size)
with open(os.path.join(ROOT,'Saved','ratchet-import.json'),'w') as f:json.dump({'asset':mesh.get_path_name(),'materials':len(mesh.static_materials),'size_cm':[size.x,size.y,size.z],'scope':'Original static tribute; inferred standing form and iris hue; feline rig remains future work'},f,indent=2)
print('RATCHET_IMPORT_READY')
