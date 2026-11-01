"""Blender 5: welded ear/tail roots, retained original rig and material conventions.

Run --background --python this_file [-- --detailed]. Existing sources stay intact.
The original generators provide the rest of the anatomy and short-coat pipeline.
"""
import pathlib, sys

script = pathlib.Path(__file__).resolve()
connected_base = (script.parent / 'build_marty_rig.py').read_text()
edits = {
    'side*(7+5*math.sin(t*1.6))': 'side*(5+7*math.sin(t*1.6))',
    '(-27-12*t,math.cos(a)*radius,27+9*t+math.sin(a)*radius)':
        '(-23-16*t,math.cos(a)*radius,25+11*t+math.sin(a)*radius)',
    "o.data.materials.append(coat);bind(o,'tail')": """o.data.materials.append(coat)
tg=o.vertex_groups.new(name='tail');sg=o.vertex_groups.new(name='spine')
for v in mesh.vertices:
    t=max(0,min(1,(-23-v.co.x)/16));w=max(0,min(1,(t-.06)/.28));w=w*w*(3-2*w)
    tg.add([v.index],w,'REPLACE');sg.add([v.index],1-w,'REPLACE')""",
}
for before, after in edits.items():
    assert before in connected_base, before
    connected_base = connected_base.replace(before, after)

# Union only the primary skin. Eyes, collar, paws, fur and other separate details
# retain their authored surfaces. Transfer weights before the common UV/fur pass.
union = r'''
from mathutils.bvhtree import BVHTree
root_parts=[body,bpy.data.objects['ear_l'],bpy.data.objects['ear_r'],bpy.data.objects['Tapered tail']]
bpy.ops.object.select_all(action='DESELECT')
for item in root_parts:item.select_set(True)
bpy.context.view_layer.objects.active=body;bpy.ops.object.join()
parts=[item for item in parts if item==body or item not in root_parts]
bm=bmesh.new();bm.from_mesh(body.data);bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces));bm.to_mesh(body.data);bm.free()
source=body.copy();source.data=body.data.copy();scene.collection.objects.link(source)
source.name='Attachment weight source';source.select_set(False)
source.data.calc_loop_triangles()
triangles=[tuple(t.vertices) for t in source.data.loop_triangles]
tree=BVHTree.FromPolygons([v.co for v in source.data.vertices],triangles,all_triangles=True)
names=[g.name for g in source.vertex_groups]
weights=[{g.group:g.weight for g in v.groups} for v in source.data.vertices]
rem=body.modifiers.new('Connected skin at ear and tail roots','REMESH')
rem.mode='VOXEL';rem.voxel_size=.32;rem.use_smooth_shade=True
bpy.context.view_layer.objects.active=body;bpy.ops.object.modifier_apply(modifier=rem.name)
sm=body.modifiers.new('Soft root transitions','SMOOTH');sm.factor=.3;sm.iterations=2
bpy.ops.object.modifier_apply(modifier=sm.name)
# A sub-voxel closed speck can form at the very tapered tail tip. Remove only
# islands of at most 16 vertices; larger disconnections still fail validation.
bm=bmesh.new();bm.from_mesh(body.data);remaining=set(bm.verts);tiny=[]
while remaining:
    seed=remaining.pop();stack=[seed];island=[seed]
    while stack:
        for edge in stack.pop().link_edges:
            for v in edge.verts:
                if v in remaining:remaining.remove(v);stack.append(v);island.append(v)
    if len(island)<=16:tiny.extend(island)
removed_tip_vertices=len(tiny)
if tiny:bmesh.ops.delete(bm,geom=tiny,context='VERTS')
bm.to_mesh(body.data);bm.free()
body.vertex_groups.clear();groups=[body.vertex_groups.new(name=n) for n in names]
for v in body.data.vertices:
    point,normal,idx,distance=tree.find_nearest(v.co);assert idx is not None
    indices=triangles[idx];a,b,c=[source.data.vertices[i].co for i in indices]
    u=b-a;w=c-a;p=point-a;uu=u.dot(u);uw=u.dot(w);ww=w.dot(w)
    denominator=uu*ww-uw*uw
    beta=(ww*p.dot(u)-uw*p.dot(w))/denominator if abs(denominator)>1e-12 else 0
    gamma=(uu*p.dot(w)-uw*p.dot(u))/denominator if abs(denominator)>1e-12 else 0
    bary=[max(0,1-beta-gamma),max(0,beta),max(0,gamma)];mixed={}
    for i,factor in zip(indices,bary):
        for group,weight in weights[i].items():mixed[group]=mixed.get(group,0)+weight*factor
    # The upper skull and ear junction move as one head surface. Fade the old
    # neck/spine influence out before it reaches the welded ear roots.
    world=body.matrix_world@v.co
    hx=max(0,min(1,(world.x-13)/4));hz=max(0,min(1,(world.z-27)/5))
    lock=(hx*hx*(3-2*hx))*(hz*hz*(3-2*hz))
    spine=names.index('spine');head=names.index('head');shift=mixed.get(spine,0)*lock
    mixed[spine]=mixed.get(spine,0)-shift;mixed[head]=mixed.get(head,0)+shift
    mixed={g:value for g,value in mixed.items() if value>.001};total=sum(mixed.values());assert total>0
    for group,value in mixed.items():groups[group].add([v.index],value/total,'REPLACE')
body.data.materials.clear()
for m in source.data.materials:body.data.materials.append(m)
for face in body.data.polygons:
    point=sum((body.data.vertices[i].co for i in face.vertices),Vector())/len(face.vertices)
    hit=tree.find_nearest(point);face.material_index=source.data.loop_triangles[hit[2]].material_index
    world=body.matrix_world@point
    if source.data.materials[face.material_index].name=='Marty_Ears' and abs(world.y)<8.5 and world.z>36:
        face.material_index=next(i for i,m in enumerate(source.data.materials) if m.name=='Marty_Coat')
    face.use_smooth=True
# A single closed skin makes a geometric crack impossible at shared root vertices.
bm=bmesh.new();bm.from_mesh(body.data);remaining=set(bm.verts);components=0
component_details=[]
while remaining:
    components+=1;seed=remaining.pop();stack=[seed];visited=[seed.co.copy()]
    while stack:
        for edge in stack.pop().link_edges:
            for v in edge.verts:
                if v in remaining:remaining.remove(v);stack.append(v);visited.append(v.co.copy())
    component_details.append({'vertices':len(visited),'min':[min(p[i] for p in visited) for i in range(3)],'max':[max(p[i] for p in visited) for i in range(3)]})
boundary=sum(e.is_boundary for e in bm.edges);nonmanifold=sum(not e.is_manifold for e in bm.edges)
bm.free();print("ROOT_COMPONENTS",component_details);assert components==1 and boundary==0 and nonmanifold==0,(components,boundary,nonmanifold)
proof={'primary_skin_components':components,'boundary_edges':boundary,'nonmanifold_edges':nonmanifold,
       'removed_subvoxel_tip_vertices':removed_tip_vertices,'primary_skin_vertices':len(body.data.vertices),'reference_bones_unchanged':True,
       'method':'Voxel union of torso/head/ears/tail; nearest-triangle interpolated normalized skin weights'}
with open(os.path.join(OUT,'attachment-topology.json'),'w') as f:json.dump(proof,f,indent=2)
bpy.data.objects.remove(source,do_unlink=True)
'''
join = "bpy.ops.object.select_all(action='DESELECT')\nfor o in parts:o.select_set(True)"
assert connected_base.count(join)==1
connected_base = connected_base.replace(join, union+'\n'+join)

def reuse_textures(base, folder):
    start=base.index('# Original short-hair grain and tangent normal texture')
    end=base.index('bpy.ops.object.armature_add(')
    copy="""# Retain bit-identical original textures; this change is skin topology only.
import shutil
for filename in ('CoatGrain.png','CoatNormal.png'):
    shutil.copyfile(os.path.join(ROOT,'SourceArt','%s',filename),os.path.join(OUT,filename))
""" % folder
    return base[:start]+copy+base[end:]

if '--detailed' in sys.argv:
    detailed=(script.parent/'build_marty_realcoat.py').read_text()
    detailed=detailed.replace("base = (script.parent / 'build_marty_rig.py').read_text()",'base = connected_base')
    detailed=detailed.replace("'SourceArt', 'MartyRealism'","'SourceArt', 'MartyConnectedRealism'")
    detailed=detailed.replace("split = base.index('bpy.ops.object.armature_add(')","base = reuse_textures(base, 'MartyRealism')\nsplit = base.index('bpy.ops.object.armature_add(')")
    detailed=detailed.replace("if material == 'Marty_Ears': return (.30,.125,.042,1)","if material == 'Marty_Ears':\n        blend=max(0,min(1,(abs(p.y)-8.5)/3.5));blend=blend*blend*(3-2*blend)\n        col=Vector((.58,.31,.105)).lerp(Vector((.30,.125,.042)),blend);return (*col,1)")
    exec(compile(detailed,str(script),'exec'),globals())
else:
    smooth=connected_base.replace("'SourceArt', 'Marty'","'SourceArt', 'MartyConnected'")
    smooth=reuse_textures(smooth,'Marty')
    smooth=smooth.replace('bpy.ops.wm.save_as_mainfile(filepath=','bpy.ops.wm.save_as_mainfile(compress=True,filepath=')
    exec(compile(smooth,str(script),'exec'),globals())
