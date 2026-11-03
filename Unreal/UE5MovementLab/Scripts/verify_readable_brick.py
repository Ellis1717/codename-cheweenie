"""Run in a fresh Editor process to verify the saved alias and its dependencies."""
import json
import pathlib
import unreal

root = pathlib.Path(__file__).resolve().parents[1]
lib = unreal.EditorAssetLibrary
alias = lib.load_asset('/Game/Adventure/Scenery/ReadableBrick/Brick_World')
assert isinstance(alias, unreal.MaterialInstanceConstant)
parent = alias.get_editor_property('parent')
assert parent
revision = lib.get_metadata_tag(alias, 'ReadableBrickRevision')
assert revision and ('/r' + revision + '/') in parent.get_path_name()
for role in ('Base', 'Normal', 'Rough'):
    texture = lib.load_asset(parent.get_path_name().split('/Brick_World.')[0] + '/Brick_' + role)
    assert isinstance(texture, unreal.Texture2D)
    assert texture.get_editor_property('srgb') == (role == 'Base')
report = {'parent': parent.get_path_name(), 'revision': revision, 'fresh_process_parent_and_textures_verified': True}
(root / 'Saved/readable-brick-persisted.json').write_text(json.dumps(report, indent=2))
unreal.log('CHEWEENIE_READABLE_BRICK_PERSISTED ' + revision)
