"""Add an independent brick revision; keep old materials and maps intact."""
import hashlib
import json
import pathlib
import unreal

ROOT = pathlib.Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'SourceArt/WorldRealism/ReadableBrick'
FOLDER = '/Game/Adventure/Scenery/ReadableBrick'
LIB = unreal.EditorAssetLibrary
EDIT = unreal.MaterialEditingLibrary
TOOLS = unreal.AssetToolsHelpers.get_asset_tools()
spec = json.loads((SOURCE / 'manifest.json').read_text())
revision = hashlib.sha256(pathlib.Path(__file__).read_bytes() + (SOURCE / 'manifest.json').read_bytes()).hexdigest()[:12]
path = FOLDER + '/r' + revision
textures = {}
for role in ('Base', 'Normal', 'Rough'):
    task = unreal.AssetImportTask()
    task.filename = str(SOURCE / ('Brick_' + role + '.png'))
    task.destination_path = path
    task.destination_name = 'Brick_' + role
    task.automated = True
    task.save = True
    task.replace_existing = False
    if not LIB.does_asset_exist(path + '/' + task.destination_name):
        TOOLS.import_asset_tasks([task])
    t = LIB.load_asset(path + '/' + task.destination_name)
    assert t
    t.set_editor_property('srgb', role == 'Base')
    if role == 'Normal':
        t.set_editor_property('compression_settings', unreal.TextureCompressionSettings.TC_NORMALMAP)
    elif role == 'Rough':
        t.set_editor_property('compression_settings', unreal.TextureCompressionSettings.TC_GRAYSCALE)
    LIB.save_loaded_asset(t)
    textures[role] = t

name = 'Brick_World'
m = LIB.load_asset(path + '/' + name) if LIB.does_asset_exist(path + '/' + name) else None
if not m:
    m = TOOLS.create_asset(name, path, unreal.Material, unreal.MaterialFactoryNew())
    m.set_editor_property('tangent_space_normal', False)
    EDIT.set_material_usage(m, unreal.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES)

    def node(cls):
        return EDIT.create_material_expression(m, cls)

    def wire(a, out, b, pin):
        assert EDIT.connect_material_expressions(a, out, b, pin)

    def custom(code, pins, output):
        c = node(unreal.MaterialExpressionCustom)
        c.set_editor_property('code', code)
        c.set_editor_property('output_type', output)
        inputs = []
        for pin in pins:
            i = unreal.CustomInput()
            i.set_editor_property('input_name', pin)
            inputs.append(i)
        c.set_editor_property('inputs', inputs)
        return c

    p = node(unreal.MaterialExpressionWorldPosition)
    n = node(unreal.MaterialExpressionVertexNormalWS)
    uv = custom('float3 a=abs(N); return (a.z>0.7 ? P.xy : (a.x>a.y ? P.yz : P.xz))/192.0;',
                ('P', 'N'), unreal.CustomMaterialOutputType.CMOT_FLOAT2)
    wire(p, '', uv, 'P')
    wire(n, '', uv, 'N')
    for role, prop in [('Base', unreal.MaterialProperty.MP_BASE_COLOR), ('Normal', unreal.MaterialProperty.MP_NORMAL), ('Rough', unreal.MaterialProperty.MP_ROUGHNESS)]:
        sample = node(unreal.MaterialExpressionTextureSample)
        sample.set_editor_property('texture', textures[role])
        sample.set_editor_property('sampler_type', {'Base': unreal.MaterialSamplerType.SAMPLERTYPE_COLOR,
            'Normal': unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL, 'Rough': unreal.MaterialSamplerType.SAMPLERTYPE_LINEAR_GRAYSCALE}[role])
        wire(uv, '', sample, 'UVs')
        result, output = sample, 'R' if role == 'Rough' else 'RGB'
        if role == 'Normal':
            # Match the projection's axes instead of using the cube's mesh tangents.
            result = custom('float3 a=abs(N); float3 U=a.z>0.7?float3(1,0,0):(a.x>a.y?float3(0,1,0):float3(1,0,0)); '
                'float3 V=a.z>0.7?float3(0,1,0):float3(0,0,1); '
                'float3 D=T.x*U-T.y*V; D-=N*dot(D,N); return normalize(N*T.z+D);',
                ('T', 'N'), unreal.CustomMaterialOutputType.CMOT_FLOAT3)
            wire(sample, 'RGB', result, 'T')
            wire(n, '', result, 'N')
            output = ''
        assert EDIT.connect_material_property(result, output, prop)
    LIB.set_metadata_tag(m, 'ReadableBrickRevision', revision)
    EDIT.recompile_material(m)
    LIB.save_loaded_asset(m)

# Native code reads this small material instance; prior compiled graphs remain retained.
alias = FOLDER + '/Brick_World'
instance = LIB.load_asset(alias) if LIB.does_asset_exist(alias) else TOOLS.create_asset('Brick_World', FOLDER, unreal.MaterialInstanceConstant, unreal.MaterialInstanceConstantFactoryNew())
EDIT.set_material_instance_parent(instance, m)
LIB.set_metadata_tag(instance, 'ReadableBrickRevision', revision)
EDIT.update_material_instance(instance)
assert instance.get_editor_property('parent') == m
assert LIB.save_loaded_asset(instance, only_if_is_dirty=False)
report = {'revision': revision, 'material': alias, 'resolution': spec['resolution'], 'tile_cm': spec['tile_cm'],
          'parent': instance.get_editor_property('parent').get_path_name(),
          'texture_samples': 3, 'world_space_normals': not m.get_editor_property('tangent_space_normal'),
          'textures': {role: {'srgb': t.get_editor_property('srgb'), 'width': t.blueprint_get_size_x()} for role, t in textures.items()}}
(ROOT / 'Saved/readable-brick-import.json').write_text(json.dumps(report, indent=2))
unreal.log('CHEWEENIE_READABLE_BRICK_IMPORTED ' + revision)
