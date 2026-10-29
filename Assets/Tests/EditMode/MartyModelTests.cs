using NUnit.Framework;
using UnityEngine;

namespace CodenameCheweenie.Tests
{
    public sealed class MartyModelTests
    {
        [Test] public void OriginalExplorerSourcesAndColourPaletteAreRetained()
        {
            const string source="Assets/Art/Marty/Source/Marty_Explorer.obj";
            var rig=Resources.Load<GameObject>("MartyVisual").GetComponent<MartyModelRig>();
            using(var hash=System.Security.Cryptography.SHA256.Create())
                Assert.AreEqual(rig.sourceSha256,System.BitConverter.ToString(hash.ComputeHash(System.IO.File.ReadAllBytes(source))).Replace("-","").ToLowerInvariant());
            var glb=System.IO.File.ReadAllBytes("Assets/Art/Marty/Source/Marty_Explorer.glb");
            Assert.AreEqual("glTF",System.Text.Encoding.ASCII.GetString(glb,0,4));
            var palette=new System.Collections.Generic.HashSet<string>();
            foreach(var renderer in rig.GetComponentsInChildren<Renderer>())palette.Add(ColorUtility.ToHtmlStringRGB(renderer.sharedMaterial.color));
            Assert.AreEqual(10,palette.Count);Assert.Contains("1997E5",new System.Collections.Generic.List<string>(palette));
        }
        [Test] public void SavedCharacterHasItsOwnMeshesAndArticulatedJoints()
        {
            var prefab=Resources.Load<GameObject>("MartyVisual");Assert.IsNotNull(prefab,"Build and retain the MartyVisual resource prefab.");
            var rig=prefab.GetComponent<MartyModelRig>();Assert.IsNotNull(rig);
            Assert.AreEqual(4,rig.legs.Length);Assert.AreEqual(4,rig.knees.Length);Assert.AreEqual(2,rig.ears.Length);Assert.AreEqual(2,rig.eyes.Length);
            Assert.IsNotNull(rig.tail);Assert.IsNotNull(rig.head);
            Assert.AreEqual(0,prefab.GetComponentsInChildren<Collider>().Length);Assert.AreEqual(0,prefab.GetComponentsInChildren<Rigidbody>().Length);
            Assert.AreEqual("Marty_Explorer.obj",rig.sourceModel);Assert.AreEqual(7666,rig.sourceVertexCount);Assert.AreEqual(15136,rig.sourceTriangleCount);Assert.AreEqual(64,rig.sourceSha256.Length);
        }
        [Test] public void SavedMeshesAreFiniteBoundedAndWithinThePrototypeBudget()
        {
            var prefab=Resources.Load<GameObject>("MartyVisual");Assert.IsNotNull(prefab);
            var renderers=prefab.GetComponentsInChildren<MeshRenderer>();Assert.Less(renderers.Length,45);Assert.Greater(renderers.Length,10);
            int vertices=0,triangles=0;
            foreach(var filter in prefab.GetComponentsInChildren<MeshFilter>())
            {
                var mesh=filter.sharedMesh;Assert.IsNotNull(mesh);vertices+=mesh.vertexCount;triangles+=mesh.triangles.Length/3;
                foreach(var vertex in mesh.vertices){Assert.IsFalse(float.IsNaN(vertex.x)||float.IsInfinity(vertex.x)||float.IsNaN(vertex.y)||float.IsInfinity(vertex.y)||float.IsNaN(vertex.z)||float.IsInfinity(vertex.z));Assert.Less(vertex.magnitude,3);}
                Assert.Greater(mesh.triangles.Length,0);Assert.Greater(mesh.bounds.size.sqrMagnitude,0);
                Assert.IsNotNull(filter.GetComponent<Renderer>().sharedMaterial.shader);
            }
            Assert.Less(vertices,40000);
            Assert.AreEqual(7666,vertices,"Retain every supplied vertex without replacing the mesh.");Assert.AreEqual(15136,triangles,"Retain every supplied source triangle.");
        }
    }
}
