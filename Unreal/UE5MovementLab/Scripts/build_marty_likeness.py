"""Photo-guided Marty proportions; retain every earlier model and generator.

Blender 5 --background --factory-startup --python this_file
M01/A07 anchor face/coat; A05/A08 anchor folded ears. Dimensions are inferred.
"""
import pathlib,sys
script=pathlib.Path(__file__).resolve()
photo_rig=(script.parent/'build_marty_rig.py').read_text()
changes={
    "('Long ribcage',(-6,0,23),(21,8.3,10))":"('Long ribcage',(-7,0,23),(23.5,7.8,9.2))",
    "('Chest',(10,0,24),(10,8.6,11))":"('Chest',(10,0,24),(10,8.0,9.8))",
    "('Neck',(16,0,30),(8,7.1,11))":"('Neck',(15,0,29),(6.9,6.1,8.8))",
    "('Jaw',(29,0,30.6),(8,5.1,2.8))":"('Jaw',(30,0,30.9),(9,4.0,2.0))",
    "oval('Cream chin',(32,0,30),(6,4.8,1.5),cream,'head')":"# Cream underside is blended into the continuous skin palette.",
    "(37.1,0,33.6),(1.7,3.5,2.2)":"(39.4,0,33.0),(1.45,2.35,1.55)",
    "(38.5,side*1.8,33.6),(.35,.65,.5)":"(40.55,side*1.25,33.1),(.25,.40,.30)",
    'width=.3+3.6*math.sin(math.pi*t)**.6':'width=.50+2.25*math.sin(math.pi*t)**.65',
    '41-15*t':'39.8-7.5*t',
    '21-2*t':'21+1.5*t-2.2*math.sin(math.pi*t)',
    'bones[name]=((21,side*7,41)':'bones[name]=((21,side*7,39.8)',
    "(20,side*10,34),'head'":"(20,side*10,35.7),'head'",
    "((20,side*10,34),(19,side*12,26),name)":"((20,side*10,35.7),(19,side*12,32.3),name)",
    '(41-v.co.z)/4':'(39.8-v.co.z)/3',
    '(36-v.co.z)/7':'(36.7-v.co.z)/4.4',
    '(4.2,2.8,1.9)':'(3.2,2.4,1.65)',
    'Vector((4,toe*1.45,0))),(.7,.48,.45)':'Vector((3,toe*1.2,0))),(.55,.35,.32)',
}
for old,new in changes.items():
    assert old in photo_rig,old
    photo_rig=photo_rig.replace(old,new)
# A shaped dark nose with a subtly tapered underside, rather than a rubber ball.
photo_rig=photo_rig.replace("    o.data.materials.append(material); bind(o,bone)","""    if name=='Velvet nose':
        for v in o.data.vertices:v.co.y*=.78+.22*max(0,min(1,(v.co.z+1.55)/3.1))
    o.data.materials.append(material); bind(o,bone)""")

# Lower the stance with a smooth monotone warp. Apply it to the finished skin,
# fur normals and reference joints together, after colour/UV authoring.
warp="""
def stance_z(z):
    t=max(0,min(1,(z-4)/20));return z-4.2*t*t*(3-2*t)
def stance_derivative(z):
    t=max(0,min(1,(z-4)/20));return 1-1.26*t*(1-t)
normal_values=[]
for loop in pet.data.loops:
    z=pet.data.vertices[loop.vertex_index].co.z;n=pet.data.corner_normals[loop.index].vector.copy()
    n.z/=stance_derivative(z);normal_values.append(tuple(n.normalized()))
for v in pet.data.vertices:v.co.z=stance_z(v.co.z)
pet.data.update();pet.data.normals_split_custom_set(normal_values)
bones={name:((a[0],a[1],stance_z(a[2])),(b[0],b[1],stance_z(b[2])),parent) for name,(a,b,parent) in bones.items()}
"""
anchor='for name,(a,b,parent) in bones.items():'
assert photo_rig.count(anchor)==1
photo_rig=photo_rig.replace(anchor,warp+'\n'+anchor)

photo_detail=(script.parent/'build_marty_realcoat.py').read_text()
detail_changes={
    "('Skull',(23,0,37),(9.4,7.5,9.2))":"('Skull',(22,0,35.8),(8.2,7.2,5.8))",
    "('Muzzle',(30,0,33),(9,5.0,4.3))":"('Muzzle',(30.4,0,32.5),(10.2,4.1,3.8))",
    '(28,side*5.8,38),(2.1,1.0,2.25)':'(27.5,side*4.95,36.8),(1.7,.65,1.9)',
    '(28.5,side*6.25,38),(1.8,.8,1.95)':'(28.0,side*5.25,36.85),(1.42,.55,1.5)',
    '(29.1,side*6.7,38.1),(1.0,.38,1.35)':'(28.35,side*5.7,36.85),(1.02,.22,1.17)',
    '7.55*math.cos(angle),29+9*math.sin(angle)':'6.6*math.cos(angle),29+7.8*math.sin(angle)',
    'Vector((.58,.31,.105))':'Vector((.60,.36,.16))',
    'Vector((.74,.59,.37))':'Vector((.78,.68,.49))',
    "if material == 'Marty_Cream': return (.72,.58,.36,1)":"if material == 'Marty_Cream': return (.78,.68,.49,1)",
    'sign*(3+i*.3)':'sign*(1.7+i*.18)',
    'sign*4.4,32.5+i*.35':'sign*3.8,32.5+i*.22',
    'radius1=.035,radius2=.003':'radius1=.012,radius2=.0015',
    'for axis in (side,across):':'for axis in (side,):',
    'side=direction.cross(Vector((0,0,1)))':'side=direction.cross(n)',
    'if side.length<.1:side=direction.cross(Vector((0,1,0)))':'if side.length<.01:side=n.orthogonal()',
    'for start in range(0,len(hair_vertices),8):':'for start in range(0,len(hair_vertices),4):',
    'list(range(start,start+8))':'list(range(start,start+4))',
    'hair_faces.extend([(start,start+2,start+1),(start+1,start+2,start+3)])':"hair_faces.extend([(start,start+1,start+2),(start+1,start+3,start+2)] if direction.cross(axis).dot(n)<0 else [(start,start+2,start+1),(start+1,start+2,start+3)])",
}
for old,new in detail_changes.items():
    assert old in photo_detail,old
    photo_detail=photo_detail.replace(old,new)
old="tag=oval('Blue tag without personal details',(15.3,-7.9,24.3),(1.5,.35,2.0),tagmat,'head');accessories.append(tag)"
assert old in photo_detail
photo_detail=photo_detail.replace(old,"""tag_center=Vector((20.5,0,20.7))
accessories.append(oval('Tag attachment link',(20.3,0,22.1),(.17,.40,.50),collar_white,'spine'))
accessories.append(oval('Blue bone tag bridge',tuple(tag_center),(.26,1.2,.32),tagmat,'spine'))
for y in (-1.15,1.15):
    for z in (-.32,.32):accessories.append(oval('Blue bone tag rounded lobe',tuple(tag_center+Vector((0,y,z))),(.26,.62,.55),tagmat,'spine'))""")
photo_detail=photo_detail.replace("mesh.materials.append(collar_black);mesh.materials.append(collar_white);bind(collar,'head')","""mesh.materials.append(collar_black);mesh.materials.append(collar_white);bind(collar,'head')
spine_group=collar.vertex_groups.new(name='spine');head_group=collar.vertex_groups['head']
for v in mesh.vertices:
    x=v.co.x-14.45;z=v.co.z-29;a=math.radians(35)
    v.co.x=14.45+x*math.cos(a)-z*math.sin(a);v.co.z=29+x*math.sin(a)+z*math.cos(a)
    w=max(0,min(1,(v.co.x-12)/9))*max(0,min(1,(v.co.z-23)/7))
    head_group.add([v.index],w,'REPLACE');spine_group.add([v.index],1-w,'REPLACE')""")
photo_detail=photo_detail.replace('accessories=[]',"""accessories=[]
from mathutils.bvhtree import BVHTree
lip_surface=BVHTree.FromPolygons([v.co for v in pet.data.vertices],[list(p.vertices) for p in pet.data.polygons])
for sign in (-1,1):
    line=bpy.data.curves.new('Closed natural lip edge','CURVE');line.dimensions='3D';line.bevel_depth=.055;line.bevel_resolution=2
    points=line.splines.new('POLY');points.points.add(11)
    for i,point in enumerate(points.points):
        x=26+i*(13.1/11);y=sign*4.1*math.sqrt(max(.05,1-((x-30.4)/10.2)**2))*.84
        hit,n,_,_=lip_surface.find_nearest(Vector((x,y,30.6)));point.co=(*(hit+n*.025),1)
    o=bpy.data.objects.new('Subtle closed lip',line);scene.collection.objects.link(o)
    bpy.ops.object.select_all(action='DESELECT');o.select_set(True);bpy.context.view_layer.objects.active=o
    bpy.ops.object.convert(target='MESH');o=bpy.context.object;o.data.materials.append(black);bind(o,'head');accessories.append(o)
""")
photo_detail=photo_detail.replace('for o in accessories:\n    if len(o.data.uv_layers)',"""for o in accessories:
    if o.name.startswith('Muzzle whisker'):
        color=o.data.color_attributes.new(name='CoatColor',type='FLOAT_COLOR',domain='CORNER')
        for entry in color.data:entry.color=(.23,.15,.08,1)
    if len(o.data.uv_layers)""")
photo_connected=(script.parent/'build_marty_connected.py').read_text()
photo_connected=photo_connected.replace("connected_base = (script.parent / 'build_marty_rig.py').read_text()",'connected_base=photo_rig')
photo_connected=photo_connected.replace('side*(5+7*math.sin(t*1.6))','side*(5+6*math.sin(t*1.6))')
photo_connected=photo_connected.replace('21-2*t','21+1.5*t-2.2*math.sin(math.pi*t)')
photo_connected=photo_connected.replace('25+11*t+math.sin(a)*radius','25+8*t+3*math.sin(math.pi*.8*t)+math.sin(a)*radius')
photo_connected=photo_connected.replace('col=Vector((.58,.31,.105))','col=Vector((.60,.36,.16))')
photo_connected=photo_connected.replace('.lerp(Vector((.30,.125,.042)),blend)','.lerp(Vector((.34,.17,.075)),blend)')
soft=(script.parent/'build_marty_softcoat.py').read_text()
soft=soft.replace("connected=(script.parent/'build_marty_connected.py').read_text()",'connected=photo_connected')
soft=soft.replace("source=(script.parent/'build_marty_realcoat.py').read_text()",'source=photo_detail')
soft=soft.replace("'SourceArt', 'MartySoftCoat'","'SourceArt', 'MartyPhotoReference'")
exec(compile(soft,str(script),'exec'),globals())
import json,hashlib
proof_path=script.parent.parent/'SourceArt'/'MartyPhotoReference'/'attachment-topology.json'
proof=json.loads(proof_path.read_text());proof.update(reference_bones_unchanged=False,reference_joint_names_and_hierarchy_unchanged=True,stance_lowering_cm=4.2)
proof_path.write_text(json.dumps(proof,indent=2))
manifest_path=proof_path.parent/'manifest.json';manifest=json.loads(manifest_path.read_text())
manifest.update(reference='Owner-confirmed pet photos M01/A07, A05/A08; pet-only observations',reference_policy='Inferred authoring proportions, not measured anatomy or photogrammetry',source_script_sha256=hashlib.sha256(script.read_bytes()).hexdigest(),ear_vertical_drop_cm=7.5,stance_lowering_cm=4.2,blue_tag='Rounded bone silhouette, no personal text')
manifest.update(technique='32000 surface-aligned outward-facing combed ribbons; filtered undercoat shading',fur_vertices_per_strand=4)
manifest_path.write_text(json.dumps(manifest,indent=2))
