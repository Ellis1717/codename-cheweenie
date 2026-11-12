"""Original smooth, weighted quadrupeds for the cumulative Unreal game. Blender 5, cm.
Preserves Marty and the comparison/Unity cats. No downloaded meshes or add-ons.
"""
import bpy, bmesh, math, os, json, hashlib
from mathutils import Vector
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT=os.path.join(ROOT,'SourceArt','CompanionRigs');os.makedirs(OUT,exist_ok=True)
manifest={}
for profile in ['Bauer','Hank','Potcake','Lakitu','Rio']:
    cat=profile in ['Lakitu','Rio'];fluffy=profile in ['Bauer','Hank']
    bpy.ops.object.select_all(action='SELECT');bpy.ops.object.delete(use_global=False)
    scene=bpy.context.scene;scene.unit_settings.system='METRIC';scene.unit_settings.scale_length=.01
    parts=[];bones={};height=23 if cat else 31
    def bone(n,a,b,parent=None):bones[n]=(tuple(a),tuple(b),parent)
    bone('root',(0,0,0),(0,0,5));bone('pelvis',(-16,0,height),(-6,0,height+1),'root')
    bone('spine',(-6,0,height+1),(7,0,height+2),'pelvis');bone('chest',(7,0,height+2),(16,0,height+3),'spine')
    bone('neck',(16,0,height+3),(20,0,height+10),'chest');bone('head',(20,0,height+10),(28,0,height+12),'neck')
    for i in range(3):bone('tail' if i==0 else 'tail_mid' if i==1 else 'tail_tip',(-24-i*7,0,height+2+i*3),(-31-i*7,0,height+5+i*3),'pelvis' if i==0 else 'tail' if i==1 else 'tail_mid')
    def mat(n,c,r=.78):
        m=bpy.data.materials.new(n);m.diffuse_color=(*c,1);m.use_nodes=True
        p=m.node_tree.nodes.get('Principled BSDF');p.inputs['Base Color'].default_value=(*c,1);p.inputs['Roughness'].default_value=r;return m
    main=(.56,.32,.12) if profile=='Bauer' else (.025,.029,.035) if profile in ['Hank','Rio'] else (.77,.76,.70) if cat else (.34,.32,.28)
    coat=mat('Coat',main);white=mat('White',(.77,.76,.70));dark=mat('Dark',(.012,.016,.022))
    nose=mat('Nose',(.40,.15,.17) if cat else (.012,.009,.008),.34)
    iris=mat('Iris',(.25,.34,.10) if cat else (.16,.075,.025),.24);pupil=mat('Pupil',(.002,.003,.004),.2)
    pink=mat('EarInner',(.34,.15,.16));glint=mat('Glint',(.9,.9,.86),.15)
    def weighted(o,weights):
        for n,w in weights.items():o.vertex_groups.new(name=n).add(list(range(len(o.data.vertices))),w,'REPLACE')
    def oval(n,p,s,m,bind='spine'):
        bpy.ops.mesh.primitive_uv_sphere_add(segments=28,ring_count=18,location=p)
        o=bpy.context.object;o.name=n;o.scale=s;bpy.ops.object.transform_apply(location=False,rotation=False,scale=True)
        o.data.materials.append(m);weighted(o,{bind:1})
        for f in o.data.polygons:f.use_smooth=True
        parts.append(o);return o
    shell=[]
    for n,p,s in [('Ribs',(-4,0,height),(20,7.3 if cat else 9,8 if cat else 11)),('Rump',(-18,0,height-1),(10,7 if cat else 8,9)),('Chest',(11,0,height+1),(9,7.5 if cat else 10,10 if cat else 13)),('Neck',(17,0,height+7),(6.5,5.4 if cat else 7,10)),('Skull',(24,0,height+14),(8,6.8,8.4)),('Muzzle',(30 if cat else 33,0,height+10),(4.5 if cat else 9,4.5,3.8)),('Jaw',(29 if cat else 32,0,height+7),(5 if cat else 8,4.2,2.5))]:shell.append(oval(n,p,s,coat))
    bpy.ops.object.select_all(action='DESELECT')
    for o in shell:o.select_set(True)
    bpy.context.view_layer.objects.active=shell[0];bpy.ops.object.join();body=shell[0]
    rem=body.modifiers.new('Continuous anatomy','REMESH');rem.mode='VOXEL';rem.voxel_size=.65;rem.use_smooth_shade=True;bpy.ops.object.modifier_apply(modifier=rem.name)
    sm=body.modifiers.new('Soft tissue','SMOOTH');sm.factor=1.1;sm.iterations=5;bpy.ops.object.modifier_apply(modifier=sm.name)
    parts=[body];body.vertex_groups.clear()
    groups={n:body.vertex_groups.new(name=n) for n in ['pelvis','spine','chest','neck','head']}
    anchors=[(-18,'pelvis'),(-4,'spine'),(10,'chest'),(17,'neck'),(24,'head')]
    for v in body.data.vertices:
        p=body.matrix_world@v.co;x=p.x
        if x<=anchors[0][0]:ws={'pelvis':1}
        elif x>=24:ws={'head':1}
        else:
            for (a,na),(b,nb) in zip(anchors,anchors[1:]):
                if a<=x<=b:t=(x-a)/(b-a);t=t*t*(3-2*t);ws={na:1-t,nb:t};break
        for n,w in ws.items():groups[n].add([v.index],w,'REPLACE')
    body.data.materials.append(white);body.data.materials.append(dark)
    for f in body.data.polygons:
        c=body.matrix_world@f.center
        if profile=='Lakitu':f.material_index=2 if (c.x<11 and c.z>height+2) or (c.x>22 and c.y<-.7 and c.z>height+12) else 1
        elif profile in ['Rio','Hank']:f.material_index=1 if (8<c.x<24 and c.z<height-1) or (profile=='Rio' and c.x>26 and c.z<height+7) else 0
    for side in [-1,1]:
        en='ear_l' if side<0 else 'ear_r';base=Vector((21,side*5.5,height+19))
        tip=base+Vector((-1,side*2.5,9 if cat else -13));bone(en,base,tip,'head')
        if cat:
            verts=[tuple(base+Vector((-4,0,0))),tuple(base+Vector((4,0,0))),tuple(tip),tuple(base+Vector((0,side*2,-1)))]
            me=bpy.data.meshes.new(en);me.from_pydata(verts,[],[(0,1,2),(0,2,3),(1,3,2),(0,3,1)]);me.update()
            o=bpy.data.objects.new(en,me);scene.collection.objects.link(o);o.data.materials.append(dark);weighted(o,{en:1});parts.append(o)
            oval('Inner ear',tuple(base+Vector((.7,-side*.5,2))),(.9,1,3),pink,en)
        else:
            o=oval('Soft hanging ear',tuple((base+tip)*.5),(3.8,1.7,8),coat,en)
        oval('Eye socket',(28,side*5.5,height+15),(2.6,1.2,2.6),dark,'head')
        oval('Iris',(29.1,side*6.1,height+15),(1.8,.7,1.9),iris,'head')
        oval('Pupil',(29.6,side*6.5,height+15),(.45 if cat else .8,.35,1.5),pupil,'head')
        oval('Eye catchlight',(30,side*6.65,height+15.6),(.35,.2,.35),glint,'head')
    oval('Nose',(34 if cat else 40,0,height+11),(1 if cat else 1.9,2 if cat else 3,1.3 if cat else 2),nose,'head')
    if cat:
        for side in [-1,1]:
            for i in range(3):
                # Tapered whiskers, not detached spheres.
                a=Vector((31,side*3,height+10-i*.65));b=a+Vector((-2,side*(8+i),1-i))
                bpy.ops.mesh.primitive_cone_add(vertices=6,radius1=.12,radius2=.015,depth=(b-a).length,location=(a+b)*.5)
                o=bpy.context.object;o.rotation_euler=(b-a).to_track_quat('Z','Y').to_euler();bpy.ops.object.transform_apply(location=False,rotation=True,scale=True);o.data.materials.append(white);weighted(o,{'head':1});parts.append(o)
    for front in [True,False]:
        for side in [-1,1]:
            key=('front' if front else 'hind')+('_l' if side<0 else '_r')
            pts=[Vector((13 if front else -18,side*(5.4 if cat else 6.5),height)),Vector((10 if front else -10,side*6.2,height*.53)),Vector((13 if front else -20,side*6.5,5.4)),Vector((16 if front else -17,side*6.5,2))]
            ns=[key+x for x in ['_upper','_lower','_hock','_paw']]
            for i in range(4):bone(ns[i],pts[i],pts[i+1] if i<3 else pts[3]+Vector((3,0,0)),'chest' if front and i==0 else 'pelvis' if i==0 else ns[i-1])
            vs=[];ws=[];faces=[];sides=16;rings=7
            for i in range(3):
                for j in range(rings):
                    t=j/(rings-1);c=pts[i].lerp(pts[i+1],t);r=(2.6 if cat else 3.8)*(1-.45*(i+t)/3)
                    for k in range(sides):
                        a=k*math.tau/sides;vs.append(tuple(c+Vector((r*math.cos(a),r*.85*math.sin(a),0))))
                        prev=max(0,(.4-t)/.4)*.45 if i else 0;nxt=max(0,(t-.6)/.4)*.45
                        w={ns[i]:1-prev-nxt,ns[min(i+1,3)]:nxt}
                        if i:w[ns[i-1]]=prev
                        if i==0:
                            blend=max(0,1-t*2);w={n:v*(1-blend) for n,v in w.items()};w['chest' if front else 'pelvis']=blend
                        ws.append(w)
            for j in range(len(vs)//sides-1):
                for k in range(sides):a=j*sides+k;b=j*sides+(k+1)%sides;faces.append((a,b,b+sides,a+sides))
            me=bpy.data.meshes.new(key);me.from_pydata(vs,[],faces);me.update();o=bpy.data.objects.new(key,me);scene.collection.objects.link(o);o.data.materials.append(coat)
            gs={n:o.vertex_groups.new(name=n) for n in ns+['chest','pelvis']}
            for j,w in enumerate(ws):
                for n,v in w.items():
                    if v:gs[n].add([j],v,'REPLACE')
            for f in me.polygons:f.use_smooth=True
            parts.append(o);oval(key+' paw',tuple(pts[3]+Vector((1,0,0))),(3.6,2.6,1.7),white if profile in ['Lakitu','Rio','Hank'] else coat,ns[3])
    # A smoothly weighted tail with three independently controlled joints.
    vs=[];ws=[];faces=[]
    for j in range(25):
        t=j/24;c=Vector((-24-24*t,0,height+2+12*t));r=(1.9 if cat else 2.8)*(1-t)+.15
        for k in range(12):
            a=k*math.tau/12;vs.append(tuple(c+Vector((0,math.cos(a)*r,math.sin(a)*r))))
            u=t*2;idx=min(1,int(u));f=u-idx;ns=['tail','tail_mid','tail_tip'];ws.append({ns[idx]:1-f,ns[idx+1]:f})
    for j in range(24):
        for k in range(12):a=j*12+k;b=j*12+(k+1)%12;faces.append((a,b,b+12,a+12))
    me=bpy.data.meshes.new('Tail');me.from_pydata(vs,[],faces);me.update();o=bpy.data.objects.new('Tail',me);scene.collection.objects.link(o);o.data.materials.append(dark if profile=='Lakitu' else coat)
    gs={n:o.vertex_groups.new(name=n) for n in ['tail','tail_mid','tail_tip']}
    for j,w in enumerate(ws):
        for n,v in w.items():gs[n].add([j],v,'REPLACE')
    for f in me.polygons:f.use_smooth=True
    parts.append(o)
    bpy.ops.object.select_all(action='DESELECT')
    for o in parts:o.select_set(True)
    bpy.context.view_layer.objects.active=body;bpy.ops.object.join();pet=body;pet.name=profile+'_Rig';bpy.ops.object.transform_apply(location=True,rotation=True,scale=True)
    bm=bmesh.new();bm.from_mesh(pet.data);bmesh.ops.remove_doubles(bm,verts=list(bm.verts),dist=.00001);bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces));bm.to_mesh(pet.data);bm.free()
    uv=pet.data.uv_layers.active or pet.data.uv_layers.new(name='CoatUV')
    for f in pet.data.polygons:
        for li in f.loop_indices:
            p=pet.data.vertices[pet.data.loops[li].vertex_index].co;uv.data[li].uv=((p.x+50)/100,math.atan2(p.y,p.z-height)/math.tau+.5)
    bpy.ops.object.armature_add(enter_editmode=True);rig=bpy.context.object;rig.name=profile+'_Anatomy';rig.data.edit_bones.remove(rig.data.edit_bones[0])
    for n,(a,b,parent) in bones.items():
        eb=rig.data.edit_bones.new(n);eb.head=a;eb.tail=b
        if parent:eb.parent=rig.data.edit_bones[parent]
    bpy.ops.object.mode_set(mode='OBJECT');mod=pet.modifiers.new('Weighted skin','ARMATURE');mod.object=rig;pet.parent=rig
    bpy.ops.object.select_all(action='DESELECT');pet.select_set(True);rig.select_set(True);bpy.context.view_layer.objects.active=rig
    bpy.ops.export_scene.fbx(filepath=os.path.join(OUT,profile+'_Rig.fbx'),use_selection=True,object_types={'ARMATURE','MESH'},add_leaf_bones=False,bake_anim=False,axis_forward='-Y',axis_up='Z',apply_unit_scale=True,use_mesh_modifiers=True)
    manifest[profile]={'bones':list(bones),'vertices':len(pet.data.vertices),'materials':{m.name.split('.')[0]:list(m.diffuse_color) for m in pet.data.materials},'species':'cat' if cat else 'dog'}
with open(os.path.join(OUT,'manifest.json'),'w') as f:json.dump(manifest,f,indent=2)
print('COMPANION_RIGS_EXPORTED',len(manifest))
