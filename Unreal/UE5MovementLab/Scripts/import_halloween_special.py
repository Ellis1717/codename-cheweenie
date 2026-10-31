"""Import original seasonal illustration and two simple festival materials."""
import unreal, os, json
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LIB=unreal.EditorAssetLibrary;TOOLS=unreal.AssetToolsHelpers.get_asset_tools();EDIT=unreal.MaterialEditingLibrary
made=[]
for name,rgb,glow in [('FestivalPlum',(.055,.022,.10),False),('FestivalGlow',(1,.30,.04),True)]:
    folder='/Game/Adventure/Halloween';path=folder+'/'+name
    if LIB.does_asset_exist(path):
        made.append(path);continue
    m=TOOLS.create_asset(name,folder,unreal.Material,unreal.MaterialFactoryNew())
    c=EDIT.create_material_expression(m,unreal.MaterialExpressionConstant3Vector)
    c.set_editor_property('constant',unreal.LinearColor(*rgb,1));EDIT.connect_material_property(c,'',unreal.MaterialProperty.MP_BASE_COLOR)
    v=EDIT.create_material_expression(m,unreal.MaterialExpressionConstant);v.set_editor_property('r',.7 if glow else .82);EDIT.connect_material_property(v,'',unreal.MaterialProperty.MP_ROUGHNESS)
    if glow:
        e=EDIT.create_material_expression(m,unreal.MaterialExpressionConstant3Vector);e.set_editor_property('constant',unreal.LinearColor(3,.6,.09,1));EDIT.connect_material_property(e,'',unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    EDIT.recompile_material(m);LIB.save_loaded_asset(m);made.append(path)
# Existing wardrobe materials also need the instancing permutation for porch batches.
for name in ['Orange','Green','Black','FestivalPlum','FestivalGlow']:
    m=LIB.load_asset('/Game/Adventure/Halloween/'+name);assert m
    EDIT.set_material_usage(m,unreal.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES)
    EDIT.recompile_material(m);LIB.save_loaded_asset(m)
job=unreal.AssetImportTask();job.filename=os.path.join(ROOT,'SourceArt','HalloweenSpecial','HalloweenCover.png');job.destination_path='/Game/Adventure/HalloweenSpecial';job.destination_name='HalloweenCover';job.automated=True;job.save=True;job.replace_existing=True
TOOLS.import_asset_tasks([job]);texture=LIB.load_asset('/Game/Adventure/HalloweenSpecial/HalloweenCover');assert isinstance(texture,unreal.Texture2D)
texture.set_editor_property('lod_group',unreal.TextureGroup.TEXTUREGROUP_UI);texture.set_editor_property('never_stream',True);LIB.save_loaded_asset(texture)
with open(os.path.join(ROOT,'Saved','festival-import.json'),'w') as f:json.dump({'illustration':texture.get_path_name(),'materials':made,'scope':'Original AI-generated seasonal illustration, separate from gameplay meshes and original cover; original engine-native materials'},f,indent=2)
print('FESTIVAL_IMPORT_READY')
