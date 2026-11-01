"""Verify Low detail removes hair without changing Marty's anatomy or rig."""
import bpy,bmesh,pathlib,json,hashlib
root=pathlib.Path(__file__).resolve().parent.parent
folder=root/'SourceArt'/'MartyPhotoSmooth';baseline=root/'SourceArt'/'MartyPhotoReference'
bpy.ops.wm.open_mainfile(filepath=str(folder/'Marty_RealCoat.blend'))
pet=bpy.data.objects['Marty_RealCoat'];rig=pet.find_armature();assert rig
with bpy.data.libraries.load(str(baseline/'Marty_RealCoat.blend'),link=False) as (source,target):
    target.objects=['Marty_RealCoat','Marty_Anatomy']
previous,previous_rig=target.objects
assert len(pet.data.vertices)==79678 and pet.data.has_custom_normals
assert set(rig.data.bones.keys())==set(previous_rig.data.bones.keys())
difference=0
for bone in rig.data.bones:
    other=previous_rig.data.bones[bone.name]
    assert (bone.parent.name if bone.parent else None)==(other.parent.name if other.parent else None)
    difference=max(difference,max(abs(bone.matrix_local[i][j]-other.matrix_local[i][j]) for i in range(4) for j in range(4)))
assert difference<.00001
proof=json.loads((folder/'attachment-topology.json').read_text());count=proof['primary_skin_vertices']
skin_difference=max((pet.data.vertices[i].co-previous.data.vertices[i].co).length for i in range(count))
assert skin_difference<.00001
assert not any(pet.data.materials[p.material_index].name=='Marty_FineFur' for p in pet.data.polygons)
error=max(abs(sum(g.weight for g in v.groups)-1) for v in pet.data.vertices);assert error<.0001
bm=bmesh.new();bm.from_mesh(pet.data)
assert all(e.is_manifold for e in bm.edges if all(v.index<count for v in e.verts));bm.free()
textures={}
for filename in ('CoatGrain.png','CoatNormal.png'):
    digest=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
    textures[filename]=digest(folder/filename);assert textures[filename]==digest(baseline/filename)
report={'variant':'MartyPhotoSmooth','vertices':len(pet.data.vertices),'bones':len(rig.data.bones),'fur_strands':0,
        'maximum_bind_matrix_difference':difference,'maximum_primary_skin_difference':skin_difference,
        'maximum_weight_sum_error':error,'closed_primary_skin':True,'custom_surface_normals':True,
        'textures_match_originals':textures}
(root/'Saved'/'marty-photo-smooth-source-audit.json').write_text(json.dumps(report,indent=2))
print('MARTY_PHOTO_SMOOTH_SOURCE_AUDIT_PASSED')
