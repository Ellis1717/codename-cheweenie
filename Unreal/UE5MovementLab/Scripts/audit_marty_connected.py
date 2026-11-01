"""Blender source audit: unchanged bind rig, normalized weights and retained maps."""
import bpy, pathlib, json, hashlib
root=pathlib.Path(__file__).resolve().parent.parent
reports=[]
for folder,old,name in [('MartyConnected','Marty','Marty_CoverRig'),('MartyConnectedRealism','MartyRealism','Marty_RealCoat')]:
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
    assert difference<.0001,difference
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
    reports.append({'variant':folder,'vertices':len(pet.data.vertices),'bones':len(rig.data.bones),
                    'maximum_bind_matrix_difference':difference,'maximum_weight_sum_error':error,
                    'maximum_influences':maximum,'textures_match_originals':textures,'primary_skin_topology':proof})
(root/'Saved'/'marty-connected-source-audit.json').write_text(json.dumps(reports,indent=2))
print('MARTY_CONNECTED_SOURCE_AUDIT_PASSED')
