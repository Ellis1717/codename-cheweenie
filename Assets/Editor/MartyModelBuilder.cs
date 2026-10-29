using System;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

namespace CodenameCheweenie.Editor
{
    // Original surface meshes, authored in metres from the owner's private Marty photo.
    // Reproducible without Blender, paid plugins, downloaded meshes or runtime generators.
    public static class MartyModelBuilder
    {
        private const string Folder = "Assets/Art/Marty";
        private const string MeshPath = Folder + "/MartyMeshes.asset";
        private const string PrefabPath = "Assets/Art/Marty/MartyPhotoGuided.prefab";
        private static Dictionary<string, Mesh> saved;
        private static Material tan, cream, ear, dark, black, blue, white, brown;
        public static void EnsureModel() { MartyExplorerImporter.EnsureModel(); }
        [MenuItem("Cheweenie/Art/Create or rebuild photo-guided Marty")]
        public static void Create()
        {
            var existing = AssetDatabase.LoadAssetAtPath<GameObject>(PrefabPath);
            if (existing && !existing.GetComponent<MartyModelRig>()) throw new InvalidOperationException("An imported custom Marty prefab already exists. Move it aside before generating this model.");
            System.IO.Directory.CreateDirectory(Folder); AssetDatabase.Refresh();
            saved = new Dictionary<string, Mesh>();
            foreach (var asset in AssetDatabase.LoadAllAssetsAtPath(MeshPath)) if (asset is Mesh mesh) saved[mesh.name] = mesh;
            tan = Material("Golden tan coat", new Color(.67f, .40f, .18f));
            cream = Material("Cream throat and belly", new Color(.82f, .69f, .49f));
            ear = Material("Warm brown ears", new Color(.39f, .20f, .075f));
            dark = Material("Dark muzzle and nose", new Color(.16f, .105f, .07f));
            black = Material("Checker black", new Color(.024f, .027f, .03f));
            white = Material("Checker ivory", new Color(.92f, .9f, .82f));
            blue = Material("Blue bone tag", new Color(.035f, .46f, .68f));
            brown = Material("Warm brown eyes", new Color(.19f, .085f, .025f), .55f);
            var root = new GameObject("Marty | photo-guided character");
            try
            {
                var body = Node("Body", root.transform, Vector3.zero);
                Surface("Long tapered body", body, tan, new[] {
                    new Vector4(-.72f,-.25f,.025f,.025f), new Vector4(-.58f,-.23f,.25f,.25f), new Vector4(-.30f,-.24f,.30f,.28f),
                    new Vector4(.02f,-.23f,.29f,.27f), new Vector4(.26f,-.21f,.245f,.27f), new Vector4(.41f,-.17f,.14f,.20f), new Vector4(.46f,-.14f,.015f,.02f) });
                Oval("Cream breast", body, new Vector3(0,-.28f,.26f), new Vector3(.39f,.36f,.36f), cream);
                Oval("Cream underside", body, new Vector3(0,-.455f,-.13f), new Vector3(.37f,.12f,.85f), cream);
                Surface("Rising neck", body, tan, new[] { new Vector4(.10f,-.12f,.14f,.18f), new Vector4(.28f,.01f,.23f,.29f), new Vector4(.43f,.09f,.23f,.32f), new Vector4(.57f,.12f,.13f,.25f) });
                var head = Node("Head joint", root.transform, new Vector3(0,.23f,.46f));
                Surface("Sculpted forehead and cheeks", head, tan, new[] {
                    new Vector4(-.25f,.01f,.02f,.02f), new Vector4(-.15f,.04f,.22f,.25f), new Vector4(.02f,.07f,.30f,.31f),
                    new Vector4(.17f,.025f,.26f,.26f), new Vector4(.28f,-.055f,.18f,.17f), new Vector4(.32f,-.09f,.06f,.06f) });
                Surface("Tapered dachshund muzzle", head, tan, new[] {
                    new Vector4(.18f,-.07f,.18f,.15f), new Vector4(.28f,-.105f,.175f,.13f), new Vector4(.42f,-.15f,.14f,.10f),
                    new Vector4(.53f,-.16f,.115f,.085f), new Vector4(.58f,-.16f,.07f,.05f) });
                Oval("Cream lower jaw", head, new Vector3(0,-.22f,.36f), new Vector3(.23f,.08f,.39f), cream);
                Oval("Mouth line", head, new Vector3(0,-.202f,.485f), new Vector3(.21f,.015f,.11f), dark);
                Oval("Soft triangular nose", head, new Vector3(0,-.14f,.565f), new Vector3(.205f,.13f,.095f), dark);
                for (int side = -1; side <= 1; side += 2)
                {
                    Oval("Nostril " + side, head, new Vector3(side*.054f,-.13f,.61f), new Vector3(.043f,.025f,.018f), black);
                    Oval("Brow " + side, head, new Vector3(side*.205f,.115f,.194f), new Vector3(.18f,.095f,.12f), tan);
                }
                var eyes = new Transform[2]; var ears = new Transform[2];
                for (int i=0;i<2;i++)
                {
                    int side=i==0?-1:1;
                    eyes[i]=Node("Blink joint " + i,head,new Vector3(side*.205f,.055f,.225f));
                    Oval("Dark eyelid",eyes[i],Vector3.zero,new Vector3(.151f,.17f,.075f),dark);
                    Oval("Brown eye",eyes[i],new Vector3(0,0,.028f),new Vector3(.12f,.14f,.055f),brown);
                    Oval("Pupil",eyes[i],new Vector3(side*.012f,0,.051f),new Vector3(.075f,.105f,.033f),black);
                    Oval("Catch light",eyes[i],new Vector3(-.02f,.035f,.068f),Vector3.one*.024f,white);
                    ears[i]=Node("Floppy ear joint " + i,head,new Vector3(side*.255f,.16f,-.045f));
                    EarMesh(ears[i],side);
                }
                Collar(root.transform);
                var legs=new Transform[4]; var knees=new Transform[4];
                for(int i=0;i<4;i++)
                {
                    int side=i%2==0?-1:1; bool front=i<2;
                    legs[i]=Node((front?"Front":"Hind")+" leg joint "+i,body,new Vector3(side*.205f,-.34f,front?.22f:-.43f));
                    Oval("Upper leg "+i,legs[i],new Vector3(0,-.105f,front?0:.015f),new Vector3(.17f,.31f,.19f),tan);
                    knees[i]=Node("Knee "+i,legs[i],new Vector3(0,-.22f,.005f));
                    Oval("Lower leg "+i,knees[i],new Vector3(0,-.08f,.018f),new Vector3(.115f,.22f,.135f),tan);
                    Oval("Paw "+i,knees[i],new Vector3(0,-.185f,.067f),new Vector3(.18f,.115f,.25f),tan);
                    for(int toe=0;toe<3;toe++) Oval("Dark claw "+i+"-"+toe,knees[i],new Vector3((toe-1)*.047f,-.19f,.179f),new Vector3(.026f,.035f,.045f),dark);
                }
                var tail=Node("Tail joint",body,new Vector3(0,-.15f,-.65f));
                Surface("Curved tapered tail",tail,tan,new[] {new Vector4(0,0,.078f,.08f),new Vector4(-.16f,.12f,.06f,.065f),new Vector4(-.30f,.25f,.041f,.045f),new Vector4(-.39f,.37f,.008f,.01f)});
                var rig=root.AddComponent<MartyModelRig>(); rig.legs=legs;rig.knees=knees;rig.ears=ears;rig.eyes=eyes;rig.tail=tail;rig.head=head;rig.sourceModel="Photo-guided alternative";
                CombineStaticParts(root.transform);
                PrefabUtility.SaveAsPrefabAsset(root,PrefabPath); AssetDatabase.SaveAssets();
                Debug.Log("Saved photo-guided alternative mesh/material assets and "+PrefabPath+".");
            }
            finally { UnityEngine.Object.DestroyImmediate(root); saved=null; }
        }
        private static Transform Node(string name,Transform parent,Vector3 position)
        {var node=new GameObject(name).transform;node.SetParent(parent,false);node.localPosition=position;return node;}
        private static void CombineStaticParts(Transform parent)
        {
            var children=new List<Transform>();foreach(Transform child in parent)children.Add(child);
            foreach(var child in children)if(child.childCount>0)CombineStaticParts(child);
            var groups=new Dictionary<Material,List<MeshFilter>>();
            foreach(Transform child in parent)
            {
                var filter=child.GetComponent<MeshFilter>();var renderer=child.GetComponent<MeshRenderer>();
                if(!filter||!renderer||child.childCount>0)continue;
                if(!groups.TryGetValue(renderer.sharedMaterial,out var list)){list=new List<MeshFilter>();groups[renderer.sharedMaterial]=list;}list.Add(filter);
            }
            foreach(var group in groups)
            {
                if(group.Value.Count<2)continue;
                var instances=new List<CombineInstance>();
                foreach(var filter in group.Value)instances.Add(new CombineInstance{mesh=filter.sharedMesh,transform=parent.worldToLocalMatrix*filter.transform.localToWorldMatrix});
                var mesh=new Mesh();mesh.CombineMeshes(instances.ToArray(),true,true);
                foreach(var filter in group.Value)UnityEngine.Object.DestroyImmediate(filter.gameObject);
                Put("Combined "+parent.name+" "+group.Key.name,parent,mesh,group.Key);
            }
        }
        private static Material Material(string name,Color color,float gloss=.13f)
        {
            string path=Folder+"/"+name+".mat";var material=AssetDatabase.LoadAssetAtPath<Material>(path);
            if(!material){material=new Material(Shader.Find("Standard")){name=name,color=color};material.SetFloat("_Glossiness",gloss);AssetDatabase.CreateAsset(material,path);}
            return material;
        }
        private static void Put(string name,Transform parent,Mesh mesh,Material material)
        {
            mesh.name=name;
            if(saved.TryGetValue(name,out var previous)){EditorUtility.CopySerialized(mesh,previous);UnityEngine.Object.DestroyImmediate(mesh);mesh=previous;}
            else{if(System.IO.File.Exists(MeshPath))AssetDatabase.AddObjectToAsset(mesh,MeshPath);else AssetDatabase.CreateAsset(mesh,MeshPath);saved[name]=mesh;}
            var part=Node(name,parent,Vector3.zero);part.gameObject.AddComponent<MeshFilter>().sharedMesh=mesh;part.gameObject.AddComponent<MeshRenderer>().sharedMaterial=material;
        }
        private static void Surface(string name,Transform parent,Material material,Vector4[] rings)
        {Put(name,parent,Loft(rings),material);}
        private static void Oval(string name,Transform parent,Vector3 center,Vector3 size,Material material)
        {
            const int count=12;var rings=new Vector4[count+1];
            for(int i=0;i<=count;i++){float angle=Mathf.PI*i/count;float radius=Mathf.Max(.002f,Mathf.Sin(angle));rings[i]=new Vector4(-Mathf.Cos(angle)*.5f,0,radius*.5f,radius*.5f);}
            var mesh=Loft(rings);var vertices=mesh.vertices;for(int i=0;i<vertices.Length;i++)vertices[i]=Vector3.Scale(vertices[i],size)+center;
            mesh.vertices=vertices;mesh.RecalculateNormals();mesh.RecalculateBounds();Put(name,parent,mesh,material);
        }
        private static Mesh Loft(Vector4[] rings)
        {
            const int sides=20;var vertices=new List<Vector3>();var triangles=new List<int>();
            foreach(var ring in rings)for(int j=0;j<sides;j++){float a=2*Mathf.PI*j/sides;vertices.Add(new Vector3(Mathf.Cos(a)*ring.z,ring.y+Mathf.Sin(a)*ring.w,ring.x));}
            // Winding follows increasing z, including backwards-authored tail rings below.
            bool reverse=rings[rings.Length-1].x<rings[0].x;
            for(int i=0;i<rings.Length-1;i++)for(int j=0;j<sides;j++)
            {int a=i*sides+j,b=i*sides+(j+1)%sides,c=a+sides,d=b+sides;Tri(triangles,a,b,c,reverse);Tri(triangles,b,d,c,reverse);}
            for(int end=0;end<2;end++){int ring=end==0?0:rings.Length-1;int center=vertices.Count;vertices.Add(new Vector3(0,rings[ring].y,rings[ring].x));for(int j=0;j<sides;j++)Tri(triangles,center,ring*sides+(j+1)%sides,ring*sides+j,(end!=0)^reverse);}
            var mesh=new Mesh();mesh.SetVertices(vertices);mesh.SetTriangles(triangles,0);mesh.RecalculateNormals();mesh.RecalculateBounds();return mesh;
        }
        private static void Tri(List<int> list,int a,int b,int c,bool reverse=false){list.Add(a);list.Add(reverse?c:b);list.Add(reverse?b:c);}
        private static void EarMesh(Transform joint,int side)
        {
            var mesh=Loft(new[]{new Vector4(-.035f,0,.045f,.04f),new Vector4(.025f,-.08f,.095f,.10f),new Vector4(.08f,-.25f,.105f,.14f),new Vector4(.13f,-.37f,.055f,.055f),new Vector4(.15f,-.385f,.008f,.008f)});
            var vertices=mesh.vertices;for(int i=0;i<vertices.Length;i++){var p=vertices[i];p.x+=side*(-p.y)*.35f;vertices[i]=p;}mesh.vertices=vertices;mesh.RecalculateNormals();mesh.RecalculateBounds();Put("Sculpted floppy ear "+side,joint,mesh,ear);
        }
        private static void Collar(Transform parent)
        {
            var collar=Node("Checker collar and blue tag",parent,new Vector3(0,-.005f,.38f));
            for(int row=0;row<2;row++)for(int cell=0;cell<20;cell++)
            {
                float a=cell*Mathf.PI*2/20,b=(cell+1)*Mathf.PI*2/20;float y0=row*.052f-.052f,y1=y0+.052f;
                var vertices=new[]{new Vector3(Mathf.Cos(a)*.237f,y0,Mathf.Sin(a)*.245f),new Vector3(Mathf.Cos(b)*.237f,y0,Mathf.Sin(b)*.245f),new Vector3(Mathf.Cos(a)*.237f,y1,Mathf.Sin(a)*.245f),new Vector3(Mathf.Cos(b)*.237f,y1,Mathf.Sin(b)*.245f)};
                var mesh=new Mesh{vertices=vertices,triangles=new[]{0,2,1,1,2,3}};mesh.RecalculateNormals();mesh.RecalculateBounds();Put("Collar check "+row+"-"+cell,collar,mesh,(row+cell)%2==0?black:white);
            }
            var tag=Node("Blue bone tag (no personal numbers)",collar,new Vector3(0,-.14f,.266f));
            Oval("Tag centre",tag,Vector3.zero,new Vector3(.14f,.075f,.024f),blue);
            for(int x=-1;x<=1;x+=2)for(int y=-1;y<=1;y+=2)Oval("Tag lobe "+x+"-"+y,tag,new Vector3(x*.075f,y*.033f,0),new Vector3(.075f,.066f,.025f),blue);
            Oval("Tag ring",collar,new Vector3(0,-.072f,.254f),new Vector3(.028f,.045f,.018f),white);
        }
    }
}
