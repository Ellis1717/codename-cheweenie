"""Add an original draped blanket fort to the retained generated room source."""
import bpy, pathlib, math, json, hashlib
ROOT=pathlib.Path(bpy.data.filepath).resolve().parents[2]
OUT=ROOT/'SourceArt'/'WorldRealism'
if not bpy.data.filepath.endswith('WorldRealism.blend'):raise RuntimeError('Open generated WorldRealism.blend first')
old=bpy.data.objects.get('FortBlanket')
if old:bpy.data.objects.remove(old,do_unlink=True)
parts=[]
def sheet(name,point,nu,nv):
    vertices=[point(i/(nu-1),j/(nv-1)) for j in range(nv) for i in range(nu)];faces=[]
    for j in range(nv-1):
        for i in range(nu-1):a=j*nu+i;faces.append((a,a+1,a+1+nu,a+nu))
    mesh=bpy.data.meshes.new(name);mesh.from_pydata(vertices,[],faces);mesh.update();mesh.materials.append(bpy.data.materials['RugWool'])
    obj=bpy.data.objects.new(name,mesh);bpy.context.scene.collection.objects.link(obj);parts.append(obj)
    for poly in mesh.polygons:poly.use_smooth=True
sheet('Naturally sagging blanket roof',lambda u,v:((u-.5)*178,(v-.5)*150,66-9*math.sin(u*math.pi)*math.sin(v*math.pi)+.6*math.sin(v*38)),45,38)
for side in (-1,1):
    sheet('Gathered blanket sides',lambda u,v:((u-.5)*178,side*(70+2.4*math.sin(u*32)*math.sin(v*math.pi)),65*(1-v)+1),45,28)
for x in (-82,82):
    for y in (-65,65):
        bpy.ops.mesh.primitive_cylinder_add(vertices=16,radius=1.8,depth=66,location=(x,y,33));o=bpy.context.object;o.data.materials.append(bpy.data.materials['Oak']);parts.append(o)
bpy.ops.object.select_all(action='DESELECT')
for p in parts:p.select_set(True)
bpy.context.view_layer.objects.active=parts[0];bpy.ops.object.join();o=bpy.context.object;o.name='FortBlanket';bpy.ops.object.transform_apply(location=True,rotation=True,scale=True)
uv=o.data.uv_layers.new(name='SurfaceUV')
for poly in o.data.polygons:
    n=poly.normal
    for li in poly.loop_indices:
        p=o.data.vertices[o.data.loops[li].vertex_index].co;uv.data[li].uv=(p.x/35,(p.y if abs(n.z)>.7 else p.z)/35)
bpy.ops.export_scene.fbx(filepath=str(OUT/'FortBlanket.fbx'),use_selection=True,object_types={'MESH'},axis_forward='-Y',axis_up='Z',apply_unit_scale=True,use_mesh_modifiers=True,bake_anim=False)
bpy.context.preferences.filepaths.save_version=0
bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'WorldRealism.blend'),compress=True)
manifest=json.loads((OUT/'manifest.json').read_text());manifest['meshes']['FortBlanket']={'vertices':len(o.data.vertices),'triangles':sum(len(p.vertices)-2 for p in o.data.polygons),'materials':[m.name for m in o.data.materials]}
manifest['fort_generator_sha256']=hashlib.sha256((ROOT/'Scripts'/'build_world_fort.py').read_bytes()).hexdigest();manifest['generator_sha256']=hashlib.sha256((ROOT/'Scripts'/'build_world_art.py').read_bytes()).hexdigest();(OUT/'manifest.json').write_text(json.dumps(manifest,indent=2));print('CHEWEENIE_DRAPED_FORT_CREATED')
