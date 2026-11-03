"""Import only the new recorded pack; retained synthesized assets stay intact."""
import hashlib
import json
from pathlib import Path
import unreal

root = Path(__file__).resolve().parents[1]
source = root/'AudioSources'/'NaturalPets'
manifest = json.loads((source/'manifest.json').read_text())
report = {'engine': unreal.SystemLibrary.get_engine_version(), 'clips': {}}
for name, clip in manifest['clips'].items():
    filename = source/(name+'.wav')
    assert hashlib.sha256(filename.read_bytes()).hexdigest() == clip['sha256']
    task = unreal.AssetImportTask()
    task.filename = str(filename)
    task.destination_path = '/Game/Adventure/Audio/NaturalPets'
    task.destination_name = name
    task.automated = True
    task.replace_existing = True
    task.save = True
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    sound = unreal.EditorAssetLibrary.load_asset(task.destination_path+'/'+name)
    assert isinstance(sound, unreal.SoundWave)
    sound.set_editor_property('looping', False)
    unreal.EditorAssetLibrary.save_loaded_asset(sound)
    report['clips'][name] = {'asset': sound.get_path_name(), 'wav_sha256': clip['sha256']}
(root/'Saved'/'natural-pet-audio-import.json').write_text(json.dumps(report, indent=2))
unreal.log('CHEWEENIE_NATURAL_PET_AUDIO_IMPORTED')
