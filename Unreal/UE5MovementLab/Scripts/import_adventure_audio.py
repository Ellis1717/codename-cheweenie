"""Import retained original project audio. Existing edited Unreal assets are preserved."""
import hashlib
import json
import os
import unreal

root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
source = os.path.abspath(os.path.join(root, '..', '..', 'Assets', 'Resources', 'Audio'))
tools = unreal.AssetToolsHelpers.get_asset_tools()
report = {'engine': unreal.SystemLibrary.get_engine_version(), 'sounds': {}}
for name in ('Home', 'Menu', 'Discovery', 'Lakitu', 'Rio', 'Cozy'):
    path = '/Game/Adventure/Audio/' + name
    wave = unreal.EditorAssetLibrary.load_asset(path) if unreal.EditorAssetLibrary.does_asset_exist(path) else None
    if not wave:
        task = unreal.AssetImportTask()
        task.filename = os.path.join(source, name + '.wav')
        task.destination_path = '/Game/Adventure/Audio'
        task.destination_name = name
        task.automated = True
        task.save = True
        tools.import_asset_tasks([task])
        wave = unreal.EditorAssetLibrary.load_asset(path)
        assert wave, 'Sound import failed: ' + name
        wave.set_editor_property('looping', name == 'Home')
        unreal.EditorAssetLibrary.save_loaded_asset(wave)
    assert isinstance(wave, unreal.SoundWave), 'Expected a SoundWave: ' + name
    if name == 'Home':
        assert wave.get_editor_property('looping'), 'Home music must loop (or explicitly change the game audio design)'
    with open(os.path.join(source, name + '.wav'), 'rb') as stream:
        digest = hashlib.sha256(stream.read()).hexdigest()
    report['sounds'][name] = {'asset': wave.get_path_name(), 'original_wav_sha256': digest, 'looping': wave.get_editor_property('looping')}
cover_path = '/Game/Adventure/Art/Cover'
if not unreal.EditorAssetLibrary.does_asset_exist(cover_path):
    task = unreal.AssetImportTask()
    task.filename = os.path.abspath(os.path.join(source, '..', 'Branding', 'Cover.png'))
    task.destination_path = '/Game/Adventure/Art'
    task.destination_name = 'Cover'
    task.automated = True
    task.save = True
    tools.import_asset_tasks([task])
assert unreal.EditorAssetLibrary.load_asset(cover_path), 'Retained cover import failed'
report['cover'] = cover_path
os.makedirs(os.path.join(root, 'Saved'), exist_ok=True)
with open(os.path.join(root, 'Saved', 'adventure-audio.json'), 'w') as stream:
    json.dump(report, stream, indent=2)
unreal.log('CHEWEENIE_ADVENTURE_AUDIO_IMPORTED')
