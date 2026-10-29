using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Security.Cryptography;
using UnityEditor;
using UnityEngine;

namespace CodenameCheweenie.Editor
{
    // Imports the supplied triangular, vertex-coloured OBJ without paid/glTF packages.
    // Disconnected original surfaces are grouped under joints; geometry is not remeshed.
    public static class MartyExplorerImporter
    {
        public const string SourcePath="Assets/Art/Marty/Source/Marty_Explorer.obj";
        public const string PrefabPath="Assets/Resources/MartyVisual.prefab";
        private const string Folder="Assets/Art/Marty/Explorer";
        private const string MeshPath=Folder+"/MartyExplorerMeshes.asset";
        private sealed class Group
        {
            public Transform joint;public Color color;
            public readonly List<Vector3> vertices=new List<Vector3>();
            public readonly List<int> triangles=new List<int>();
            public readonly Dictionary<int,int> indices=new Dictionary<int,int>();
        }
        private static Vector3 Fit(Vector3 p){return p*.78f+new Vector3(0,-.8312f,-.08f);}
        public static void EnsureModel(){if(!AssetDatabase.LoadAssetAtPath<GameObject>(PrefabPath))Create();}
        public static void CreateRetainedModels(){MartyModelBuilder.Create();Create();}
        [MenuItem("Cheweenie/Art/Import supplied Marty Explorer model")]
        public static void Create()
        {
            if(!File.Exists(SourcePath))throw new FileNotFoundException("Restore the retained Marty Explorer source model from Git.",SourcePath);
            var existing=AssetDatabase.LoadAssetAtPath<GameObject>(PrefabPath);
            if(existing&&!existing.GetComponent<MartyModelRig>())throw new InvalidOperationException("Move the custom Marty prefab aside before importing Explorer.");
            var vertices=new List<Vector3>();var colors=new List<Color>();var triangles=new List<int>();
            foreach(string line in File.ReadLines(SourcePath))
            {
                string[] parts=line.Split((char[])null,StringSplitOptions.RemoveEmptyEntries);
                if(parts.Length==0||parts[0].StartsWith("#"))continue;
                if(parts[0]=="v")
                {
                    if(parts.Length!=7)throw new InvalidDataException("Expected xyz and RGB in the supplied coloured OBJ.");
                    vertices.Add(new Vector3(Number(parts[1]),Number(parts[2]),Number(parts[3])));
                    colors.Add(new Color(Number(parts[4]),Number(parts[5]),Number(parts[6]),1));
                }
                else if(parts[0]=="f")
                {
                    if(parts.Length!=4)throw new InvalidDataException("Expected triangular source faces.");
                    for(int i=1;i<4;i++){int index=int.Parse(parts[i].Split('/')[0],CultureInfo.InvariantCulture)-1;if(index<0||index>=vertices.Count)throw new InvalidDataException("Invalid source vertex index.");triangles.Add(index);}
                }
            }
            if(vertices.Count==0||triangles.Count==0)throw new InvalidDataException("Empty Marty model.");
            int[] parents=new int[vertices.Count];for(int i=0;i<parents.Length;i++)parents[i]=i;
            for(int i=0;i<triangles.Count;i+=3){parents[Find(parents,triangles[i+1])]=Find(parents,triangles[i]);parents[Find(parents,triangles[i+2])]=Find(parents,triangles[i]);}
            var bounds=new Dictionary<int,Bounds>();
            for(int i=0;i<vertices.Count;i++){int island=Find(parents,i);if(!bounds.TryGetValue(island,out var box))box=new Bounds(vertices[i],Vector3.zero);box.Encapsulate(vertices[i]);bounds[island]=box;}
            Directory.CreateDirectory(Folder);AssetDatabase.Refresh();
            var root=new GameObject("Marty Explorer | supplied original mesh");
            try
            {
                Transform head=Joint("Head joint",root.transform,new Vector3(0,1.10f,.55f));
                Transform tail=Joint("Tail joint",root.transform,new Vector3(0,.85f,-.75f));
                var legs=new Transform[4];var knees=new Transform[4];var ears=new Transform[2];var eyes=new Transform[2];
                for(int i=0;i<4;i++){float side=i%2==0?-1:1;float z=i<2?.35f:-.58f;legs[i]=Joint("Leg joint "+i,root.transform,new Vector3(side*.25f,.70f,z));knees[i]=Joint("Knee "+i,legs[i],new Vector3(side*.25f,.32f,z));}
                for(int i=0;i<2;i++){float side=i==0?-1:1;ears[i]=Joint("Ear joint "+i,head,new Vector3(side*.34f,1.54f,.62f));eyes[i]=Joint("Blink joint "+i,head,new Vector3(side*.228f,1.451f,1.048f));}
                var groups=new Dictionary<string,Group>();
                for(int i=0;i<triangles.Count;i+=3)
                {
                    int first=triangles[i];Bounds box=bounds[Find(parents,first)];Vector3 center=box.center;
                    Transform joint=root.transform;
                    if(Mathf.Abs(center.x)>.15f&&center.y<.75f){int leg=(center.z>0?0:2)+(center.x<0?0:1);joint=center.y<.30f?knees[leg]:legs[leg];}
                    else if(center.z<-.8f&&center.y>.8f)joint=tail;
                    else if(Mathf.Abs(center.x)>.40f&&center.y>1.15f&&center.z<.85f)joint=ears[center.x<0?0:1];
                    else if(Mathf.Abs(center.x)>.1f&&center.y>1.40f&&center.z>1)joint=eyes[center.x<0?0:1];
                    else if(center.z>.9f&&center.y>1||center.y>1.17f&&center.z>.58f)joint=head;
                    Color color=colors[first];string key=joint.name+"_"+ColorUtility.ToHtmlStringRGB(color);
                    if(!groups.TryGetValue(key,out var group)){group=new Group{joint=joint,color=color};groups[key]=group;}
                    for(int j=0;j<3;j++)
                    {
                        int source=triangles[i+j];if(colors[source]!=color)throw new InvalidDataException("Mixed-colour source triangle needs a vertex-colour shader.");
                        if(!group.indices.TryGetValue(source,out int target)){target=group.vertices.Count;group.indices[source]=target;group.vertices.Add(joint.InverseTransformPoint(Fit(vertices[source])));}
                        group.triangles.Add(target);
                    }
                }
                var savedMeshes=new Dictionary<string,Mesh>();foreach(var asset in AssetDatabase.LoadAllAssetsAtPath(MeshPath))if(asset is Mesh mesh)savedMeshes[mesh.name]=mesh;
                foreach(var pair in groups)
                {
                    var group=pair.Value;var mesh=new Mesh{name=pair.Key};mesh.SetVertices(group.vertices);mesh.SetTriangles(group.triangles,0);mesh.RecalculateNormals();mesh.RecalculateBounds();
                    if(savedMeshes.TryGetValue(pair.Key,out var saved)){EditorUtility.CopySerialized(mesh,saved);UnityEngine.Object.DestroyImmediate(mesh);mesh=saved;}
                    else if(File.Exists(MeshPath))AssetDatabase.AddObjectToAsset(mesh,MeshPath);else AssetDatabase.CreateAsset(mesh,MeshPath);
                    string materialPath=Folder+"/Source_"+ColorUtility.ToHtmlStringRGB(group.color)+".mat";var material=AssetDatabase.LoadAssetAtPath<Material>(materialPath);
                    if(!material){material=new Material(Shader.Find("Standard")){color=group.color,name="Explorer source "+ColorUtility.ToHtmlStringRGB(group.color)};material.SetFloat("_Glossiness",.13f);AssetDatabase.CreateAsset(material,materialPath);}
                    var part=new GameObject("Original surface "+pair.Key);part.transform.SetParent(group.joint,false);part.AddComponent<MeshFilter>().sharedMesh=mesh;part.AddComponent<MeshRenderer>().sharedMaterial=material;
                }
                var rig=root.AddComponent<MartyModelRig>();rig.legs=legs;rig.knees=knees;rig.ears=ears;rig.eyes=eyes;rig.head=head;rig.tail=tail;
                rig.sourceModel="Marty_Explorer.obj";rig.sourceVertexCount=vertices.Count;rig.sourceTriangleCount=triangles.Count/3;
                using(var hash=SHA256.Create())rig.sourceSha256=BitConverter.ToString(hash.ComputeHash(File.ReadAllBytes(SourcePath))).Replace("-","").ToLowerInvariant();
                PrefabUtility.SaveAsPrefabAsset(root,PrefabPath);AssetDatabase.SaveAssets();
                Debug.Log("Retained original Marty Explorer imported: "+vertices.Count+" vertices, "+triangles.Count/3+" triangles, "+groups.Count+" renderers; original colours preserved.");
            }
            finally{UnityEngine.Object.DestroyImmediate(root);}
        }
        private static float Number(string text){float result=float.Parse(text,CultureInfo.InvariantCulture);if(float.IsNaN(result)||float.IsInfinity(result))throw new InvalidDataException("Non-finite model data.");return result;}
        private static int Find(int[] parents,int value){while(parents[value]!=value){parents[value]=parents[parents[value]];value=parents[value];}return value;}
        private static Transform Joint(string name,Transform parent,Vector3 sourcePosition){var joint=new GameObject(name).transform;joint.SetParent(parent,false);joint.position=Fit(sourcePosition);return joint;}
    }
}
