"""Original panelled cabinets, brushed fridge, sink/tap and TV surround."""
import bpy, pathlib, math, json, hashlib
ROOT=pathlib.Path(bpy.data.filepath).resolve().parents[2];OUT=ROOT/'SourceArt'/'WorldRealism';manifest=json.loads((OUT/'manifest.json').read_text());parts=[]
old=bpy.data.objects.get('RoomKitchen')
if old:bpy.data.objects.remove(old,do_unlink=True)
steel=bpy.data.materials.get('Steel') or bpy.data.materials.new('Steel');steel.diffuse_color=(.62,.66,.70,1)
manifest['materials']['Steel']={'color':[.62,.66,.70],'roughness':.24,'kind':'metal','tile_cm':100,'maps':False}
def box(name,p,size,mat,radius=1):
    bpy.ops.mesh.primitive_cube_add(size=1,location=p);o=bpy.context.object;o.name=name;o.scale=size;bpy.ops.object.transform_apply(location=False,rotation=False,scale=True);o.data.materials.append(bpy.data.materials[mat])
    b=o.modifiers.new('Rounded manufactured edges','BEVEL');b.width=radius;b.segments=5;bpy.ops.object.modifier_apply(modifier=b.name)
    for poly in o.data.polygons:poly.use_smooth=True
    w=o.modifiers.new('Weighted cabinet normals','WEIGHTED_NORMAL');w.keep_sharp=True;bpy.ops.object.modifier_apply(modifier=w.name);parts.append(o);return o
box('Kitchen cabinet carcass',(-250,425,41),(145,65,82),'Paint',.7)
box('Solid oak counter',(-250,425,85),(151,70,5),'Oak',1)
for x in (-298,-250,-202):
    box('Separate cabinet front',(x,390,43),(45,5,74),'BluePaint',.8)
    box('Recessed painted cabinet panel',(x,386.8,43),(35,1.5,61),'BluePaint',.5)
    box('Brass cabinet pull',(x,383.5,70),(18,2,1.5),'Brass',.7)
box('Kitchen plinth',(-250,400,4),(139,10,8),'DarkMetal',.5)
box('Inset sink rim',(-250,425,88),(47,35,2),'Steel',3)
box('Sink basin interior',(-250,425,89),(39,27,1),'DarkMetal',3)
box('Brushed fridge body',(-100,425,80),(70,65,160),'Steel',2.5)
for z,h in ((58,108),(137,45)):
    box('Separate fridge door',(-100,389.5,z),(66,5,h),'Steel',2)
    box('Recessed fridge handle',(-125,385,z),(2,3,h*.45),'DarkMetal',.7)
for x in (-129,-71):box('Adjustable refrigerator foot',(x,405,2),(5,8,4),'DarkMetal',.4)
# Bent metal tap, hand-authored as a tube rather than a primitive ball cluster.
curve=bpy.data.curves.new('Gooseneck sink tap','CURVE');curve.dimensions='3D';curve.bevel_depth=1.4;curve.bevel_resolution=3
spline=curve.splines.new('BEZIER');spline.bezier_points.add(3)
for p,co in zip(spline.bezier_points,[(-250,443,88),(-250,443,110),(-250,432,116),(-250,424,107)]):p.co=co;p.handle_left_type=p.handle_right_type='AUTO'
o=bpy.data.objects.new('Gooseneck sink tap',curve);bpy.context.scene.collection.objects.link(o);bpy.context.view_layer.objects.active=o;bpy.ops.object.select_all(action='DESELECT');o.select_set(True);bpy.ops.object.convert(target='MESH');o=bpy.context.object;o.data.materials.append(steel);parts.append(o)
box('TV dark surround',(-459,100,175),(8,240,135),'DarkMetal',2)
box('TV glass panel',(-454,100,175),(1,229,123),'DarkMetal',.8)
bpy.ops.object.select_all(action='DESELECT')
for o in parts:o.select_set(True)
bpy.context.view_layer.objects.active=parts[0];bpy.ops.object.join();o=bpy.context.object;o.name='RoomKitchen';bpy.ops.object.transform_apply(location=True,rotation=True,scale=True)
uv=o.data.uv_layers.active or o.data.uv_layers.new(name='SurfaceUV')
for poly in o.data.polygons:
    n=poly.normal
    for li in poly.loop_indices:
        p=o.data.vertices[o.data.loops[li].vertex_index].co;a,b=(p.x,p.y) if abs(n.z)>.7 else (p.y,p.z) if abs(n.x)>abs(n.y) else (p.x,p.z);uv.data[li].uv=(a/100,b/100)
bpy.ops.export_scene.fbx(filepath=str(OUT/'RoomKitchen.fbx'),use_selection=True,object_types={'MESH'},axis_forward='-Y',axis_up='Z',apply_unit_scale=True,use_mesh_modifiers=True,bake_anim=False)
bpy.context.preferences.filepaths.save_version=0;bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'WorldRealism.blend'),compress=True)
manifest['meshes']['RoomKitchen']={'vertices':len(o.data.vertices),'triangles':sum(len(p.vertices)-2 for p in o.data.polygons),'materials':[m.name for m in o.data.materials]}
manifest['kitchen_generator_sha256']=hashlib.sha256((ROOT/'Scripts'/'build_world_kitchen.py').read_bytes()).hexdigest();manifest['generator_sha256']=hashlib.sha256((ROOT/'Scripts'/'build_world_art.py').read_bytes()).hexdigest();(OUT/'manifest.json').write_text(json.dumps(manifest,indent=2));print('CHEWEENIE_KITCHEN_ART_CREATED')
