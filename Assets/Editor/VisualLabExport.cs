using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Text;
using UnityEditor;
using UnityEngine;
namespace CodenameCheweenie.Editor
{
    public static class VisualLabExport
    {
        [Serializable] private class ExportMaterial {public string key;public Color color;public string texture;}
        [Serializable] private class ExportManifest {public string name;public List<ExportMaterial> materials=new List<ExportMaterial>();}
        public static void Export()
        {
            string folder="Unreal/VisualLab/SourceAssets";Directory.CreateDirectory(folder);
            Write("Marty","Assets/Art/Marty/MartyPhotoGuided.prefab",folder);
            Write("Lakitu","Assets/Resources/Cats/LakituVisual.prefab",folder);
            Write("Rio","Assets/Resources/Cats/RioVisual.prefab",folder);
        }
        private static string F(float value)=>value.ToString("R",CultureInfo.InvariantCulture);
        private static Vector3 ToUnreal(Vector3 p)=>new Vector3(p.z*100,p.x*100,p.y*100);
        private static void Write(string name,string path,string folder)
        {
            var prefab=AssetDatabase.LoadAssetAtPath<GameObject>(path);if(!prefab)throw new InvalidOperationException("Missing export source "+path);
            var filters=prefab.GetComponentsInChildren<MeshFilter>();float min=float.MaxValue;
            foreach(var filter in filters){var matrix=prefab.transform.worldToLocalMatrix*filter.transform.localToWorldMatrix;foreach(var vertex in filter.sharedMesh.vertices)min=Mathf.Min(min,matrix.MultiplyPoint3x4(vertex).y);}
            var obj=new StringBuilder("# Original owned pet geometry, Unity metres converted to Unreal centimetres\nmtllib "+name+".mtl\n");var mtl=new StringBuilder();var manifest=new ExportManifest{name=name};var materials=new Dictionary<Material,int>();int offset=1;
            foreach(var filter in filters)
            {
                var renderer=filter.GetComponent<MeshRenderer>();if(!renderer)continue;var mesh=filter.sharedMesh;var points=mesh.vertices;var normals=mesh.normals;var uv=mesh.uv;var matrix=prefab.transform.worldToLocalMatrix*filter.transform.localToWorldMatrix;
                obj.AppendLine("g part"+offset);
                foreach(var vertex in points){var p=matrix.MultiplyPoint3x4(vertex);p.y-=min;p=ToUnreal(p);obj.AppendLine("v "+F(p.x)+" "+F(p.y)+" "+F(p.z));}
                for(int i=0;i<points.Length;i++){var p=uv.Length==points.Length?uv[i]:Vector2.zero;obj.AppendLine("vt "+F(p.x)+" "+F(p.y));}
                for(int i=0;i<points.Length;i++){var n=normals.Length==points.Length?matrix.inverse.transpose.MultiplyVector(normals[i]).normalized:Vector3.up;n=ToUnreal(n).normalized;obj.AppendLine("vn "+F(n.x)+" "+F(n.y)+" "+F(n.z));}
                for(int sub=0;sub<mesh.subMeshCount;sub++)
                {
                    var material=renderer.sharedMaterials[Mathf.Min(sub,renderer.sharedMaterials.Length-1)];
                    if(!materials.TryGetValue(material,out int index))
                    {
                        index=materials.Count;materials[material]=index;string key="M"+index;var color=material.color.linear;var entry=new ExportMaterial{key=key,color=color,texture=""};
                        if(material.mainTexture){string texturePath=AssetDatabase.GetAssetPath(material.mainTexture);if(File.Exists(texturePath)){entry.texture=name+"_"+key+Path.GetExtension(texturePath);File.Copy(texturePath,Path.Combine(folder,entry.texture),true);}}
                        manifest.materials.Add(entry);mtl.AppendLine("newmtl "+key+"\nKd "+F(color.r)+" "+F(color.g)+" "+F(color.b)+"\nNs 25\n");
                    }
                    obj.AppendLine("usemtl M"+index);var triangles=mesh.GetTriangles(sub);
                    for(int i=0;i<triangles.Length;i+=3){obj.Append("f");for(int j=0;j<3;j++){int v=triangles[i+j]+offset;obj.Append(" "+v+"/"+v+"/"+v);}obj.AppendLine();}
                }
                offset+=points.Length;
            }
            File.WriteAllText(Path.Combine(folder,name+".obj"),obj.ToString());File.WriteAllText(Path.Combine(folder,name+".mtl"),mtl.ToString().TrimEnd()+"\n");File.WriteAllText(Path.Combine(folder,name+".json"),JsonUtility.ToJson(manifest,true));
            Debug.Log("Exported portable static comparison asset "+name+". This does not export animation/quests.");
        }
    }
}
