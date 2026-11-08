"""Import retained illustrations non-destructively for native bio/menu screens."""
from pathlib import Path
import json
import unreal

root = Path(__file__).resolve().parents[1]
branding = root.parents[1] / 'Assets/Resources/Branding'
transitions = root / 'SourceArt/Transitions'
sources = {
    'MartyPortrait': branding / 'MartyTrail.png',
    'LakituPortrait': branding / 'LakituPatrol.png',
    'RioPortrait': branding / 'RioCozy.png',
    'SofaCuddle': transitions / '01-sofa-cuddle.png',
    'RainyWindow': transitions / '02-rainy-window.png',
    'GardenEvening': transitions / '03-garden-evening.png',
    'FortPeek': transitions / '04-fort-peek.png',
    'BedtimePile': transitions / '05-bedtime-pile.png',
}
folder = '/Game/Adventure/Crew'
lib = unreal.EditorAssetLibrary
tools = unreal.AssetToolsHelpers.get_asset_tools()
report = []
for name, source in sources.items():
    assert source.is_file(), str(source)
    path = folder + '/' + name
    if not lib.does_asset_exist(path):
        task = unreal.AssetImportTask()
        task.filename = str(source)
        task.destination_path = folder
        task.destination_name = name
        task.automated = True
        task.save = True
        task.replace_existing = False
        tools.import_asset_tasks([task])
        texture = lib.load_asset(path)
        assert texture, path
        texture.set_editor_property('lod_group', unreal.TextureGroup.TEXTUREGROUP_UI)
        texture.set_editor_property('mip_gen_settings', unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS)
        lib.save_loaded_asset(texture)
    assert lib.load_asset(path), path
    report.append(path)
(root / 'Saved/crew-art-import.json').write_text(json.dumps(report, indent=2))
unreal.log('CREW_ART_IMPORTED: ' + str(len(report)))
