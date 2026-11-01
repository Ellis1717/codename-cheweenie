"""Remove only geometric hairs from the photo model for Low detail."""
import bpy,bmesh,pathlib,json,hashlib,shutil
root=pathlib.Path(__file__).resolve().parent.parent
source=root/'SourceArt'/'MartyPhotoReference';out=root/'SourceArt'/'MartyPhotoSmooth'
out.mkdir(parents=True,exist_ok=True)
bpy.ops.wm.open_mainfile(filepath=str(source/'Marty_RealCoat.blend'))
pet=bpy.data.objects['Marty_RealCoat'];rig=pet.find_armature();mesh=pet.data
fur={i for i,m in enumerate(mesh.materials) if m.name=='Marty_FineFur'}
normals={}
for p in mesh.polygons:
    if p.material_index not in fur:
        signature=tuple(sorted(p.vertices))
        for li in p.loop_indices:normals[(signature,mesh.loops[li].vertex_index)]=tuple(mesh.corner_normals[li].vector)
index=mesh.attributes.new(name='OriginalVertex',type='INT',domain='POINT')
for i,value in enumerate(index.data):value.value=i
bm=bmesh.new();bm.from_mesh(mesh)
remove={v for face in bm.faces if face.material_index in fur for v in face.verts}
assert len(remove)==128000
bmesh.ops.delete(bm,geom=list(remove),context='VERTS');bm.to_mesh(mesh);bm.free();mesh.update()
original=[v.value for v in mesh.attributes['OriginalVertex'].data]
values=[]
for p in mesh.polygons:
    signature=tuple(sorted(original[v] for v in p.vertices))
    for li in p.loop_indices:values.append(normals[(signature,original[mesh.loops[li].vertex_index])])
mesh.attributes.remove(mesh.attributes['OriginalVertex']);mesh.normals_split_custom_set(values)
assert len(mesh.vertices)==79678
for filename in ('CoatGrain.png','CoatNormal.png','attachment-topology.json'):shutil.copyfile(source/filename,out/filename)
manifest=json.loads((source/'manifest.json').read_text());manifest.update(vertices=len(mesh.vertices),fur_hairs=0,
    technique='Retained photo-guided skin and accessories without geometric hairs',
    reference_rig_and_shape_unchanged=True,source_script_sha256=hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest())
(out/'manifest.json').write_text(json.dumps(manifest,indent=2))
bpy.ops.object.select_all(action='DESELECT');pet.select_set(True);rig.select_set(True);bpy.context.view_layer.objects.active=rig
bpy.ops.wm.save_as_mainfile(filepath=str(out/'Marty_RealCoat.blend'))
bpy.ops.export_scene.fbx(filepath=str(out/'Marty_RealCoat.fbx'),use_selection=True,object_types={'ARMATURE','MESH'},
    add_leaf_bones=False,bake_anim=False,axis_forward='-Y',axis_up='Z',apply_unit_scale=True,use_mesh_modifiers=True)
print('MARTY_PHOTO_SMOOTH_EXPORTED',len(mesh.vertices))
