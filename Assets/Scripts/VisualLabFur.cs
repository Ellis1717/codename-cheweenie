using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering;
namespace CodenameCheweenie
{
    public sealed class VisualLabFur:MonoBehaviour
    {
        private readonly List<Mesh> meshes=new List<Mesh>();
        private readonly List<Material> materials=new List<Material>();
        public int SurfaceCount {get;private set;}
        public static Mesh Shells(Mesh source,int layers,float length,float scale)
        {
            var p=source.vertices;var n=source.normals;var uv=source.uv;var tris=source.triangles;
            var vertices=new List<Vector3>();var normals=new List<Vector3>();var coords=new List<Vector2>();var levels=new List<Vector2>();var rest=new List<Vector3>();var indices=new List<int>();
            for(int layer=1;layer<=layers;layer++){
                float t=layer/(float)layers;int offset=vertices.Count;
                for(int i=0;i<p.Length;i++){vertices.Add(p[i]+n[i]*length*t/Mathf.Max(.01f,scale));normals.Add(n[i]);coords.Add(uv.Length==p.Length?uv[i]:Vector2.zero);levels.Add(new Vector2(t,0));rest.Add(p[i]);}
                foreach(int index in tris)indices.Add(offset+index);
            }
            var mesh=new Mesh{name="Visual lab fur shells",indexFormat=IndexFormat.UInt32};mesh.SetVertices(vertices);mesh.SetNormals(normals);mesh.SetUVs(0,coords);mesh.SetUVs(1,levels);mesh.SetUVs(2,rest);mesh.SetTriangles(indices,0);mesh.RecalculateBounds();return mesh;
        }
        public void Apply(float length)
        {
            var shader=Resources.Load<Shader>("VisualLab/SoftFur");if(!shader||!shader.isSupported)return;
            var sources=GetComponentsInChildren<MeshRenderer>();
            foreach(var renderer in sources){
                string name=renderer.name.ToLowerInvariant();
                bool coat=name.Contains("coat")||name.Contains("saddle")||name.Contains("bib")||name.Contains("chin")||name.Contains("forehead")||name.Contains("muzzle")||name.Contains("body")||name.Contains("neck")||name.Contains("leg")||name.Contains("paw")||name.Contains("ear")||name.Contains("tail")||name.Contains("breast")||name.Contains("underside")||name.Contains("cheek")||name.Contains("head");
                if(!coat||name.Contains("nose")||name.Contains("eye")||name.Contains("collar")||name.Contains("claw")||name.Contains("inner"))continue;
                var filter=renderer.GetComponent<MeshFilter>();if(!filter||!filter.sharedMesh||!filter.sharedMesh.isReadable)continue;
                var original=renderer.sharedMaterial;if(!original)continue;
                var fur=new Material(shader){name="Lab short fur",color=original.color,mainTexture=original.mainTexture};
                fur.mainTextureScale=original.mainTextureScale;fur.mainTextureOffset=original.mainTextureOffset;
                var scale=renderer.transform.lossyScale;var mesh=Shells(filter.sharedMesh,8,length,Mathf.Max(scale.x,scale.y,scale.z));
                var child=new GameObject("Short fur | visual only");child.transform.SetParent(renderer.transform,false);
                child.AddComponent<MeshFilter>().sharedMesh=mesh;var surface=child.AddComponent<MeshRenderer>();surface.sharedMaterial=fur;surface.shadowCastingMode=ShadowCastingMode.Off;surface.receiveShadows=true;
                meshes.Add(mesh);materials.Add(fur);SurfaceCount++;
            }
        }
        private void OnDestroy(){foreach(var mesh in meshes)if(mesh)Destroy(mesh);foreach(var material in materials)if(material)Destroy(material);}
    }
}
