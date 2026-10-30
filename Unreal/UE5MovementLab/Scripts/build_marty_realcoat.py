"""Original short-coat variant. Preserves the 0.5 model, rig and all private photos.

Blender 5 --background --python this_file. Fine fur is skin-weighted geometry,
not a strand Groom simulation. All sources are generated without paid assets.
"""
import pathlib, random
import bpy
from mathutils import Vector

script = pathlib.Path(__file__).resolve()
base = (script.parent / 'build_marty_rig.py').read_text()
edits = {
    "'SourceArt', 'Marty'": "'SourceArt', 'MartyRealism'",
    "pet.name='Marty_CoverRig'": "pet.name='Marty_RealCoat'",
    "'Marty_CoverRig.blend'": "'Marty_RealCoat.blend'",
    "'Marty_CoverRig.fbx'": "'Marty_RealCoat.fbx'",
    "('Skull',(23,0,37),(10,8.5,10))": "('Skull',(23,0,37),(9.4,7.5,9.2))",
    "('Muzzle',(30,0,33),(9,5.6,4.8))": "('Muzzle',(30,0,33),(9,5.0,4.3))",
    "(27,side*6.65,38),(2.7,1.4,3.0)": "(28,side*5.8,38),(2.1,1.0,2.25)",
    "(27.65,side*7.35,38),(2.25,.85,2.5)": "(28.5,side*6.25,38),(1.8,.8,1.95)",
    "(28.2,side*7.85,38.1),(1.25,.45,1.75)": "(29.1,side*6.7,38.1),(1.0,.38,1.35)",
    "size=512;height=": "size=4096;height=",
    "range(22000)": "range(350000)",
    "rng.integers(5,22)": "rng.integers(8,38)",
    "bpy.ops.wm.save_as_mainfile(filepath=": "bpy.ops.wm.save_as_mainfile(compress=True,filepath=",
}
for before, after in edits.items():
    assert before in base, before
    base = base.replace(before, after)
split = base.index('bpy.ops.object.armature_add(')
# Reuse the proven anatomy/weights while exporting into a separate source folder.
exec(compile(base[:split], str(script), 'exec'), globals())

def coat_color(p, material='Marty_Coat'):
    if material == 'Marty_Ears': return (.30,.125,.042,1)
    if material == 'Marty_Cream': return (.72,.58,.36,1)
    # Golden short coat; cream throat/chest/belly and a subtly darker dorsal saddle.
    cream_weight = max(0,min(1,(21-p.z)/8)) * max(0,min(1,(7-abs(p.y))/4))
    if p.x>8: cream_weight=max(cream_weight,max(0,min(1,(31-p.z)/8))*max(0,min(1,(6-abs(p.y))/4)))
    dorsal=max(0,min(1,(p.z-27)/12)) if p.x<15 else 0
    gold=Vector((.58,.31,.105))*(1-.18*dorsal)
    c=gold.lerp(Vector((.74,.59,.37)),cream_weight*.85)
    return (*c,1)

# Color variation belongs to the moving skin, never to a projection of a photo.
colors=pet.data.color_attributes.new(name='CoatColor',type='FLOAT_COLOR',domain='CORNER')
pet.data.uv_layers[0].name='CoatUV'
for poly in pet.data.polygons:
    name=pet.data.materials[poly.material_index].name
    for li in poly.loop_indices:
        p=pet.data.vertices[pet.data.loops[li].vertex_index].co
        colors.data[li].color=coat_color(p,name)

fur_mat=mat('Marty_FineFur',(.58,.31,.105),.65)
collar_black=mat('Marty_CollarBlack',(.012,.015,.018),.65)
collar_white=mat('Marty_CollarWhite',(.7,.72,.66),.7)
tagmat=mat('Marty_TagBlue',(.025,.29,.52),.38)
accessories=[]
# Checker collar is a likeness cue from the supplied photographs. No tag numbers.
verts=[];faces=[]
for i in range(96):
    angle=i*math.tau/96
    for x in (13.1,15.8):verts.append((x,7.55*math.cos(angle),29+9*math.sin(angle)))
for i in range(96):faces.append((2*i,2*((i+1)%96),2*((i+1)%96)+1,2*i+1))
mesh=bpy.data.meshes.new('Checker collar');mesh.from_pydata(verts,[],faces);mesh.update()
collar=bpy.data.objects.new('Checker collar',mesh);scene.collection.objects.link(collar)
mesh.materials.append(collar_black);mesh.materials.append(collar_white);bind(collar,'head')
for f in mesh.polygons:f.material_index=(f.index//6)%2
accessories.append(collar)
tag=oval('Blue tag without personal details',(15.3,-7.9,24.3),(1.5,.35,2.0),tagmat,'head');accessories.append(tag)

# Surface-area weighted roots, with interpolated skin weights from each triangle.
pet.data.calc_loop_triangles()
tris=[];total=0
for t in pet.data.loop_triangles:
    name=pet.data.materials[t.material_index].name
    if name not in ('Marty_Coat','Marty_Cream','Marty_Ears'):continue
    total+=t.area;tris.append((total,tuple(t.vertices),name))
import bisect
areas=[t[0] for t in tris];rng=random.Random(1717)
hair_vertices=[];hair_faces=[];hair_weights=[];hair_colors=[]
groups={g.index:g.name for g in pet.vertex_groups}
source_weights=[{groups[g.group]:g.weight for g in v.groups} for v in pet.data.vertices]
count=16000
for h in range(count):
    _,indices,name=tris[bisect.bisect_left(areas,rng.random()*total)]
    a,b,c=[pet.data.vertices[i] for i in indices]
    u=math.sqrt(rng.random());v=rng.random();bary=(1-u,u*(1-v),u*v)
    p=a.co*bary[0]+b.co*bary[1]+c.co*bary[2]
    n=(a.normal*bary[0]+b.normal*bary[1]+c.normal*bary[2]).normalized()
    flow=Vector((1 if p.x>20 else -1,0,-.45))
    if name=='Marty_Ears':flow=Vector((0,0,-1))
    flow=(flow-n*flow.dot(n)).normalized()
    direction=(n*.55+flow*.84).normalized()
    length=rng.uniform(.18,.48) * (.55 if p.x>22 or name=='Marty_Ears' else 1)
    width=rng.uniform(.009,.018)
    side=direction.cross(Vector((0,0,1)))
    if side.length<.1:side=direction.cross(Vector((0,1,0)))
    side.normalize();across=direction.cross(side).normalized()
    ws={}
    for idx,w in zip(indices,bary):
        for bone,weight in source_weights[idx].items():ws[bone]=ws.get(bone,0)+weight*w
    ws={bone:w for bone,w in ws.items() if w>.025};norm=sum(ws.values());ws={bone:w/norm for bone,w in ws.items()}
    col=coat_color(p,name);variation=rng.uniform(.82,1.13);col=tuple(min(1,x*variation) for x in col[:3])+(1,)
    root=p-n*.025;tip=p+direction*length
    for axis in (side,across):
        start=len(hair_vertices)
        hair_vertices.extend([tuple(root-axis*width),tuple(root+axis*width),tuple(tip-axis*width*.08),tuple(tip+axis*width*.08)])
        hair_faces.extend([(start,start+2,start+1),(start+1,start+2,start+3)])
        hair_weights.extend([ws]*4);hair_colors.extend([col]*4)
mesh=bpy.data.meshes.new('Individual short fur');mesh.from_pydata(hair_vertices,[],hair_faces);mesh.update()
hair=bpy.data.objects.new('16000 skinned short hairs',mesh);scene.collection.objects.link(hair);mesh.materials.append(fur_mat)
hair_groups={name:hair.vertex_groups.new(name=name) for name in bones}
for start in range(0,len(hair_vertices),8):
    for bone,w in hair_weights[start].items():hair_groups[bone].add(list(range(start,start+8)),w,'REPLACE')
color=mesh.color_attributes.new(name='CoatColor',type='FLOAT_COLOR',domain='CORNER')
uv=mesh.uv_layers.new(name='CoatUV')
for poly in mesh.polygons:
    for li in poly.loop_indices:
        vi=mesh.loops[li].vertex_index;color.data[li].color=hair_colors[vi]
        # Longitudinal U generates an actual strand direction for hair shading.
        uv.data[li].uv=((vi%4)//2,(vi%4)%2)
accessories.append(hair)
# A few tapered whiskers, short enough to avoid cartoon wire silhouettes.
for sign in (-1,1):
    for i in range(5):
        start=Vector((33-i*.4,sign*4.4,32.5+i*.35));end=start+Vector((-1-i*.3,sign*(3+i*.3),.3+i*.1))
        delta=end-start
        bpy.ops.mesh.primitive_cone_add(vertices=5,radius1=.035,radius2=.003,depth=delta.length,location=(start+end)/2)
        o=bpy.context.object;o.name='Muzzle whisker';o.rotation_euler=delta.to_track_quat('Z','Y').to_euler();bpy.ops.object.transform_apply(location=False,rotation=True,scale=True)
        o.data.materials.append(cream);bind(o,'head');accessories.append(o)
for o in accessories:
    if len(o.data.uv_layers):o.data.uv_layers[0].name='CoatUV'
bpy.ops.object.select_all(action='DESELECT');pet.select_set(True)
for o in accessories:o.select_set(True)
bpy.context.view_layer.objects.active=pet;bpy.ops.object.join()
# Poseable skin colour streams differ between factories. Encode the original
# linear palette in two extra UV channels, which deform with the skin reliably.
rg=pet.data.uv_layers.new(name='CoatColorRG');ba=pet.data.uv_layers.new(name='CoatColorBA')
palette=pet.data.color_attributes['CoatColor']
for i in range(len(pet.data.loops)):
    c=palette.data[i].color;rg.data[i].uv=(c[0],c[1]);ba.data[i].uv=(c[2],c[3])
exec(compile(base[split:],str(script),'exec'),globals())
manifest=pathlib.Path(OUT)/'manifest.json'
data=json.loads(manifest.read_text());data.update(fur_hairs=count,coat_texture_resolution=4096,technique='Skin-weighted crossed tapered micro-ribbons; no Groom/cloth simulation',private_photos_embedded=False,likeness='Short tan coat, darker ears, cream chest, dark eyes, checker collar from retained owner references; inferred geometry')
manifest.write_text(json.dumps(data,indent=2))
