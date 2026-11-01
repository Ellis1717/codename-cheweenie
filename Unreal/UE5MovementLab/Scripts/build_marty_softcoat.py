"""Denser, shorter, combed coat on the retained connected skin and 24-joint rig.

Blender 5 --background --factory-startup --python this_file
Original generators, models and texture maps remain intact.
"""
import pathlib, sys

script=pathlib.Path(__file__).resolve()
connected=(script.parent/'build_marty_connected.py').read_text()
source=(script.parent/'build_marty_realcoat.py').read_text()
edits={
    'count=16000':'count=32000',
    "'16000 skinned short hairs'":"'32000 combed short hairs'",
    'direction=(n*.55+flow*.84).normalized()':'direction=(n*.20+flow*.98).normalized()',
    'rng.uniform(.18,.48)':'rng.uniform(.10,.28)',
    'width=rng.uniform(.009,.018)':'width=rng.uniform(.006,.012)',
    'rng.uniform(.82,1.13)':'rng.uniform(.96,1.04)',
    'hair_vertices=[];hair_faces=[];hair_weights=[];hair_colors=[]':'hair_vertices=[];hair_faces=[];hair_weights=[];hair_colors=[];hair_normals=[]',
    'hair_weights.extend([ws]*4);hair_colors.extend([col]*4)':'hair_weights.extend([ws]*4);hair_colors.extend([col]*4);hair_normals.extend([tuple(n)]*4)',
    "hair=bpy.data.objects.new('32000 combed short hairs',mesh)":"mesh.normals_split_custom_set([hair_normals[loop.vertex_index] for loop in mesh.loops])\nhair=bpy.data.objects.new('32000 combed short hairs',mesh)",
}
for old,new in edits.items():
    assert old in source,old
    source=source.replace(old,new)
# Feed the refined fur pass into the existing root-union generator without
# overwriting either of its source files or the previous connected export.
anchor="detailed=(script.parent/'build_marty_realcoat.py').read_text()"
assert anchor in connected
connected=connected.replace(anchor,'detailed=softcoat_source')
connected=connected.replace("'SourceArt', 'MartyConnectedRealism'","'SourceArt', 'MartySoftCoat'")
sys.argv.append('--detailed')
softcoat_source=source
exec(compile(connected,str(script),'exec'),globals())
