using NUnit.Framework;
using UnityEngine;

namespace CodenameCheweenie.Tests
{
    public sealed class RetainedWorldArtTests
    {
        [Test] public void BothCatPrefabsRetainTheirOwnCoatAndFourAnimatedLegs()
        {
            foreach(CatLook look in System.Enum.GetValues(typeof(CatLook)))
            {
                var prefab=Resources.Load<GameObject>("Cats/"+look+"Visual");Assert.IsNotNull(prefab);var rig=prefab.GetComponent<CatModelRig>();Assert.IsNotNull(rig);Assert.AreEqual(look,rig.look);
                Assert.AreEqual(4,rig.legs.Length);Assert.AreEqual(2,rig.eyes.Length);Assert.IsNotNull(rig.tail);Assert.AreEqual(0,prefab.GetComponentsInChildren<Collider>().Length);Assert.AreEqual(0,prefab.GetComponentsInChildren<Rigidbody>().Length);
                var face=prefab.transform.Find("Reference-patterned head").GetComponent<Renderer>().sharedMaterial;Assert.IsNotNull(face.shader);Assert.IsNotNull(face.mainTexture);Assert.AreEqual(256,face.mainTexture.width);Assert.AreEqual(128,face.mainTexture.height);
                Assert.Less(prefab.GetComponentsInChildren<Renderer>().Length,55);
            }
        }
        [Test] public void SceneryUsesRetainedTexturedPropsWithoutHiddenGameplayPhysics()
        {
            var palette=Resources.Load<SceneryPalette>("SceneryPalette");Assert.IsNotNull(palette);
            foreach(var material in new[]{palette.wood,palette.grass,palette.stone,palette.paving,palette.brick,palette.plaster,palette.fabric,palette.roof,palette.leaf,palette.glass,palette.metal,palette.warmLight}){Assert.IsNotNull(material);Assert.IsNotNull(material.shader);Assert.IsNotNull(material.mainTexture);}
            foreach(var prop in new[]{palette.sofa,palette.table,palette.island,palette.cushion,palette.fence,palette.tree,palette.flowers,palette.window,palette.bench,palette.playground,palette.rug,palette.gabledRoof,palette.shrub})
            {
                Assert.IsNotNull(prop);Assert.AreEqual(0,prop.GetComponentsInChildren<Collider>().Length);Assert.AreEqual(0,prop.GetComponentsInChildren<Rigidbody>().Length);
                int vertices=0;foreach(var filter in prop.GetComponentsInChildren<MeshFilter>()){Assert.IsNotNull(filter.sharedMesh);vertices+=filter.sharedMesh.vertexCount;Assert.Greater(filter.sharedMesh.bounds.size.sqrMagnitude,0);}Assert.Less(vertices,40000);
            }
        }
    }
}
