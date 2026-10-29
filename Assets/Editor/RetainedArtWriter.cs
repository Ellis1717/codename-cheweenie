using System.Collections.Generic;
using System.IO;
using UnityEditor;
using UnityEngine;

namespace CodenameCheweenie.Editor
{
    // Save native art resources with stable mesh/material IDs across explicit rebuilds.
    public sealed class RetainedArtWriter
    {
        private readonly string folder, meshPath;
        private readonly Dictionary<int,Mesh> copied=new Dictionary<int,Mesh>();
        private readonly Dictionary<int,Material> materials=new Dictionary<int,Material>();
        private readonly Dictionary<string,Mesh> existing=new Dictionary<string,Mesh>();
        private int meshNumber,materialNumber;
        public RetainedArtWriter(string folder)
        {
            this.folder=folder;Directory.CreateDirectory(folder);AssetDatabase.Refresh();meshPath=folder+"/Meshes.asset";
            foreach(var asset in AssetDatabase.LoadAllAssetsAtPath(meshPath))if(asset is Mesh mesh)existing[mesh.name]=mesh;
        }
        public Mesh Mesh(Mesh source)
        {
            if(copied.TryGetValue(source.GetInstanceID(),out var saved))return saved;
            var copy=Object.Instantiate(source);copy.name="Mesh "+meshNumber+++" "+source.name;
            if(existing.TryGetValue(copy.name,out saved)){EditorUtility.CopySerialized(copy,saved);Object.DestroyImmediate(copy);}
            else{saved=copy;if(File.Exists(meshPath))AssetDatabase.AddObjectToAsset(saved,meshPath);else AssetDatabase.CreateAsset(saved,meshPath);}
            copied[source.GetInstanceID()]=saved;return saved;
        }
        public Material Material(Material source)
        {
            if(AssetDatabase.Contains(source))return source;
            if(materials.TryGetValue(source.GetInstanceID(),out var saved))return saved;
            string path=folder+"/Material "+materialNumber+++".mat";saved=AssetDatabase.LoadAssetAtPath<Material>(path);
            Texture2D retainedTexture=null;
            if(source.mainTexture is Texture2D paint&&!AssetDatabase.Contains(paint))
            {
                string png=folder+"/Coat.png";File.WriteAllBytes(png,paint.EncodeToPNG());AssetDatabase.ImportAsset(png);
                var importer=(TextureImporter)AssetImporter.GetAtPath(png);importer.mipmapEnabled=true;importer.wrapMode=TextureWrapMode.Repeat;importer.wrapModeV=TextureWrapMode.Clamp;importer.SaveAndReimport();
                retainedTexture=AssetDatabase.LoadAssetAtPath<Texture2D>(png);
            }
            if(!saved)
            {
                saved=new Material(source);saved.name=source.name;saved.enableInstancing=true;
                if(retainedTexture)saved.mainTexture=retainedTexture;
                AssetDatabase.CreateAsset(saved,path);
            }
            materials[source.GetInstanceID()]=saved;return saved;
        }
        public void SavePrefab(GameObject root,string path)
        {
            foreach(var filter in root.GetComponentsInChildren<MeshFilter>())filter.sharedMesh=Mesh(filter.sharedMesh);
            foreach(var renderer in root.GetComponentsInChildren<MeshRenderer>())renderer.sharedMaterial=Material(renderer.sharedMaterial);
            foreach(var collider in root.GetComponentsInChildren<Collider>())Object.DestroyImmediate(collider);
            PrefabUtility.SaveAsPrefabAsset(root,path);AssetDatabase.SaveAssets();
        }
    }
}
