using System.Collections.Generic;
using System.IO;
using UnityEditor;
using UnityEngine;

namespace CodenameCheweenie.Editor
{
    public static class CatModelBuilder
    {
        public static void EnsureModels()
        {
            if(!AssetDatabase.LoadAssetAtPath<GameObject>("Assets/Resources/Cats/LakituVisual.prefab")||!AssetDatabase.LoadAssetAtPath<GameObject>("Assets/Resources/Cats/RioVisual.prefab"))Create();
        }
        [MenuItem("Cheweenie/Art/Create or rebuild retained cat characters")]
        public static void Create()
        {
            Directory.CreateDirectory("Assets/Resources/Cats");AssetDatabase.Refresh();
            foreach(CatLook look in System.Enum.GetValues(typeof(CatLook)))
            {
                var owner=new GameObject("Cat authoring only");
                try
                {
                    var visual=owner.AddComponent<CatVisual>();visual.look=look;var model=visual.BuildAuthoringModel();
                    var eyes=new List<Transform>();var paws=new List<Transform>();var tails=new List<Transform>();
                    foreach(Transform child in model){if(child.name=="Blinking eye")eyes.Add(child);if(child.name.StartsWith("Paw "))paws.Add(child);if(child.name=="Black tail")tails.Add(child);}
                    var legs=new Transform[4];
                    for(int i=0;i<4;i++)
                    {
                        var hip=new GameObject("Articulated leg "+i).transform;hip.SetParent(model,false);hip.localPosition=new Vector3(paws[i].localPosition.x,-.22f,paws[i].localPosition.z);paws[i].SetParent(hip,true);legs[i]=hip;
                        var leg=GameObject.CreatePrimitive(PrimitiveType.Sphere);leg.name="Short cat leg";leg.transform.SetParent(hip,false);leg.transform.localPosition=new Vector3(0,-.09f,0);leg.transform.localScale=new Vector3(.14f,.25f,.16f);leg.GetComponent<Renderer>().sharedMaterial=WorldMaterials.For(look==CatLook.Lakitu?CatVisual.WhiteFur:CatVisual.BlackFur);
                    }
                    var tail=new GameObject("Tail joint").transform;tail.SetParent(model,false);tail.localPosition=new Vector3(0,-.04f,-.48f);foreach(var part in tails)part.SetParent(tail,true);
                    var rig=model.gameObject.AddComponent<CatModelRig>();rig.look=look;rig.legs=legs;rig.eyes=eyes.ToArray();rig.tail=tail;
                    var writer=new RetainedArtWriter("Assets/Art/Cats/"+look);writer.SavePrefab(model.gameObject,"Assets/Resources/Cats/"+look+"Visual.prefab");
                    Debug.Log("Retained "+look+" prefab, coat texture, meshes, materials and joints saved.");
                }
                finally{Object.DestroyImmediate(owner);}
            }
        }
    }
}
