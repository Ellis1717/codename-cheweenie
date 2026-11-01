"""Import connected-root variants into sibling folders; reuse retained materials."""
import unreal, os, json
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LIB=unreal.EditorAssetLibrary;TOOLS=unreal.AssetToolsHelpers.get_asset_tools()
reports=[]
for folder,name,previous in [('MartyConnected','Marty_CoverRig','Marty'),('MartyConnectedRealism','Marty_RealCoat','MartyRealism')]:
    with open(os.path.join(ROOT,'SourceArt',folder,'attachment-topology.json')) as f:proof=json.load(f)
    assert proof['primary_skin_components']==1 and proof['nonmanifold_edges']==0
    destination='/Game/Adventure/'+folder
    options=unreal.FbxImportUI();options.import_mesh=True;options.import_as_skeletal=True
    options.automated_import_should_detect_type=False;options.mesh_type_to_import=unreal.FBXImportType.FBXIT_SKELETAL_MESH
    options.import_materials=False;options.import_textures=False;options.import_animations=False
    options.create_physics_asset=False
    original=LIB.load_asset('/Game/Adventure/'+previous+'/'+name);assert original
    skeleton=original.get_editor_property('skeleton');assert skeleton
    options.skeleton=skeleton
    options.skeletal_mesh_import_data.convert_scene=True;options.skeletal_mesh_import_data.force_front_x_axis=False
    options.skeletal_mesh_import_data.import_uniform_scale=1
    options.skeletal_mesh_import_data.vertex_color_import_option=unreal.VertexColorImportOption.REPLACE
    task=unreal.AssetImportTask();task.filename=os.path.join(ROOT,'SourceArt',folder,name+'.fbx')
    task.destination_path=destination;task.destination_name=name;task.automated=True;task.save=True
    task.replace_existing=True;task.factory=unreal.FbxFactory();task.options=options
    TOOLS.import_asset_tasks([task]);pet=LIB.load_asset(destination+'/'+name)
    assert isinstance(pet,unreal.SkeletalMesh)
    assert pet.get_editor_property('skeleton'), 'Imported mesh must retain the supplied skeleton'
    slots=pet.get_editor_property('materials')
    for i,slot in enumerate(slots):
        material=LIB.load_asset('/Game/Adventure/'+previous+'/'+str(slot.material_slot_name));assert material,slot.material_slot_name
        slot.material_interface=material;slots[i]=slot
    pet.set_editor_property('materials',slots)
    editor=unreal.get_editor_subsystem(unreal.SkeletalMeshEditorSubsystem)
    fur_shadow_sections=[]
    for section in range(editor.get_num_sections(pet,0)):
        slot=editor.get_lod_material_slot(pet,0,section)
        if str(slots[slot].material_slot_name)=='Marty_FineFur':
            assert editor.set_section_cast_shadow(pet,0,section,False)
            fur_shadow_sections.append(section)
    LIB.set_metadata_tag(pet,'MartyAttachmentSkin','one-closed-component-v1')
    linked=pet.get_editor_property('skeleton')
    if linked.get_path_name().startswith(destination+'/'):
        assert LIB.save_loaded_asset(linked,only_if_is_dirty=False), 'New skeleton must be saved before the mesh'
        package=linked.get_path_name().split('.')[0].replace('/Game/','Content/')+'.uasset'
        assert os.path.isfile(os.path.join(ROOT,package)),package
    LIB.save_loaded_asset(pet,only_if_is_dirty=False)
    reports.append({'skeleton':pet.get_editor_property('skeleton').get_path_name(),'asset':pet.get_path_name(),'source_topology':proof,
                    'materials':[s.material_interface.get_path_name() for s in slots],
                    'fine_fur_shadow_sections_disabled':fur_shadow_sections,'original_assets_preserved':True})
with open(os.path.join(ROOT,'Saved','marty-connected-import.json'),'w') as f:json.dump(reports,f,indent=2)
print('MARTY_CONNECTED_IMPORT_READY')
