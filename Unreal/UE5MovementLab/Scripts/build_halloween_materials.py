"""Original Halloween fabric materials. Regenerate in the active Unreal project only."""
import unreal
ASSETS = unreal.EditorAssetLibrary
EDIT = unreal.MaterialEditingLibrary
TOOLS = unreal.AssetToolsHelpers.get_asset_tools()
FOLDER = '/Game/Adventure/Halloween'
for name, rgb in {
    'Bun': (.66, .34, .105), 'Red': (.58, .025, .035),
    'Gold': (.95, .56, .025), 'Orange': (.85, .19, .016),
    'PumpkinSeam': (.42, .075, .009), 'Green': (.055, .18, .026),
    'Black': (.012, .014, .018), 'Wing': (.75, .82, .78),
}.items():
    path = FOLDER + '/' + name
    mat = ASSETS.load_asset(path) if ASSETS.does_asset_exist(path) else TOOLS.create_asset(name, FOLDER, unreal.Material, unreal.MaterialFactoryNew())
    EDIT.delete_all_material_expressions(mat)
    color = EDIT.create_material_expression(mat, unreal.MaterialExpressionConstant3Vector)
    color.set_editor_property('constant', unreal.LinearColor(*rgb, 1))
    EDIT.connect_material_property(color, '', unreal.MaterialProperty.MP_BASE_COLOR)
    rough = EDIT.create_material_expression(mat, unreal.MaterialExpressionConstant)
    rough.set_editor_property('r', .82)
    EDIT.connect_material_property(rough, '', unreal.MaterialProperty.MP_ROUGHNESS)
    EDIT.recompile_material(mat)
    ASSETS.save_loaded_asset(mat)
print('HALLOWEEN_MATERIALS_READY: 8 original opaque costume fabrics')
