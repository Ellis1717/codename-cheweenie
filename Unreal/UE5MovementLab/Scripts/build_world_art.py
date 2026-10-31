"""Original centimetre-scale environment assets. Blender 5, no downloaded art/add-ons.

Rebuilds only SourceArt/WorldRealism. Does not edit retained pets, photos or previews.
Textures are newly authored procedural PBR maps, not edited reference photographs.
"""
import bpy, math, pathlib, json, random, hashlib
import numpy as np
from mathutils import Vector
ROOT=pathlib.Path(__file__).resolve().parents[1]
OUT=ROOT/'SourceArt'/'WorldRealism';OUT.mkdir(parents=True,exist_ok=True)
bpy.ops.object.select_all(action='SELECT');bpy.ops.object.delete(use_global=False)
scene=bpy.context.scene;scene.unit_settings.system='METRIC';scene.unit_settings.scale_length=.01
rng=random.Random(1717);groups={};materials={}
SPEC={
 'Oak':((.68,.56,.42),.52,'wood',180),
 'Plaster':((.82,.80,.75),.89,'plaster',140),
 'SofaLinen':((.36,.43,.40),.91,'fabric',35),
 'RugWool':((.30,.41,.49),.96,'fabric',45),
 'OchreLinen':((.69,.49,.26),.9,'fabric',35),
 'IvoryLinen':((.78,.75,.66),.93,'fabric',35),
 'Brick':((.52,.28,.20),.86,'brick',192),
 'Concrete':((.56,.55,.50),.87,'stone',200),
 'Asphalt':((.16,.17,.18),.94,'stone',300),
 'Grass':((.27,.36,.14),.95,'grass',200),
 'RoofSlate':((.21,.23,.25),.81,'roof',200),
 'Bark':((.30,.23,.15),.95,'wood',100),
 'Leaf':((.18,.29,.075),.75,'leaf',40),
 'Paint':((.80,.78,.72),.56,'plaster',150),
 'BluePaint':((.13,.25,.34),.48,'plaster',150),
 'Ceramic':((.63,.65,.58),.25,'stone',100),
 'Brass':((.56,.37,.13),.29,'metal',100),
 'DarkMetal':((.055,.065,.07),.43,'metal',100),
 'BookPaper':((.70,.66,.55),.86,'plaster',100)}

def image(name,array):
    n=array.shape[0];im=bpy.data.images.new(name,width=n,height=n,alpha=True)
    im.colorspace_settings.name='Non-Color'
    if array.ndim==2:array=np.stack((array,array,array),axis=-1)
    rgba=np.concatenate((array,np.ones((n,n,1))),axis=-1).astype(np.float32)
    im.pixels.foreach_set(rgba.ravel());im.filepath_raw=str(OUT/(name+'.png'));im.file_format='PNG';im.save();bpy.data.images.remove(im)

for index,(name,(color,rough,kind,scale)) in enumerate(SPEC.items()):
    mat=bpy.data.materials.new(name);mat.diffuse_color=(*color,1);materials[name]=mat
    if kind=='metal':continue
    n=2048 if name=='Oak' else 1024
    y,x=np.mgrid[0:n,0:n].astype(np.float32)/n
    randoms=np.random.default_rng(1717+index);noise=randoms.random((n,n),dtype=np.float32)-.5
    broad=(np.sin(x*math.tau*3)+np.sin(y*math.tau*5)+np.sin((x+y)*math.tau*7))/3
    h=.5+.035*noise;variation=.025*noise+.015*broad
    if kind=='wood':
        warp=.16*np.sin(x*math.tau*2)+.04*np.sin(x*math.tau*7)
        grain=np.sin((y*96+warp)*math.tau)*.5+.5
        fine=np.sin((y*370+warp*2)*math.tau)
        h+=.035*grain+.008*fine;variation+=.075*grain+.012*fine
    elif kind=='fabric':
        weft=np.sin(x*math.tau*256)*np.cos(y*math.tau*256)
        wrinkle=np.sin(x*math.tau*4+np.sin(y*math.tau*3))
        h+=.035*weft+.012*wrinkle;variation+=.018*weft+.012*wrinkle
    elif kind in ('brick','roof'):
        rows=24 if kind=='brick' else 14;cols=8 if kind=='brick' else 10
        row=np.floor(y*rows);u=(x*cols+(row%2)*.5)%1;v=(y*rows)%1
        joint=(u<.035)|(u>.965)|(v<.07)|(v>.93)
        block=np.sin(np.floor(x*cols+(row%2)*.5)*7.3+row*13.4)*.07
        h+=np.where(joint,-.16,.025*noise);variation+=block
        if kind=='brick':variation=np.where(joint,.22,variation)
        else:variation=np.where(joint,-.08,variation)
    elif kind=='grass':
        blades=np.sin(x*math.tau*180+np.sin(y*math.tau*35))
        h+=.05*blades;variation+=.08*broad+.035*blades
    elif kind=='leaf':
        vein=np.exp(-((x-.5)*100)**2);h+=vein*.1;variation+=.09*broad
    else:h+=.018*broad;variation+=.025*broad
    rgb=np.clip(np.array(color)[None,None,:]*(1+variation[:,:,None]),.015,.95)
    if kind=='brick':rgb=np.where(joint[:,:,None],np.array((.56,.53,.47)),rgb)
    dx=(np.roll(h,-1,axis=1)-np.roll(h,1,axis=1))*3
    dy=(np.roll(h,-1,axis=0)-np.roll(h,1,axis=0))*3
    normal=np.stack((-dx,dy,np.ones_like(h)),axis=-1);normal/=np.linalg.norm(normal,axis=-1,keepdims=True)
    image(name+'_Base',rgb);image(name+'_Normal',normal*.5+.5)
    image(name+'_Rough',np.clip(rough+variation*.2,.08,.99))

def register(o,group,material):
    o.data.materials.append(materials[material]);groups.setdefault(group,[]).append(o);return o
def box(name,p,size,material,group='RoomDetails',bevel=1.0,rot=(0,0,0)):
    bpy.ops.mesh.primitive_cube_add(size=1,location=p,rotation=rot);o=bpy.context.object;o.name=name;o.scale=size
    bpy.ops.object.transform_apply(location=False,rotation=False,scale=True)
    if bevel:
        b=o.modifiers.new('Real edge radius','BEVEL');b.width=bevel;b.segments=3
        bpy.ops.object.modifier_apply(modifier=b.name)
        w=o.modifiers.new('Weighted furniture normals','WEIGHTED_NORMAL');w.keep_sharp=True;bpy.ops.object.modifier_apply(modifier=w.name)
    return register(o,group,material)
def cylinder(name,p,radius,depth,material,group='RoomDetails',vertices=32):
    bpy.ops.mesh.primitive_cylinder_add(vertices=vertices,radius=radius,depth=depth,location=p);o=bpy.context.object;o.name=name
    b=o.modifiers.new('Rounded lip','BEVEL');b.width=min(.7,depth*.15);b.segments=2;bpy.ops.object.modifier_apply(modifier=b.name)
    for poly in o.data.polygons:poly.use_smooth=True
    return register(o,group,material)
def tube(name,points,radius,material,group='RoomDetails'):
    curve=bpy.data.curves.new(name,'CURVE');curve.dimensions='3D';curve.resolution_u=1;curve.bevel_depth=radius;curve.bevel_resolution=2
    spline=curve.splines.new('POLY');spline.points.add(len(points)-1)
    for q,p in zip(spline.points,points):q.co=(*p,1)
    obj=bpy.data.objects.new(name,curve);scene.collection.objects.link(obj);bpy.context.view_layer.objects.active=obj;obj.select_set(True)
    bpy.ops.object.convert(target='MESH');return register(bpy.context.object,group,material)
def ellipsoid(name,p,size,material,group='RoomPlants'):
    bpy.ops.mesh.primitive_uv_sphere_add(segments=20,ring_count=12,location=p);o=bpy.context.object;o.name=name;o.scale=size;bpy.ops.object.transform_apply(location=False,rotation=False,scale=True)
    for f in o.data.polygons:f.use_smooth=True
    return register(o,group,material)

# A complete room shell, including return walls/ceiling, avoids the photography-set void.
box('Floor foundation',(0,0,-10),(1020,1020,18),'Oak','RoomShell',0)
for row in range(62):
    y=-488+row*16
    start=-500-(row%3)*74
    while start<500:
        end=min(500,start+220);left=max(-500,start)
        if end>left:box('Staggered white-oak plank',((end+left)/2,y,.25),(end-left-.16,15.85,1.5),'Oak','RoomFloor',.12)
        start+=220
box('Ceiling',(0,0,367),(1020,1020,16),'Plaster','RoomShell',.5)
box('Sofa wall',(500,0,180),(20,1020,360),'Plaster','RoomShell',.7)
for p,size in [((-180,500,180),(640,20,360)),((420,500,180),(160,20,360)),((230,500,49),(180,20,98)),((230,500,334),(180,20,52)),
               ((-500, -385,180),(20,230,360)),((-500,180,180),(20,640,360)),((-500,-200,310),(20,140,100)),
               ((-355,-500,180),(290,20,360)),((355,-500,180),(290,20,360)),((0,-500,44),(420,20,88)),((0,-500,337),(420,20,46))]:
    box('Limewash architecture',p,size,'Plaster','RoomShell',.6)
for axis in (0,1):
    for sign in (-1,1):
        p=[0,0,9];p[axis]=sign*485;size=[975,975,18];size[axis]=3.5
        box('Painted timber skirting',p,size,'Paint','RoomDetails',.6)
        p[2]=352;size[2]=9;box('Ceiling cornice',p,size,'Paint','RoomDetails',.5)
for y,cx,width,bottom,top in [(480,230,180,98,310),(-480,0,420,88,314)]:
    for x in (cx-width/2,cx+width/2,cx):box('Window vertical frame',(x,y,(top+bottom)/2),(5,9,top-bottom),'Paint','RoomDetails',.4)
    for z in (bottom,top,(bottom+top)/2):box('Window crossbar',(cx,y,z),(width+10,9,5),'Paint','RoomDetails',.4)
    box('Deep window ledge',(cx,y,bottom-3),(width+20,36,6),'Oak','RoomDetails',.5)
    # Pleated opaque curtains are explicit cloth geometry, not flat hanging boxes.
    for side in (-1,1):
        verts=[];faces=[];columns=22;rows=28
        for j in range(rows):
            z=top+12-(top-bottom+22)*j/(rows-1)
            for i in range(columns):
                x=cx+side*(width/2+16)+(i/(columns-1)-.5)*52
                verts.append((x,y+(6*math.sin(i/(columns-1)*math.tau*4))+(3*math.sin(j*.16)),z))
        for j in range(rows-1):
            for i in range(columns-1):a=j*columns+i;faces.append((a,a+1,a+1+columns,a+columns))
        me=bpy.data.meshes.new('Pleated linen');me.from_pydata(verts,[],faces);me.update();o=bpy.data.objects.new('Window linen drape',me);scene.collection.objects.link(o)
        register(o,'RoomSoft','IvoryLinen')
        for f in me.polygons:f.use_smooth=True

# Panelled blue door retains the existing interaction point and invisible boundary collision.
box('Panelled outside door',(-483,-200,111),(6,108,222),'BluePaint','RoomDetails',.7)
for y in (-229,-171):
    for z in (55,150):box('Recessed door panel',(-478,y,z),(2,44,72),'BluePaint','RoomDetails',.8)
for y in (-261,-139):box('Door architrave',(-475,y,115),(12,10,230),'Paint','RoomDetails',.7)
box('Door lintel',(-475,-200,233),(12,132,12),'Paint','RoomDetails',.7)
ellipsoid('Brass door knob',(-470,-163,112),(3,3,3),'Brass','RoomDetails')

# Long four-seat sofa fitted to the tested seat/arm/back collision envelopes.
box('Sofa lower upholstered frame',(340,-15,30),(170,426,30),'SofaLinen','RoomSofa',4)
for x in (280,400):
    for y in (-200,170):cylinder('Tapered oak sofa foot',(x,y,9),4.5,16,'Oak','RoomSofa')
for y in (-165,-65,35,135):
    box('Individual seat cushion',(336,y,59),(166,98,28),'SofaLinen','RoomSofa',6)
    box('Soft separate back cushion',(402,y,110),(38,98,91),'SofaLinen','RoomSofa',7,rot=(0,math.radians(-5),0))
    # Piping follows a rounded rectangular seat, visible at pet-eye height.
    pts=[(257,y-43,59),(257,y+43,59),(262,y+47,59),(411,y+47,59),(416,y+43,59),(416,y-43,59),(411,y-47,59),(262,y-47,59),(257,y-43,59)]
    tube('Cushion edge piping',pts,.48,'SofaLinen','RoomSofa')
for y in (-230,200):box('Rounded upholstered arm',(340,y,75),(190,38,79),'SofaLinen','RoomSofa',7)
for y,mat,angle in [(-150,'OchreLinen',.18),(120,'IvoryLinen',-.15)]:
    box('Sewn throw pillow',(372,y,113),(22,66,62),mat,'RoomSoft',9,rot=(angle,.15,.05))
    ellipsoid('Pillow tuft',(358,y,113),(1.1,2,2),mat,'RoomSoft')

# Woven rug at the original play-area height, with a border and short tassels.
box('Bound wool area rug',(25,-25,3),(480,620,3),'RugWool','RoomSoft',1.2)
for x in (-207,257):box('Woven rug border',(x,-25,4.7),(9,610,.15),'IvoryLinen','RoomSoft',0)
for y in (-326,276):
    for x in range(-205,255,5):tube('Hand-tied rug fringe',[(x,y,3.9),(x+rng.uniform(-1,1),y+(-7 if y<0 else 7),3.1)],.3,'IvoryLinen','RoomSoft')

# Table, plant, lamps, books and non-branded architectural details.
cylinder('Oak side table top',(300,320,67),50,8,'Oak','RoomDetails',64)
for angle in (0,2.094,4.189):
    tube('Splayed oak table leg',[(300+35*math.cos(angle),320+35*math.sin(angle),4),(300+25*math.cos(angle),320+25*math.sin(angle),64)],3,'Oak','RoomDetails')
cylinder('Glazed plant pot',(300,320,85),15.5,27,'Ceramic','RoomPlants')
cylinder('Pot soil',(300,320,99),13.5,1,'Bark','RoomPlants')
for i in range(18):
    angle=i*2.4;end=Vector((300+math.cos(angle)*rng.uniform(15,40),320+math.sin(angle)*rng.uniform(15,40),rng.uniform(125,170)))
    tube('Plant stem',[(300,320,99),end],.6,'Leaf','RoomPlants')
    leaf=ellipsoid('Shaped indoor leaf',end,(5,13,.65),'Leaf','RoomPlants');leaf.rotation_euler=(.3,rng.random(),angle)
for i in range(3):box('Unlabelled cloth-bound book',(255+i*2,335,74+i*3),(25,34,2.7),['BluePaint','IvoryLinen','OchreLinen'][i],'RoomDetails',.35,rot=(0,0,i*.15))
cylinder('Floor lamp foot',(435,-340,5),21,5,'DarkMetal')
cylinder('Floor lamp stem',(435,-340,118),1.8,225,'Brass')
bpy.ops.mesh.primitive_cone_add(vertices=48,radius1=30,radius2=20,depth=40,end_fill_type='NOTHING',location=(435,-340,226));register(bpy.context.object,'RoomDetails','IvoryLinen')
for y in (-220,-20,180):
    box('Oak picture frame',(484,y,230),(6,65,86),'Oak','RoomDetails',.5)
    box('Original abstract relief',(480,y,230),(1,55,76),'IvoryLinen','RoomDetails',.1)
    for k in range(3):ellipsoid('Original raised botanical relief',(479,y+(k-1)*14,224+k*11),(.7,9,17),'Leaf','RoomDetails')
for y in (-360,360):box('Electrical faceplate',(483,y,40),(2,10,14),'Paint','RoomDetails',.5)

# Original leafy tree and slatted bench, reusable as static/instanced outdoor assets.
tube('Tree branching trunk',[(0,0,0),(8,2,150),(-3,0,300),(4,8,460)],16,'Bark','LeafTree')
for i in range(12):
    a=i*2.4;end=Vector((math.cos(a)*rng.uniform(100,200),math.sin(a)*rng.uniform(100,200),rng.uniform(370,640)))
    tube('Natural branch',[(0,0,230+i*10),end*.8,end],4,'Bark','LeafTree')
    for j in range(24):
        p=end+Vector((rng.gauss(0,47),rng.gauss(0,47),rng.gauss(0,40)))
        leaf=ellipsoid('Leaf cluster',p,(rng.uniform(16,29),rng.uniform(9,18),rng.uniform(4,9)),'Leaf','LeafTree');leaf.rotation_euler=(rng.uniform(-1,1),rng.uniform(-1,1),rng.random()*math.tau)
for y in (-17,-6,5,16):box('Bench seat slat',(0,y,50),(180,9,6),'Oak','ParkBench',.7)
for z in (70,82,94):box('Bench back slat',(0,23,z),(180,6,9),'Oak','ParkBench',.7)
for x in (-66,66):
    box('Bench metal support',(x,0,25),(6,46,50),'DarkMetal','ParkBench',.8)
    box('Bench back upright',(x,25,72),(5,5,62),'DarkMetal','ParkBench',.7)
for i in range(18):
    a=rng.random()*math.tau;r=rng.uniform(0,11);h=rng.uniform(10,25)
    tube('Grass blade',[(math.cos(a)*r,math.sin(a)*r,0),(math.cos(a)*(r+2),math.sin(a)*(r+2),h*.7),(math.cos(a)*(r+5),math.sin(a)*(r+5),h)],.3,'Grass','GrassClump')

manifest={'author':'Codename .... Cheweenie original procedural environment art','units':'centimetres','materials':{},'meshes':{},'photographs_used':False,'third_party_assets':False}
for name,(c,r,kind,scale) in SPEC.items():manifest['materials'][name]={'color':c,'roughness':r,'kind':kind,'tile_cm':scale,'maps':kind!='metal'}
for name,objects in groups.items():
    bpy.ops.object.select_all(action='DESELECT')
    for o in objects:o.select_set(True)
    bpy.context.view_layer.objects.active=objects[0];bpy.ops.object.join();o=bpy.context.object;o.name=name
    bpy.ops.object.transform_apply(location=True,rotation=True,scale=True)
    uv=o.data.uv_layers.active or o.data.uv_layers.new(name='SurfaceUV')
    for poly in o.data.polygons:
        ma=o.data.materials[poly.material_index].name;tile=SPEC[ma][3];n=poly.normal
        for li in poly.loop_indices:
            p=o.data.vertices[o.data.loops[li].vertex_index].co
            a,b=(p.x,p.y) if abs(n.z)>.7 else (p.y,p.z) if abs(n.x)>abs(n.y) else (p.x,p.z)
            uv.data[li].uv=(a/tile,b/tile)
    bpy.ops.export_scene.fbx(filepath=str(OUT/(name+'.fbx')),use_selection=True,object_types={'MESH'},axis_forward='-Y',axis_up='Z',apply_unit_scale=True,use_mesh_modifiers=True,bake_anim=False)
    manifest['meshes'][name]={'vertices':len(o.data.vertices),'triangles':sum(len(p.vertices)-2 for p in o.data.polygons),'materials':[m.name for m in o.data.materials]}
bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'WorldRealism.blend'),compress=True)
manifest['generator_sha256']=hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest()
(OUT/'manifest.json').write_text(json.dumps(manifest,indent=2))
print('CHEWEENIE_WORLD_ART_CREATED',manifest['meshes'])
exec(compile((ROOT/'Scripts'/'build_world_fort.py').read_text(),str(ROOT/'Scripts'/'build_world_fort.py'),'exec'),globals())
exec(compile((ROOT/'Scripts'/'build_world_outdoor.py').read_text(),str(ROOT/'Scripts'/'build_world_outdoor.py'),'exec'),globals())
exec(compile((ROOT/'Scripts'/'refine_world_surfaces.py').read_text(),str(ROOT/'Scripts'/'refine_world_surfaces.py'),'exec'),globals())
exec(compile((ROOT/'Scripts'/'build_world_kitchen.py').read_text(),str(ROOT/'Scripts'/'build_world_kitchen.py'),'exec'),globals())
