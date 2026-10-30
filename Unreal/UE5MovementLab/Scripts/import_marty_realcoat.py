"""Import the original fine-coat variant without changing the retained smooth model."""
import pathlib
script=pathlib.Path(__file__).resolve()
base=(script.parent/'import_marty_rig.py').read_text()
base=base.replace("FOLDER='/Game/Adventure/Marty'","FOLDER='/Game/Adventure/MartyRealism'").replace("'SourceArt','Marty'","'SourceArt','MartyRealism'").replace('Marty_CoverRig','Marty_RealCoat').replace("'marty-import.json'","'marty-realcoat-import.json'")
before=base.index('def material(')
after=base.index('slots=pet.get_editor_property(')
setup=base[:before]
setup=setup.replace('task.options=options;TOOLS.import_asset_tasks([task])','options.skeletal_mesh_import_data.vertex_color_import_option=unreal.VertexColorImportOption.REPLACE;task.factory=unreal.FbxFactory();task.options=options;TOOLS.import_asset_tasks([task])')
exec(compile(setup,str(script),'exec'),globals())

def material(name,color,roughness,fur=False):
    path=FOLDER+'/'+name
    m=LIB.load_asset(path) if LIB.does_asset_exist(path) else None
    if m and (not fur or LIB.get_metadata_tag(m,'RealCoatColorEncoding')=='uv-rgb-v3'):return m
    m=m or TOOLS.create_asset(name,FOLDER,unreal.Material,unreal.MaterialFactoryNew())
    EDIT.set_material_usage(m,unreal.MaterialUsage.MATUSAGE_SKELETAL_MESH)
    def node(kind):return EDIT.create_material_expression(m,kind)
    def wire(a,output,b,input):
        assert EDIT.connect_material_expressions(a,output,b,input), 'Invalid material expression connection: '+str(type(a))+':'+output+' -> '+str(type(b))+':'+input
    def plug(a,output,prop):
        assert EDIT.connect_material_property(a,output,prop), 'Invalid material property connection: '+str(prop)
    def scalar(v):
        n=node(unreal.MaterialExpressionConstant);n.set_editor_property('r',v);return n
    def rgb(c):
        n=node(unreal.MaterialExpressionConstant3Vector);n.set_editor_property('constant',unreal.LinearColor(*c,1));return n
    base=rgb(color)
    def palette():
        rg=node(unreal.MaterialExpressionTextureCoordinate);rg.set_editor_property('coordinate_index',1)
        ba=node(unreal.MaterialExpressionTextureCoordinate);ba.set_editor_property('coordinate_index',2)
        b=node(unreal.MaterialExpressionComponentMask);b.set_editor_property('r',True);b.set_editor_property('g',False)
        wire(ba,'',b,'')
        # FBX changes the UV V convention; restore green before decoding RGB.
        flip=node(unreal.MaterialExpressionConstant2Vector);flip.set_editor_property('r',1);flip.set_editor_property('g',-1)
        offset=node(unreal.MaterialExpressionConstant2Vector);offset.set_editor_property('r',0);offset.set_editor_property('g',1)
        scaled=node(unreal.MaterialExpressionMultiply);wire(rg,'',scaled,'A');wire(flip,'',scaled,'B')
        linear=node(unreal.MaterialExpressionAdd);wire(scaled,'',linear,'A');wire(offset,'',linear,'B')
        result=node(unreal.MaterialExpressionAppendVector);wire(linear,'',result,'A');wire(b,'',result,'B');return result
    if fur and name!='Marty_FineFur':
        vertex=palette()
        coords=node(unreal.MaterialExpressionTextureCoordinate);coords.set_editor_property('u_tiling',4.0);coords.set_editor_property('v_tiling',4.0)
        noise=node(unreal.MaterialExpressionTextureSample);noise.set_editor_property('texture',grain);wire(coords,'',noise,'')
        amount=node(unreal.MaterialExpressionMultiply);wire(noise,'R',amount,'A');wire(scalar(.20),'',amount,'B')
        bias=node(unreal.MaterialExpressionAdd);wire(amount,'',bias,'A');wire(scalar(.85),'',bias,'B')
        base=node(unreal.MaterialExpressionMultiply);wire(vertex,'',base,'A');wire(bias,'',base,'B')
        normal=node(unreal.MaterialExpressionTextureSample);normal.set_editor_property('texture',normalmap);normal.set_editor_property('sampler_type',unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
        wire(coords,'',normal,'');plug(normal,'',unreal.MaterialProperty.MP_NORMAL)
    if name=='Marty_FineFur':
        m.set_editor_property('two_sided',True);m.set_editor_property('shading_model',unreal.MaterialShadingModel.MSM_HAIR)
        base=palette()
        tangent=node(unreal.MaterialExpressionVertexTangentWS);plug(tangent,'',unreal.MaterialProperty.MP_TANGENT)
    plug(base,'',unreal.MaterialProperty.MP_BASE_COLOR)
    plug(scalar(roughness),'',unreal.MaterialProperty.MP_ROUGHNESS)
    plug(scalar(.25 if fur else .5),'',unreal.MaterialProperty.MP_SPECULAR)
    if fur:LIB.set_metadata_tag(m,'RealCoatColorEncoding','uv-rgb-v3')
    EDIT.recompile_material(m);LIB.save_loaded_asset(m,only_if_is_dirty=False);return m

spec={'Marty_Coat':((.58,.31,.105),.68,True),'Marty_Cream':((.74,.59,.37),.73,True),
      'Marty_Ears':((.30,.125,.042),.7,True),'Marty_Iris':((.045,.020,.008),.19,False),
      'Marty_Nose':((.022,.014,.01),.30,False),'Marty_Pupil':((.0015,.0015,.0015),.14,False),
      'Marty_FineFur':((.58,.31,.105),.55,True),'Marty_CollarBlack':((.012,.015,.018),.65,False),
      'Marty_CollarWhite':((.7,.72,.66),.7,False),'Marty_TagBlue':((.025,.29,.52),.38,False)}
exec(compile(base[after:],str(script),'exec'),globals())
report['texture_resolution']={name:[LIB.load_asset(FOLDER+'/'+name).blueprint_get_size_x(),LIB.load_asset(FOLDER+'/'+name).blueprint_get_size_y()] for name in ('CoatGrain','CoatNormal')}
report['fur_material_shading']=str(LIB.load_asset(FOLDER+'/Marty_FineFur').get_editor_property('shading_model'))
report['vertex_color_policy']='Explicit Replace through the legacy FBX factory'
with open(os.path.join(ROOT,'Saved','marty-realcoat-import.json'),'w') as f:json.dump(report,f,indent=2)
