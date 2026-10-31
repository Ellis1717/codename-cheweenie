"""Original connected twig/leaf tree, gable infill and irregular grass PBR maps."""
import bpy, pathlib, math, json, random, hashlib
import numpy as np
from mathutils import Vector
ROOT=pathlib.Path(bpy.data.filepath).resolve().parents[2];OUT=ROOT/'SourceArt'/'WorldRealism'
randoms=random.Random(1951);verts=[];faces=[];indices=[];uvs=[]
def twig(a,b,r1,r2):
    a,b=Vector(a),Vector(b);direction=(b-a).normalized();u=direction.cross(Vector((0,0,1)))
    if u.length<.01:u=Vector((1,0,0))
    u.normalize();v=direction.cross(u).normalized();base=len(verts);sides=7
    for p,r in ((a,r1),(b,r2)):
        for i in range(sides):
            q=p+r*(u*math.cos(i*math.tau/sides)+v*math.sin(i*math.tau/sides));verts.append(tuple(q));uvs.append((q.z/100,i/sides))
    for i in range(sides):faces.append((base+i,base+(i+1)%sides,base+(i+1)%sides+sides,base+i+sides));indices.append(0)
def leaf(base,direction,length,width):
    direction=Vector(direction).normalized();side=direction.cross(Vector((0,0,1)))
    if side.length<.01:side=Vector((1,0,0))
    side.normalize();up=side.cross(direction);first=len(verts)
    shape=[(0,0),(.28,-.45),(.42,-.33),(.55,-.68),(.70,-.38),(1,0),(.70,.38),(.55,.68),(.42,.33),(.28,.45)]
    verts.append(tuple(base+direction*length*.48+up*.5));uvs.append((.5,.5))
    for t,w in shape:
        verts.append(tuple(base+direction*(t*length)+side*(w*width)+up*(math.sin(t*math.pi)*.35)));uvs.append((t,.5+w*.7))
    for i in range(len(shape)):faces.append((first,first+1+i,first+1+(i+1)%len(shape)));indices.append(1)
twig((0,0,0),(7,3,240),17,10);twig((7,3,240),(-4,0,430),10,3.5)
for i in range(12):
    angle=i*2.399;start=Vector((2,0,230+i*12));end=Vector((math.cos(angle)*randoms.uniform(130,220),math.sin(angle)*randoms.uniform(130,220),randoms.uniform(430,640)))
    middle=start.lerp(end,.55)+Vector((0,0,25));twig(start,middle,6,3.1);twig(middle,end,3.1,1.5)
    for j in range(18):
        origin=middle.lerp(end,randoms.uniform(.2,1));a=randoms.uniform(0,math.tau);tip=origin+Vector((math.cos(a)*randoms.uniform(35,85),math.sin(a)*randoms.uniform(35,85),randoms.uniform(5,70)))
        twig(origin,tip,1.2,.16)
        for k in range(16):
            p=origin.lerp(tip,.15+.85*k/15);angle=a+(math.pi/2 if k%2 else -math.pi/2)+randoms.uniform(-.4,.4)
            d=Vector((math.cos(angle),math.sin(angle),randoms.uniform(-.5,.7))).normalized();petiole=p+d*3;twig(p,petiole,.11,.07);leaf(petiole,d,randoms.uniform(8,15),randoms.uniform(6,10))
old=bpy.data.objects.get('LeafTree')
if old:bpy.data.objects.remove(old,do_unlink=True)
me=bpy.data.meshes.new('Connected tree branches and thin serrated leaves');me.from_pydata(verts,[],faces);me.update();me.materials.append(bpy.data.materials['Bark']);me.materials.append(bpy.data.materials['Leaf'])
tree=bpy.data.objects.new('LeafTree',me);bpy.context.scene.collection.objects.link(tree);layer=me.uv_layers.new(name='SurfaceUV')
for poly,index in zip(me.polygons,indices):
    poly.material_index=index;poly.use_smooth=index==0
    for li in poly.loop_indices:layer.data[li].uv=uvs[me.loops[li].vertex_index]
def export(obj):
    bpy.ops.object.select_all(action='DESELECT');obj.select_set(True);bpy.context.view_layer.objects.active=obj
    bpy.ops.export_scene.fbx(filepath=str(OUT/(obj.name+'.fbx')),use_selection=True,object_types={'MESH'},axis_forward='-Y',axis_up='Z',apply_unit_scale=True,use_mesh_modifiers=True,bake_anim=False)
export(tree)
old=bpy.data.objects.get('RoofGable')
if old:bpy.data.objects.remove(old,do_unlink=True)
me=bpy.data.meshes.new('Unit gable infill');me.from_pydata([(-.5,-.5,0),(.5,-.5,0),(0,-.5,1),(-.5,.5,0),(.5,.5,0),(0,.5,1)],[],[(0,2,1),(3,4,5),(0,1,4,3),(1,2,5,4),(2,0,3,5)]);me.update();me.materials.append(bpy.data.materials.get('Brick') or bpy.data.materials.new('Brick'));gable=bpy.data.objects.new('RoofGable',me);bpy.context.scene.collection.objects.link(gable);me.uv_layers.new(name='SurfaceUV');export(gable)
n=1024;y,x=np.mgrid[0:n,0:n];rng=np.random.default_rng(919)
def field(size):
    grid=rng.random((size,size)).astype(np.float32)-.5
    gx=x*size/n;gy=y*size/n;ix=gx.astype(int);iy=gy.astype(int);u=gx-ix;v=gy-iy
    u=u*u*(3-2*u);v=v*v*(3-2*v)
    return (grid[iy%size,ix%size]*(1-u)+grid[iy%size,(ix+1)%size]*u)*(1-v)+(grid[(iy+1)%size,ix%size]*(1-u)+grid[(iy+1)%size,(ix+1)%size]*u)*v
variation=.18*field(5)+.12*field(17)+.06*field(63)+.025*field(180);h=field(180)*.08+(rng.random((n,n))-.5)*.018
base=np.clip(np.array((.30,.37,.17))[None,None,:]*(1+variation[:,:,None]),.02,.95)
dx=(np.roll(h,-1,1)-np.roll(h,1,1))*2;dy=(np.roll(h,-1,0)-np.roll(h,1,0))*2
normal=np.stack((-dx,dy,np.ones_like(h)),axis=-1);normal/=np.linalg.norm(normal,axis=-1,keepdims=True)
for name,array in [('Grass_Base',base),('Grass_Normal',normal*.5+.5),('Grass_Rough',np.repeat(np.clip(.94+variation*.1,.85,.99)[:,:,None],3,axis=2))]:
    image=bpy.data.images.new(name,width=n,height=n,alpha=True);image.colorspace_settings.name='Non-Color';image.pixels.foreach_set(np.concatenate((array,np.ones((n,n,1))),axis=-1).astype(np.float32).ravel());image.filepath_raw=str(OUT/(name+'.png'));image.file_format='PNG';image.save();bpy.data.images.remove(image)
bpy.context.preferences.filepaths.save_version=0;bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'WorldRealism.blend'),compress=True)
manifest=json.loads((OUT/'manifest.json').read_text())
for o in (tree,gable):manifest['meshes'][o.name]={'vertices':len(o.data.vertices),'triangles':sum(len(p.vertices)-2 for p in o.data.polygons),'materials':[m.name for m in o.data.materials]}
manifest['materials']['WindowGlass']={'color':[.16,.23,.28],'roughness':.12,'kind':'glass','tile_cm':100,'maps':False}
manifest['outdoor_generator_sha256']=hashlib.sha256((ROOT/'Scripts'/'build_world_outdoor.py').read_bytes()).hexdigest();manifest['generator_sha256']=hashlib.sha256((ROOT/'Scripts'/'build_world_art.py').read_bytes()).hexdigest();(OUT/'manifest.json').write_text(json.dumps(manifest,indent=2));print('CHEWEENIE_OUTDOOR_ART_CREATED',manifest['meshes']['LeafTree'])
