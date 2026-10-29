using NUnit.Framework;
using UnityEngine;
namespace CodenameCheweenie.Tests
{
    public sealed class VisualLabMeshTests
    {
        [Test] public void FurShellsPreserveSourceAndBoundTheirExtrusion()
        {
            var source=new Mesh();source.vertices=new[]{Vector3.zero,Vector3.right,Vector3.forward};source.normals=new[]{Vector3.up,Vector3.up,Vector3.up};source.triangles=new[]{0,1,2};source.uv=new[]{Vector2.zero,Vector2.right,Vector2.up};var original=source.vertices;
            var shells=VisualLabFur.Shells(source,8,.02f,1);
            try{Assert.AreEqual(24,shells.vertexCount);Assert.AreEqual(24,shells.triangles.Length);Assert.AreEqual(3,source.vertexCount);CollectionAssert.AreEqual(original,source.vertices);foreach(var p in shells.vertices){Assert.Greater(p.y,0);Assert.LessOrEqual(p.y,.02001f);}Assert.IsFalse(VisualLab.Requested,"Normal editor/test launches retain the release visuals.");}
            finally{Object.DestroyImmediate(shells);Object.DestroyImmediate(source);}
        }
    }
}
