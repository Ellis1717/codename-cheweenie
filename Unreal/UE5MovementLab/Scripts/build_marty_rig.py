"""Blender 5: original cover-guided Marty, centimetres, smooth skin and named canine joints.

Run blender --background --python this_file. No downloaded geometry or paid add-ons.
The retained Explorer and comparison meshes are never modified.
"""
import bpy, bmesh, math, os, json, hashlib
import numpy as np
from mathutils import Vector

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'SourceArt', 'Marty')
os.makedirs(OUT, exist_ok=True)
bpy.ops.object.select_all(action='SELECT'); bpy.ops.object.delete(use_global=False)
scene = bpy.context.scene
scene.unit_settings.system = 'METRIC'; scene.unit_settings.scale_length = .01
parts = []
bones = {'root': ((0,0,0),(0,0,5),None), 'spine': ((-8,0,24),(12,0,25),'root'),
         'head': ((15,0,30),(27,0,35),'spine'), 'tail': ((-27,0,27),(-37,0,33),'spine')}

def mat(name, color, roughness):
    m=bpy.data.materials.new(name); m.diffuse_color=(*color,1)
    m.use_nodes=True; p=m.node_tree.nodes.get('Principled BSDF')
    p.inputs['Base Color'].default_value=(*color,1); p.inputs['Roughness'].default_value=roughness
    return m
coat=mat('Marty_Coat',(.46,.205,.065),.72)
cream=mat('Marty_Cream',(.64,.40,.20),.78)
earmat=mat('Marty_Ears',(.24,.075,.022),.78)
eye=mat('Marty_Iris',(.12,.045,.012),.18)
black=mat('Marty_Nose',(.012,.008,.006),.28)
pupil=mat('Marty_Pupil',(.002,.002,.002),.12)

def bind(o, bone):
    g=o.vertex_groups.new(name=bone); g.add(list(range(len(o.data.vertices))),1,'REPLACE')

def oval(name, p, scale, material, bone='spine', smooth=1):
    bpy.ops.mesh.primitive_uv_sphere_add(segments=40, ring_count=24, location=p)
    o=bpy.context.object; o.name=name; o.scale=scale
    bpy.ops.object.transform_apply(location=False,rotation=False,scale=True)
    o.data.materials.append(material); bind(o,bone)
    for f in o.data.polygons: f.use_smooth=True
    parts.append(o); return o

# Blend the torso/neck/head/muzzle into a single organic surface, avoiding ball joints.
shell=[]
for name,p,s in [('Long ribcage',(-6,0,23),(21,8.3,10)),('Chest',(10,0,24),(10,8.6,11)),
                 ('Neck',(16,0,30),(8,7.1,11)),('Skull',(23,0,37),(10,8.5,10)),
                 ('Muzzle',(30,0,33),(9,5.6,4.8)),('Jaw',(29,0,30.6),(8,5.1,2.8))]:
    shell.append(oval(name,p,s,coat))
bpy.ops.object.select_all(action='DESELECT')
for o in shell:o.select_set(True)
bpy.context.view_layer.objects.active=shell[0]; bpy.ops.object.join()
body=shell[0]; body.name='Continuous body neck head'
rem=body.modifiers.new('Organic union','REMESH'); rem.mode='VOXEL'; rem.voxel_size=.62; rem.use_smooth_shade=True
bpy.ops.object.modifier_apply(modifier=rem.name)
sm=body.modifiers.new('Soft anatomical transition','SMOOTH'); sm.factor=1.2; sm.iterations=5
bpy.ops.object.modifier_apply(modifier=sm.name)
parts=[body]
body.vertex_groups.clear(); sg=body.vertex_groups.new(name='spine'); hg=body.vertex_groups.new(name='head')
for v in body.data.vertices:
    p=body.matrix_world@v.co; w=max(0,min(1,(p.x-12)/9))*max(0,min(1,(p.z-23)/7))
    sg.add([v.index],1-w,'REPLACE'); hg.add([v.index],w,'REPLACE')
# Subtle cream chin, distinct wet nose and inset eyes with corneal highlight.
oval('Cream chin',(32,0,30),(6,4.8,1.5),cream,'head')
oval('Velvet nose',(37.1,0,33.6),(1.7,3.5,2.2),black,'head')
for side in (-1,1):
    oval('Nostril',(38.5,side*1.8,33.6),(.35,.65,.5),pupil,'head')
    oval('Eyelid',(27,side*6.65,38),(2.7,1.4,3.0),earmat,'head')
    oval('Amber eye',(27.65,side*7.35,38),(2.25,.85,2.5),eye,'head')
    oval('Pupil',(28.2,side*7.85,38.1),(1.25,.45,1.75),pupil,'head')
    # Ears are a tapered, curved continuous sheet with thickness, skin-weighted at base.
    name='ear_l' if side<0 else 'ear_r'
    bones[name]=((21,side*7,41),(20,side*10,34),'head')
    tip='ear_tip_l' if side<0 else 'ear_tip_r'
    bones[tip]=((20,side*10,34),(19,side*12,26),name)
    verts=[]; faces=[]
    rings=18; sides=24
    for r in range(rings):
        t=r/(rings-1); width=.3+3.6*math.sin(math.pi*t)**.6
        center=Vector((21-2*t,side*(7+5*math.sin(t*1.6)),41-15*t))
        for j in range(sides):
            a=j*math.tau/sides; verts.append(tuple(center+Vector((width*math.cos(a),side*.7*math.sin(a),0))))
    for r in range(rings-1):
        for j in range(sides):
            a=r*sides+j; b=r*sides+(j+1)%sides; faces.append((a,b,b+sides,a+sides))
    faces.append(tuple(range(sides-1,-1,-1)));faces.append(tuple((rings-1)*sides+j for j in range(sides)))
    mesh=bpy.data.meshes.new(name); mesh.from_pydata(verts,[],faces); mesh.update()
    o=bpy.data.objects.new(name,mesh); scene.collection.objects.link(o); o.data.materials.append(earmat)
    g=o.vertex_groups.new(name=name); h=o.vertex_groups.new(name='head'); tg=o.vertex_groups.new(name=tip)
    for v in mesh.vertices:
        w=min(1,max(0,(41-v.co.z)/4));tw=min(1,max(0,(36-v.co.z)/7))
        g.add([v.index],w*(1-tw),'REPLACE');h.add([v.index],1-w,'REPLACE');tg.add([v.index],w*tw,'REPLACE')
    for f in mesh.polygons:f.use_smooth=True
    parts.append(o)

# Continuous lofted limbs; weights interpolate through elbow/stifle and wrist/hock.
for front in (True,False):
    for side in (-1,1):
        key=('front' if front else 'hind')+('_l' if side<0 else '_r')
        hip=Vector((13 if front else -20,side*4.5,24))
        knee=Vector((10 if front else -12,side*6.5,13))
        hock=Vector((13 if front else -18,side*6.5,5))
        paw=Vector((16 if front else -15,side*6.5,2.5))
        names=[key+'_upper',key+'_lower',key+'_hock',key+'_paw']
        points=[hip,knee,hock,paw,paw+Vector((3,0,0))]
        for i in range(4):bones[names[i]]=(tuple(points[i]),tuple(points[i+1]),'spine' if i==0 else names[i-1])
        verts=[];weights=[];faces=[];sides=20
        for i in range(3):
            for r in range(9):
                t=r/8; center=points[i].lerp(points[i+1],t)
                radius=[(1.7,2.4),(2.4,1.65),(1.65,2.1)][i]; rad=radius[0]*(1-t)+radius[1]*t
                for j in range(sides):
                    a=j*math.tau/sides
                    verts.append(tuple(center+Vector((rad*math.cos(a),rad*.83*math.sin(a),0))))
                    # Smooth skin blend around joints rather than detached rigid segments.
                    w=max(0,(t-.55)/.45)*.5
                    previous=max(0,(.45-t)/.45)*.5 if i>0 else 0
                    ws={names[i]:1-w-previous,names[min(i+1,3)]:w,**({names[i-1]:previous} if i>0 else {})}
                    if i==0:
                        anchor=max(0,1-t*2)
                        ws={n:value*(1-anchor) for n,value in ws.items()};ws['spine']=anchor
                    weights.append(ws)
        for r in range(len(verts)//sides-1):
            for j in range(sides):
                a=r*sides+j;b=r*sides+(j+1)%sides;faces.append((a,a+sides,b+sides,b))
        faces.append(tuple(range(sides)));faces.append(tuple(len(verts)-sides+j for j in range(sides-1,-1,-1)))
        mesh=bpy.data.meshes.new(key);mesh.from_pydata(verts,[],faces);mesh.update()
        o=bpy.data.objects.new(key,mesh);scene.collection.objects.link(o);o.data.materials.append(coat)
        groups={n:o.vertex_groups.new(name=n) for n in names+['spine']}
        for i,ws in enumerate(weights):
            for n,w in ws.items():
                if w:groups[n].add([i],w,'REPLACE')
        for f in mesh.polygons:f.use_smooth=True
        parts.append(o)
        oval(key+' paw',tuple(paw+Vector((1,0,0))),(4.2,2.8,1.9),coat,names[3])
        for toe in (-1,0,1):oval(key+' nail',tuple(paw+Vector((4,toe*1.45,0))),(.7,.48,.45),black,names[3])
# Continuous tail taper, avoiding overlapping bead silhouettes.
verts=[];faces=[]
for i in range(18):
    t=i/17;radius=2.1*(1-t)+.08
    for j in range(20):
        a=j*math.tau/20;verts.append((-27-12*t,math.cos(a)*radius,27+9*t+math.sin(a)*radius))
for i in range(17):
    for j in range(20):
        a=i*20+j;b=i*20+(j+1)%20;faces.append((a,b,b+20,a+20))
faces.append(tuple(range(19,-1,-1)));faces.append(tuple(17*20+j for j in range(20)))
mesh=bpy.data.meshes.new('Tapered tail');mesh.from_pydata(verts,[],faces);mesh.update()
o=bpy.data.objects.new('Tapered tail',mesh);scene.collection.objects.link(o);o.data.materials.append(coat);bind(o,'tail')
for f in mesh.polygons:f.use_smooth=True
parts.append(o)

bpy.ops.object.select_all(action='DESELECT')
for o in parts:o.select_set(True)
bpy.context.view_layer.objects.active=body;bpy.ops.object.join();pet=body;pet.name='Marty_CoverRig'
bm=bmesh.new();bm.from_mesh(pet.data)
bmesh.ops.remove_doubles(bm,verts=list(bm.verts),dist=.00001)
bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces));bm.to_mesh(pet.data);bm.free();pet.data.update()
# The joined object origin is normalized before binding to the centimetre skeleton.
bpy.ops.object.transform_apply(location=True,rotation=True,scale=True)
# A continuous cylindrical coat UV, independent of the reference illustration.
uv=pet.data.uv_layers.active or pet.data.uv_layers.new(name='CoatUV')
for poly in pet.data.polygons:
    coords=[]
    for li in poly.loop_indices:
        p=pet.data.vertices[pet.data.loops[li].vertex_index].co
        coords.append(((p.x+42)/85,math.atan2(p.y,p.z-24)/math.tau+.5))
    if max(v for u,v in coords)-min(v for u,v in coords)>.5:coords=[(u,v+1 if v<.5 else v) for u,v in coords]
    for li,co in zip(poly.loop_indices,coords):uv.data[li].uv=co
# Original short-hair grain and tangent normal texture, seeded and tileable.
rng=np.random.default_rng(1717);size=512;height=np.zeros((size,size),dtype=np.float32)
for i in range(22000):
    x,y=rng.integers(0,size,2);length=int(rng.integers(5,22));strength=float(rng.uniform(.2,1))
    for j in range(length):height[y,(x+j)%size]+=strength*math.sin(math.pi*j/length)
height=height/(height.max()+1e-5);height=.5+.5*height
dx=(np.roll(height,-1,1)-np.roll(height,1,1))*.45;dy=(np.roll(height,-1,0)-np.roll(height,1,0))*.45
for name,channels in [('CoatGrain',(height,height,height)),('CoatNormal',(-dx*.5+.5,-dy*.5+.5,np.ones_like(height)))]:
    image=bpy.data.images.new(name,width=size,height=size,alpha=True)
    rgba=np.stack((*channels,np.ones_like(height)),axis=-1).astype(np.float32)
    image.pixels.foreach_set(rgba.ravel());image.filepath_raw=os.path.join(OUT,name+'.png');image.file_format='PNG';image.save()
bpy.ops.object.armature_add(enter_editmode=True,location=(0,0,0));rig=bpy.context.object;rig.name='Marty_Anatomy'
rig.data.edit_bones.remove(rig.data.edit_bones[0])
for name,(a,b,parent) in bones.items():
    bone=rig.data.edit_bones.new(name);bone.head=a;bone.tail=b
    if parent:bone.parent=rig.data.edit_bones[parent]
bpy.ops.object.mode_set(mode='OBJECT')
mod=pet.modifiers.new('Smooth canine skin','ARMATURE');mod.object=rig;pet.parent=rig
bpy.ops.object.select_all(action='DESELECT');rig.select_set(True);pet.select_set(True);bpy.context.view_layer.objects.active=rig
bpy.ops.wm.save_as_mainfile(filepath=os.path.join(OUT,'Marty_CoverRig.blend'))
bpy.ops.export_scene.fbx(filepath=os.path.join(OUT,'Marty_CoverRig.fbx'),use_selection=True,object_types={'ARMATURE','MESH'},
    add_leaf_bones=False,bake_anim=False,axis_forward='-Y',axis_up='Z',apply_unit_scale=True,use_mesh_modifiers=True)
with open(os.path.join(OUT,'manifest.json'),'w') as f:
    json.dump({'author':'Codename .... Cheweenie original generated geometry','reference':'Assets/Resources/Branding/Cover.png (start-page illustration)',
               'method':'Blender smooth skin-weighted canine rig; runtime two-bone IK, not motion capture','bones':list(bones),'vertices':len(pet.data.vertices),
               'source_script_sha256':hashlib.sha256(open(__file__,'rb').read()).hexdigest()},f,indent=2)
print('MARTY_RIG_EXPORTED',len(pet.data.vertices),'vertices',len(bones),'bones')
