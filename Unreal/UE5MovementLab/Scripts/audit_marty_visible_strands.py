"""Blender source audit: photo stance and retained rig hierarchy, normalized weights and retained maps."""
import bpy, pathlib, json, hashlib
root=pathlib.Path(__file__).resolve().parent.parent
reports=[]
for folder,old,name in [('MartyVisibleStrands','MartyPhotoReference','Marty_RealCoat')]:
    bpy.ops.wm.open_mainfile(filepath=str(root/'SourceArt'/folder/(name+'.blend')))
    pet=bpy.data.objects[name];rig=pet.find_armature();assert rig
    with bpy.data.libraries.load(str(root/'SourceArt'/old/(name+'.blend')),link=False) as (source,target):
        assert 'Marty_Anatomy' in source.objects;target.objects=['Marty_Anatomy']
    previous=target.objects[0]
    assert set(rig.data.bones.keys())==set(previous.data.bones.keys())
    difference=0
    for bone in rig.data.bones:
        other=previous.data.bones[bone.name]
        assert (bone.parent.name if bone.parent else None)==(other.parent.name if other.parent else None)
        difference=max(difference,max(abs(bone.matrix_local[i][j]-other.matrix_local[i][j]) for i in range(4) for j in range(4)))
    assert difference<.00001,difference
    assert abs(rig.data.bones['spine'].head_local.z-19.8)<.001
    assert (rig.data.bones['ear_l'].head_local-rig.data.bones['ear_tip_l'].head_local).length<6.5
    manifest=json.loads((root/'SourceArt'/folder/'manifest.json').read_text());assert manifest['fur_hairs']==64000
    assert pet.data.has_custom_normals, 'Combed fibre surface normals must survive joining'
    assert len(pet.data.vertices)<=350000, 'Bound the dense-coat vertex budget'
    fur_faces=[p for p in pet.data.polygons if pet.data.materials[p.material_index].name=='Marty_FineFur']
    assert len(fur_faces)==128000
    minimum_dot=min(p.normal.dot(pet.data.corner_normals[p.loop_start].vector) for p in fur_faces)
    assert minimum_dot>-.01,minimum_dot
    maximum=0;error=0
    for vertex in pet.data.vertices:
        maximum=max(maximum,sum(g.weight>.001 for g in vertex.groups))
        error=max(error,abs(sum(g.weight for g in vertex.groups)-1))
    assert error<.0001 and maximum<=4,(error,maximum)
    textures={}
    for filename in ('CoatGrain.png','CoatNormal.png'):
        digest=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
        textures[filename]=digest(root/'SourceArt'/folder/filename)
        assert textures[filename]==digest(root/'SourceArt'/old/filename)
    proof=json.loads((root/'SourceArt'/folder/'attachment-topology.json').read_text())
    assert proof['primary_skin_components']==1 and proof['boundary_edges']==0 and proof['nonmanifold_edges']==0
    with bpy.data.libraries.load(str(root/'SourceArt'/old/(name+'.blend')),link=False) as (source,target):
        target.objects=[name]
    previous_skin=target.objects[0]
    skin_count=proof['primary_skin_vertices']
    maximum_skin_difference=max((pet.data.vertices[i].co-previous_skin.data.vertices[i].co).length for i in range(skin_count))
    assert maximum_skin_difference<.00001,'Fur changes must preserve photo-guided primary skin'
    fur_indices=sorted({i for face in fur_faces for i in face.vertices})
    assert len(fur_indices)==256000
    lengths=[]
    for start in range(0,len(fur_indices),4):
        a,b,c,d=[pet.data.vertices[i].co for i in fur_indices[start:start+4]]
        lengths.append(((c+d-a-b)*.5).length)
    assert max(lengths)<.50 and min(lengths)>.075
    import bmesh
    bm=bmesh.new();bm.from_mesh(pet.data);n=proof['primary_skin_vertices']
    skin_edges=[e for e in bm.edges if all(v.index<n for v in e.verts)]
    assert all(e.is_manifold for e in skin_edges),'Photo skin must remain closed after stance warp'
    bm.free()
    assert proof['reference_bones_unchanged'] is False
    reports.append({'variant':folder,'vertices':len(pet.data.vertices),'bones':len(rig.data.bones),
                    'maximum_primary_skin_difference':maximum_skin_difference,'strand_lengths_cm':{'min':min(lengths),'mean':sum(lengths)/len(lengths),'max':max(lengths)},'outward_fur_faces':len(fur_faces),'minimum_fur_normal_alignment':minimum_dot,'custom_surface_normals':pet.data.has_custom_normals,'vertex_budget':350000,'spine_reference_height_cm':rig.data.bones['spine'].head_local.z,'maximum_bind_matrix_difference':difference,'maximum_weight_sum_error':error,
                    'maximum_influences':maximum,'textures_match_originals':textures,'primary_skin_topology':proof})
(root/'Saved'/'marty-visible-strands-source-audit.json').write_text(json.dumps(reports,indent=2))
print('MARTY_VISIBLE_STRANDS_SOURCE_AUDIT_PASSED')
