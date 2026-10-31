"""Original Ratchet tribute mesh from owner-confirmed A01-A04 observations.
Static first-pass model: fuller black coat, white socks/toes/bib and bushy tail.
Unseen anatomy/iris hue are inferred; this is not a scan or an animated feline rig.
"""
import bpy, math, random, os
from mathutils import Vector
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT=os.path.join(ROOT,'SourceArt','Ratchet');os.makedirs(OUT,exist_ok=True)
bpy.ops.object.select_all(action='SELECT');bpy.ops.object.delete(use_global=False)
bpy.context.scene.unit_settings.system='METRIC';bpy.context.scene.unit_settings.scale_length=.01
parts=[]
def mat(name,color,rough=.7):
    m=bpy.data.materials.new(name);m.diffuse_color=(*color,1);m.use_nodes=True
    p=m.node_tree.nodes.get('Principled BSDF');p.inputs['Base Color'].default_value=(*color,1);p.inputs['Roughness'].default_value=rough
    return m
black=mat('Ratchet_Black',(.018,.014,.012));white=mat('Ratchet_White',(.74,.72,.68));nose=mat('Ratchet_Nose',(.03,.018,.02),.4);iris=mat('Ratchet_Iris',(.26,.29,.12),.32);eye=mat('Ratchet_Eye',(.008,.009,.009),.15);ear=mat('Ratchet_InnerEar',(.10,.065,.06))
def oval(name,p,size,m):
    bpy.ops.mesh.primitive_uv_sphere_add(segments=24,ring_count=16,location=p);o=bpy.context.object;o.name=name;o.scale=size
    bpy.ops.object.transform_apply(location=False,rotation=False,scale=True);o.data.materials.append(m)
    for f in o.data.polygons:f.use_smooth=True
    parts.append(o);return o
oval('Long soft torso',(-5,0,21),(18,10,11),black);oval('Chest',(9,0,23),(10,9,11),black)
oval('White bib',(18,0,23),(1.5,5.2,6.5),white);oval('White belly',(-3,0,13),(13,7,2.8),white)
oval('Feline head',(17,0,32),(8.4,7.9,7.5),black)
for side in [-1,1]:
    oval('Cheek',(21,side*3.1,29.5),(3.5,3,2.5),black)
    oval('Eye lid',(22.2,side*5.2,34),(.9,1.6,1.5),black)
    oval('Muted provisional iris',(23.05,side*5.2,34),(.18,1.05,1.15),iris)
    oval('Eye pupil',(23.21,side*5.2,34),(.10,.32,.9),eye)
    oval('Small white chin',(22,0,27.5),(3,3.8,1.3),white)
    # Rounded triangular ears, with inset dark lining.
    verts=[(12,side*3.2,36),(21,side*6.6,36),(15,side*7.2,44),(13,side*5.5,36)]
    mesh=bpy.data.meshes.new('Ear');mesh.from_pydata(verts,[],[(0,1,2),(0,2,3),(1,3,2),(0,3,1)]);mesh.materials.append(black)
    obj=bpy.data.objects.new('Pointed ear',mesh);bpy.context.collection.objects.link(obj);parts.append(obj)
    oval('Inner ear',(16.2,side*6.2,39),(1.1,1,2.8),ear)
    for x in [-17,10]:
        oval('Leg',(x,side*6.4,10),(3.8,3.5,8),black)
        oval('Paw',(x+1,side*6.4,3),(4.5,3.4,2.8),white if x<0 else black)
        if x>0:oval('White front toes',(x+4,side*6.4,2.6),(1.8,3,1.8),white)
oval('Small dark nose',(25,0,30),(1.2,1.5,1),nose)
# Unite the black anatomical forms before adding the separate markings and fur.
core=[o for o in parts if any(o.name.startswith(n) for n in ['Long soft torso','Chest','Feline head','Cheek','Leg'])]
bpy.ops.object.select_all(action='DESELECT')
for o in core:o.select_set(True)
bpy.context.view_layer.objects.active=core[0];bpy.ops.object.join();body=bpy.context.object
rem=body.modifiers.new('Continuous feline body','REMESH');rem.mode='VOXEL';rem.voxel_size=.45;rem.use_smooth_shade=True
bpy.ops.object.modifier_apply(modifier=rem.name)
sm=body.modifiers.new('Soft transitions','SMOOTH');sm.factor=1;sm.iterations=3;bpy.ops.object.modifier_apply(modifier=sm.name)
parts=[o for o in parts if o not in core]+[body]
# Curved, full tail, distinct from Rio's thinner silhouette.
for i in range(12):
    t=i/11;p=(-19-18*t,-3-12*math.sin(t*1.5),21-11*t)
    oval('Bushy tail',p,(5.8,4.4,4.4),black)
# Fine tapered fur tufts follow the torso and tail; deterministic, untextured geometry.
rng=random.Random(1717);verts=[];faces=[]
for i in range(8500):
    a=rng.uniform(0,math.tau);z=rng.uniform(-.85,.95);rad=math.sqrt(max(0,1-z*z));n=Vector((rad*math.cos(a),rad*math.sin(a),z))
    p=Vector((-5+18*n.x,10*n.y,21+11*n.z));d=Vector((-1,.1*n.y,-.15)).normalized();length=rng.uniform(.45,1.7)
    width=rng.uniform(.025,.07);cross=n.cross(d).normalized()*width
    base=len(verts);verts.extend([tuple(p-cross),tuple(p+cross),tuple(p+d*length+n*.3)])
    faces.append((base,base+1,base+2))
mesh=bpy.data.meshes.new('Directional coat tufts');mesh.from_pydata(verts,[],faces);mesh.materials.append(black)
obj=bpy.data.objects.new('Short-to-medium coat fringe',mesh);bpy.context.collection.objects.link(obj);parts.append(obj)
# Long pale whiskers, small tapered curves converted to mesh.
for side in [-1,1]:
    for i in range(4):
        c=bpy.data.curves.new('Whisker','CURVE');c.dimensions='3D';c.bevel_depth=.022;c.bevel_resolution=1
        s=c.splines.new('POLY');s.points.add(2)
        for pt,p in zip(s.points,[(23,side*3,29),(25,side*9,29+i*.35),(24,side*15,28+i*.8)]):pt.co=(*p,1)
        o=bpy.data.objects.new('Pale whisker',c);bpy.context.collection.objects.link(o);o.data.materials.append(white)
        bpy.context.view_layer.objects.active=o;o.select_set(True);bpy.ops.object.convert(target='MESH');parts.append(bpy.context.object);bpy.ops.object.select_all(action='DESELECT')
bpy.ops.object.select_all(action='DESELECT')
for o in parts:o.select_set(True)
bpy.context.view_layer.objects.active=parts[0];bpy.ops.object.join();pet=bpy.context.object;pet.name='Ratchet_Tribute'
bpy.context.scene.cursor.location=(0,0,0);bpy.ops.object.origin_set(type='ORIGIN_CURSOR')
bpy.ops.wm.save_as_mainfile(filepath=os.path.join(OUT,'Ratchet_Tribute.blend'))
bpy.ops.export_scene.fbx(filepath=os.path.join(OUT,'Ratchet_Tribute.fbx'),use_selection=True,object_types={'MESH'},apply_unit_scale=True,axis_forward='-Y',axis_up='Z',add_leaf_bones=False,bake_anim=False)
print('RATCHET_SOURCE_READY',len(pet.data.vertices))
