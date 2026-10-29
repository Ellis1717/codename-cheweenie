using System.Collections.Generic;
using System.IO;
using UnityEditor;
using UnityEngine;

namespace CodenameCheweenie.Editor
{
    // Original reusable props and seamless surface patterns; no external art dependencies.
    public static class SceneryAssetBuilder
    {
        private const string Folder="Assets/Art/Scenery";
        private static Mesh rounded;
        private static SceneryPalette palette;
        public static void EnsureAssets(){if(!AssetDatabase.LoadAssetAtPath<SceneryPalette>("Assets/Resources/SceneryPalette.asset"))Create();}
        public static void CreateAll(){CatModelBuilder.Create();Create();}
        [MenuItem("Cheweenie/Art/Create or rebuild cozy scenery assets")]
        public static void Create()
        {
            Directory.CreateDirectory(Folder);AssetDatabase.Refresh();
            palette=AssetDatabase.LoadAssetAtPath<SceneryPalette>("Assets/Resources/SceneryPalette.asset");
            if(!palette){palette=ScriptableObject.CreateInstance<SceneryPalette>();AssetDatabase.CreateAsset(palette,"Assets/Resources/SceneryPalette.asset");}
            palette.wood=Surface("Oak",new Color(.57f,.38f,.22f),0);palette.grass=Surface("Meadow",new Color(.34f,.49f,.24f),1);
            palette.stone=Surface("Stone",new Color(.49f,.51f,.49f),2);palette.paving=Surface("Pavers",new Color(.62f,.61f,.54f),3);
            palette.brick=Surface("Brick",new Color(.63f,.40f,.29f),4);palette.plaster=Surface("Plaster",new Color(.82f,.79f,.67f),2);
            palette.fabric=Surface("Woven blue",new Color(.27f,.40f,.51f),5);palette.roof=Surface("Roof",new Color(.32f,.30f,.31f),3);
            palette.leaf=Surface("Leaves",new Color(.24f,.39f,.16f),1);palette.glass=Surface("Window glass",new Color(.38f,.62f,.70f),6);
            palette.metal=Surface("Painted iron",new Color(.11f,.16f,.17f),6);palette.warmLight=Surface("Warm glow",new Color(1,.71f,.32f),6);
            palette.warmLight.EnableKeyword("_EMISSION");palette.warmLight.SetColor("_EmissionColor",new Color(1,.62f,.22f)*1.3f);EditorUtility.SetDirty(palette.warmLight);
            rounded=RoundedBox();
            palette.sofa=Prop("Sofa",root=>{
                Box(root,"Upholstered base",new Vector3(0,-.17f,0),new Vector3(.94f,.4f,.9f),palette.fabric);
                Box(root,"Padded back",new Vector3(0,.19f,.34f),new Vector3(1,.61f,.28f),palette.fabric);
                for(int side=-1;side<=1;side+=2){Box(root,"Rolled arm",new Vector3(side*.43f,.02f,0),new Vector3(.15f,.65f,.96f),palette.fabric);for(int z=-1;z<=1;z+=2)Box(root,"Oak foot",new Vector3(side*.36f,-.45f,z*.31f),new Vector3(.12f,.12f,.14f),palette.wood);}
                for(int i=-1;i<=1;i++)Box(root,"Separate seat cushion",new Vector3(i*.25f,.06f,-.07f),new Vector3(.24f,.15f,.65f),palette.fabric);
            });
            palette.table=Prop("Table",root=>{Box(root,"Rounded oak top",new Vector3(0,.35f,0),new Vector3(1,.25f,1),palette.wood);for(int x=-1;x<=1;x+=2)for(int z=-1;z<=1;z+=2)Box(root,"Table leg",new Vector3(x*.36f,-.1f,z*.36f),new Vector3(.13f,.75f,.13f),palette.wood);});
            palette.island=Prop("Kitchen island",root=>{Box(root,"Oak cabinet",new Vector3(0,-.06f,0),new Vector3(.95f,.86f,.91f),palette.wood);Box(root,"Stone countertop",new Vector3(0,.43f,0),new Vector3(1,.14f,1),palette.stone);for(int i=-1;i<=1;i++){Box(root,"Inset door",new Vector3(i*.3f,-.08f,-.46f),new Vector3(.27f,.68f,.035f),palette.plaster);Box(root,"Door handle",new Vector3(i*.3f,.13f,-.49f),new Vector3(.11f,.024f,.025f),palette.metal);}});
            palette.cushion=Prop("Cushion",root=>Box(root,"Soft cushion",Vector3.zero,Vector3.one,palette.fabric));
            palette.fence=Prop("Picket fence",root=>{for(int i=0;i<4;i++)Box(root,"Rounded picket",new Vector3((i-1.5f)*.25f,0,0),new Vector3(.15f,1.3f,.10f),palette.wood);for(int y=-1;y<=1;y+=2)Box(root,"Horizontal rail",new Vector3(0,y*.32f,.07f),new Vector3(1,.13f,.11f),palette.wood);});
            palette.tree=Prop("Garden tree",root=>{
                Box(root,"Bark trunk",new Vector3(0,1.15f,0),new Vector3(.35f,2.4f,.38f),palette.wood);
                for(int i=0;i<7;i++){float angle=i*Mathf.PI*2/7;Oval(root,"Layered foliage",new Vector3(Mathf.Cos(angle)*.65f,2.7f+(i%3)*.4f,Mathf.Sin(angle)*.65f),new Vector3(2,1.8f,2),palette.leaf);}
            });
            palette.shrub=Prop("Leafy shrub",root=>{for(int i=0;i<3;i++)Oval(root,"Shrub foliage",new Vector3((i-1)*.34f,.36f+(i%2)*.1f,0),new Vector3(.95f,.85f,.95f),palette.leaf);});
            palette.flowers=Prop("Flower planter",root=>{
                Box(root,"Timber planter",new Vector3(0,.18f,0),new Vector3(1.4f,.42f,.7f),palette.wood);Box(root,"Dark soil",new Vector3(0,.39f,0),new Vector3(1.2f,.06f,.54f),palette.stone);
                for(int i=0;i<5;i++){float x=(i-2)*.22f;Box(root,"Stem",new Vector3(x,.62f,(i%2)*.15f-.075f),new Vector3(.025f,.42f,.025f),palette.leaf);for(int p=0;p<5;p++){float a=p*Mathf.PI*2/5;Oval(root,"Amber petal",new Vector3(x+Mathf.Cos(a)*.08f,.84f+Mathf.Sin(a)*.065f,(i%2)*.15f-.075f),new Vector3(.13f,.12f,.07f),palette.warmLight);}}
            });
            palette.window=Prop("Framed window",root=>{
                Box(root,"Blue glass",new Vector3(0,0,0),new Vector3(.9f,.9f,.025f),palette.glass);
                for(int side=-1;side<=1;side+=2){Box(root,"Side casing",new Vector3(side*.47f,0,.035f),new Vector3(.07f,1,.07f),palette.plaster);Box(root,"Top bottom casing",new Vector3(0,side*.47f,.035f),new Vector3(1,.07f,.07f),palette.plaster);}
                Box(root,"Centre mullion",new Vector3(0,0,.035f),new Vector3(.04f,.94f,.05f),palette.plaster);Box(root,"Cross mullion",new Vector3(0,0,.035f),new Vector3(.94f,.04f,.05f),palette.plaster);
                Box(root,"Window sill",new Vector3(0,-.53f,.07f),new Vector3(1.08f,.08f,.16f),palette.wood);
            });
            palette.bench=Prop("Park bench",root=>{for(int i=0;i<4;i++)Box(root,"Seat slat",new Vector3(0,.5f,(i-1.5f)*.18f),new Vector3(2.5f,.10f,.14f),palette.wood);for(int i=0;i<3;i++)Box(root,"Back slat",new Vector3(0,.8f+i*.18f,.4f),new Vector3(2.5f,.14f,.10f),palette.wood);for(int side=-1;side<=1;side+=2){Box(root,"Iron support",new Vector3(side*.9f,.25f,0),new Vector3(.12f,.55f,.8f),palette.metal);Box(root,"Armrest",new Vector3(side*1.1f,.8f,0),new Vector3(.10f,.08f,.8f),palette.metal);}});
            palette.playground=Prop("Park play frame",root=>{
                for(int x=-1;x<=1;x+=2)for(int z=-1;z<=1;z+=2)Box(root,"Play tower post",new Vector3(x*1.1f,1.6f,z*1.1f),new Vector3(.16f,3.2f,.16f),palette.wood);
                Box(root,"Tower platform",new Vector3(0,1.5f,0),new Vector3(2.4f,.14f,2.4f),palette.wood);Box(root,"Blue canopy",new Vector3(0,3.3f,0),new Vector3(2.8f,.25f,2.8f),palette.fabric);
                var slide=Box(root,"Slide",new Vector3(0,.82f,-2.2f),new Vector3(.9f,.10f,3),palette.glass);slide.transform.localRotation=Quaternion.Euler(-27,0,0);
                for(int i=0;i<5;i++)Box(root,"Ladder rung",new Vector3(0,.3f+i*.3f,1.2f),new Vector3(.8f,.07f,.08f),palette.metal);
            });
            palette.rug=Prop("Woven rug",root=>Box(root,"Bound rug",Vector3.zero,new Vector3(1,.008f,1),palette.fabric));
            palette.gabledRoof=Prop("Cottage roof",root=>{
                var mesh=new Mesh{name="Original cottage gable"};mesh.vertices=new[]{new Vector3(-.55f,0,-.55f),new Vector3(.55f,0,-.55f),new Vector3(0,.6f,-.55f),new Vector3(-.55f,0,.55f),new Vector3(.55f,0,.55f),new Vector3(0,.6f,.55f)};
                mesh.uv=new[]{Vector2.zero,Vector2.right,new Vector2(.5f,1),Vector2.zero,Vector2.right,new Vector2(.5f,1)};mesh.triangles=new[]{0,2,1,3,4,5,0,3,5,0,5,2,1,2,5,1,5,4};mesh.RecalculateNormals();mesh.RecalculateBounds();var part=new GameObject("Tiled gable");part.transform.SetParent(root,false);part.AddComponent<MeshFilter>().sharedMesh=mesh;part.AddComponent<MeshRenderer>().sharedMaterial=palette.roof;
            });
            Object.DestroyImmediate(rounded);EditorUtility.SetDirty(palette);AssetDatabase.SaveAssets();Debug.Log("Retained scenery palette, seamless textures and reusable props saved.");
        }
        private static Material Surface(string name,Color color,int style)
        {
            string path=Folder+"/"+name+".mat";var material=AssetDatabase.LoadAssetAtPath<Material>(path);if(material)return material;
            const int size=128;var texture=new Texture2D(size,size,TextureFormat.RGB24,false);var pixels=new Color[size*size];
            for(int y=0;y<size;y++)for(int x=0;x<size;x++)
            {
                float noise=Noise(x,y)*.12f-.06f;float pattern=0;
                if(style==0){pattern=Mathf.Sin(y*.32f+Mathf.Sin(x*.12f)*.8f)*.035f;if(y%32<2)pattern=-.17f;}
                if(style==1)pattern=(Noise(x/3,y/7)-.5f)*.17f;
                if(style==3||style==4){int width=style==4?64:32,height=style==4?16:32;int offset=(y/height)%2*width/2;if((x+offset)%width<2||y%height<2)pattern=-.17f;else pattern=(Noise((x+offset)/width,y/height)-.5f)*.1f;}
                if(style==5)pattern=((x%3==0?1:0)+(y%3==0?1:0))*.035f-.03f;
                if(style==6)noise=0;
                pixels[y*size+x]=color*Mathf.Clamp(1+noise+pattern,.55f,1.3f);
            }
            texture.SetPixels(pixels);texture.Apply();string png=Folder+"/"+name+".png";File.WriteAllBytes(png,texture.EncodeToPNG());Object.DestroyImmediate(texture);AssetDatabase.ImportAsset(png);
            var importer=(TextureImporter)AssetImporter.GetAtPath(png);importer.mipmapEnabled=true;importer.wrapMode=TextureWrapMode.Repeat;importer.filterMode=FilterMode.Trilinear;importer.anisoLevel=2;importer.SaveAndReimport();
            material=new Material(Shader.Find("Standard")){name="Cozy "+name,mainTexture=AssetDatabase.LoadAssetAtPath<Texture2D>(png),enableInstancing=true};material.SetFloat("_Glossiness",style==6?.3f:.10f);AssetDatabase.CreateAsset(material,path);return material;
        }
        private static float Noise(int x,int y){unchecked{uint n=(uint)(x*374761393+y*668265263);n=(n^(n>>13))*1274126177;return(n&65535)/65535f;}}
        private static GameObject Prop(string name,System.Action<Transform> create)
        {
            var root=new GameObject(name);try{create(root.transform);Combine(root.transform);string path=Folder+"/"+name+".prefab";new RetainedArtWriter(Folder+"/"+name+" meshes").SavePrefab(root,path);return AssetDatabase.LoadAssetAtPath<GameObject>(path);}finally{Object.DestroyImmediate(root);}
        }
        private static GameObject Box(Transform parent,string name,Vector3 position,Vector3 size,Material material)
        {var go=new GameObject(name);go.transform.SetParent(parent,false);go.transform.localPosition=position;go.transform.localScale=size;go.AddComponent<MeshFilter>().sharedMesh=rounded;go.AddComponent<MeshRenderer>().sharedMaterial=material;return go;}
        private static void Oval(Transform parent,string name,Vector3 position,Vector3 size,Material material)
        {var go=GameObject.CreatePrimitive(PrimitiveType.Sphere);go.name=name;go.transform.SetParent(parent,false);go.transform.localPosition=position;go.transform.localScale=size;go.GetComponent<Renderer>().sharedMaterial=material;Object.DestroyImmediate(go.GetComponent<Collider>());}
        private static void Combine(Transform root)
        {
            var groups=new Dictionary<Material,List<MeshFilter>>();foreach(var filter in root.GetComponentsInChildren<MeshFilter>()){var material=filter.GetComponent<MeshRenderer>().sharedMaterial;if(!groups.TryGetValue(material,out var list)){list=new List<MeshFilter>();groups[material]=list;}list.Add(filter);}
            foreach(var pair in groups){var instances=new List<CombineInstance>();foreach(var filter in pair.Value)instances.Add(new CombineInstance{mesh=filter.sharedMesh,transform=root.worldToLocalMatrix*filter.transform.localToWorldMatrix});var mesh=new Mesh{name="Combined "+root.name+" "+pair.Key.name};mesh.CombineMeshes(instances.ToArray(),true,true);foreach(var filter in pair.Value)Object.DestroyImmediate(filter.gameObject);var part=new GameObject(mesh.name);part.transform.SetParent(root,false);part.AddComponent<MeshFilter>().sharedMesh=mesh;part.AddComponent<MeshRenderer>().sharedMaterial=pair.Key;}
        }
        private static Mesh RoundedBox()
        {
            const int cells=5;const float bevel=.085f;var vertices=new List<Vector3>();var normals=new List<Vector3>();var uv=new List<Vector2>();var triangles=new List<int>();
            Vector3[] ns={Vector3.forward,Vector3.back,Vector3.right,Vector3.left,Vector3.up,Vector3.down};Vector3[] us={Vector3.right,Vector3.left,Vector3.back,Vector3.forward,Vector3.right,Vector3.right};Vector3[] vs={Vector3.up,Vector3.up,Vector3.up,Vector3.up,Vector3.back,Vector3.forward};
            for(int face=0;face<6;face++){int start=vertices.Count;for(int y=0;y<=cells;y++)for(int x=0;x<=cells;x++){Vector3 p=ns[face]*.5f+us[face]*(x/(float)cells-.5f)+vs[face]*(y/(float)cells-.5f);Vector3 inside=new Vector3(Mathf.Clamp(p.x,-.5f+bevel,.5f-bevel),Mathf.Clamp(p.y,-.5f+bevel,.5f-bevel),Mathf.Clamp(p.z,-.5f+bevel,.5f-bevel));Vector3 normal=(p-inside).normalized;vertices.Add(inside+normal*bevel);normals.Add(normal);uv.Add(new Vector2(x/(float)cells,y/(float)cells));if(x<cells&&y<cells){int a=start+y*(cells+1)+x,b=a+1,c=a+cells+1,d=c+1;triangles.AddRange(new[]{a,b,c,b,d,c});}}}
            var mesh=new Mesh{name="Original rounded box"};mesh.SetVertices(vertices);mesh.SetNormals(normals);mesh.SetUVs(0,uv);mesh.SetTriangles(triangles,0);mesh.RecalculateBounds();return mesh;
        }
    }
}
