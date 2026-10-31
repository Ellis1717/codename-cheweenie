"""Final deterministic smooth upholstery normals and subtle, irregular plaster maps."""
import bpy, pathlib, json, hashlib
import numpy as np
ROOT=pathlib.Path(bpy.data.filepath).resolve().parents[2];OUT=ROOT/'SourceArt'/'WorldRealism';manifest=json.loads((OUT/'manifest.json').read_text())
for name in ('RoomSofa','RoomSoft'):
    obj=bpy.data.objects[name];bpy.ops.object.select_all(action='DESELECT');obj.select_set(True);bpy.context.view_layer.objects.active=obj
    for p in obj.data.polygons:p.use_smooth=True
    modifier=obj.modifiers.new('Weighted soft furniture normals','WEIGHTED_NORMAL');modifier.keep_sharp=True;modifier.weight=75;bpy.ops.object.modifier_apply(modifier=modifier.name)
    bpy.ops.export_scene.fbx(filepath=str(OUT/(name+'.fbx')),use_selection=True,object_types={'MESH'},axis_forward='-Y',axis_up='Z',apply_unit_scale=True,use_mesh_modifiers=True,bake_anim=False)
n=1024;y,x=np.mgrid[0:n,0:n];rng=np.random.default_rng(2449)
def field(size):
    grid=rng.random((size,size))-.5;gx=x*size/n;gy=y*size/n;ix=gx.astype(int);iy=gy.astype(int);u=gx-ix;v=gy-iy;u=u*u*(3-2*u);v=v*v*(3-2*v)
    return (grid[iy%size,ix%size]*(1-u)+grid[iy%size,(ix+1)%size]*u)*(1-v)+(grid[(iy+1)%size,ix%size]*(1-u)+grid[(iy+1)%size,(ix+1)%size]*u)*v
for name in ('Plaster','Paint','BluePaint'):
    c=np.array(manifest['materials'][name]['color']);rough=manifest['materials'][name]['roughness'];variation=field(13)*.018+(rng.random((n,n))-.5)*.008;h=field(190)*.02
    dx=(np.roll(h,-1,1)-np.roll(h,1,1));dy=(np.roll(h,-1,0)-np.roll(h,1,0));normal=np.stack((-dx,dy,np.ones_like(h)),axis=-1);normal/=np.linalg.norm(normal,axis=-1,keepdims=True)
    for role,array in [('Base',np.clip(c[None,None,:]*(1+variation[:,:,None]),0,1)),('Normal',normal*.5+.5),('Rough',np.repeat((rough+variation*.1)[:,:,None],3,axis=2))]:
        image=bpy.data.images.new(name+'_'+role,width=n,height=n,alpha=True);image.colorspace_settings.name='Non-Color';image.pixels.foreach_set(np.concatenate((array,np.ones((n,n,1))),axis=-1).astype(np.float32).ravel());image.filepath_raw=str(OUT/(name+'_'+role+'.png'));image.file_format='PNG';image.save();bpy.data.images.remove(image)
bpy.context.preferences.filepaths.save_version=0;bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'WorldRealism.blend'),compress=True)
manifest['surface_generator_sha256']=hashlib.sha256((ROOT/'Scripts'/'refine_world_surfaces.py').read_bytes()).hexdigest();manifest['generator_sha256']=hashlib.sha256((ROOT/'Scripts'/'build_world_art.py').read_bytes()).hexdigest();(OUT/'manifest.json').write_text(json.dumps(manifest,indent=2));print('CHEWEENIE_WORLD_SURFACES_REFINED')
