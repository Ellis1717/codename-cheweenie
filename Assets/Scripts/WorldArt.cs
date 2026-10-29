using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering;

namespace CodenameCheweenie
{
    // Presentation layer over the existing tested layout/colliders. Private geography is not saved here.
    public sealed class WorldArt:MonoBehaviour
    {
        public bool Ready {get;private set;}
        public int PropCount {get;private set;}
        private SceneryPalette palette;
        private Transform art;
        public void Build(HashSet<GameObject> existingRoots)
        {
            palette=Resources.Load<SceneryPalette>("SceneryPalette");if(!palette)return;
            art=new GameObject("Retained cozy scenery | visual layer").transform;art.SetParent(transform,false);
            var renderers=Object.FindObjectsByType<MeshRenderer>(FindObjectsSortMode.None);
            foreach(var renderer in renderers)
            {
                if(existingRoots.Contains(renderer.transform.root.gameObject)||renderer.GetComponent<TextMesh>())continue;
                string label=renderer.name.ToLowerInvariant();var size=renderer.transform.lossyScale;
                if(label=="sofa"){Replace(renderer,palette.sofa);continue;}
                if(label=="coffee table"){Replace(renderer,palette.table);continue;}
                if(label=="kitchen island"){Replace(renderer,palette.island);continue;}
                if(label.Contains("cushion")){Replace(renderer,palette.cushion);continue;}
                if(label.Contains("garden fence")||label.Contains("entry fence")){Fence(renderer);continue;}
                if(label=="neighbour roof"||label=="gabled roof"){Roof(renderer);continue;}
                if(label=="front window"||label=="school window"){Window(renderer);continue;}
                if(label=="tree trunk")
                {
                    renderer.enabled=false;Spawn(palette.tree,new Vector3(renderer.transform.position.x,renderer.bounds.min.y,renderer.transform.position.z),Vector3.one,Quaternion.identity);continue;
                }
                if(label=="leaf canopy"){renderer.enabled=false;continue;}
                if(label.StartsWith("neighbour house")){Apply(renderer,palette.brick);HouseWindows(renderer);continue;}
                if(label.StartsWith("school |")){Apply(renderer,palette.brick);SchoolEntry(renderer);continue;}
                if(label.Contains("roof")){Apply(renderer,palette.roof);continue;}
                if(label.Contains("wall")||label.Contains("frame")||label.Contains("railing")||label.Contains("partition")){Apply(renderer,palette.plaster);continue;}
                if(label.Contains("floor")||label.Contains("ramp")||label.Contains("lookout")||label.Contains("bench")||label.Contains("planter")){Apply(renderer,palette.wood);continue;}
                if(label.Contains("hedge")){Apply(renderer,palette.leaf);continue;}
                if(label=="blanket canopy"){Apply(renderer,palette.fabric);FairyLights(renderer);continue;}
                if(label.Contains("stone")||label.Contains("pond border")){Apply(renderer,palette.stone);continue;}
                if(label=="patio"||label.Contains("pavement")||label.Contains("walking path")||label.Contains("gate path")){Apply(renderer,palette.paving);continue;}
                if(label.Contains("lawn")||label.Contains("green space")||label=="local walking ground"){Apply(renderer,palette.grass);continue;}
                if(renderer.bounds.max.y<.15f&&renderer.bounds.size.y<.3f)
                {
                    Color c=renderer.sharedMaterial.color;
                    if(c.g>c.r*1.15f)Apply(renderer,palette.grass);
                    else if(c.r>.55f&&c.g>.5f)Apply(renderer,palette.paving);
                    else if(c.r>.2f&&c.r<.5f&&c.b>.25f)Apply(renderer,palette.stone);
                }
            }
            Headquarters();Garden();Park();Lighting();Ready=true;
        }
        private void Apply(Renderer renderer,Material material)
        {
            renderer.sharedMaterial=material;
            Vector3 size=renderer.transform.lossyScale;
            var block=new MaterialPropertyBlock();block.SetVector("_MainTex_ST",new Vector4(Mathf.Clamp(Mathf.Abs(size.x)/2,1,128),Mathf.Clamp(Mathf.Abs(size.z)/2,1,128),0,0));renderer.SetPropertyBlock(block);
        }
        private GameObject Spawn(GameObject prefab,Vector3 position,Vector3 scale,Quaternion rotation)
        {
            var go=Instantiate(prefab,art);go.name=prefab.name+" | retained scenery";go.transform.SetPositionAndRotation(position,rotation);go.transform.localScale=scale;PropCount++;
            var lod=go.AddComponent<LODGroup>();lod.SetLODs(new[]{new LOD(prefab==palette.tree?.025f:.015f,go.GetComponentsInChildren<Renderer>())});lod.RecalculateBounds();
            return go;
        }
        private void Replace(Renderer source,GameObject prefab)
        {source.enabled=false;Spawn(prefab,source.transform.position,source.transform.lossyScale,source.transform.rotation);}
        private void Fence(Renderer source)
        {
            source.enabled=false;Vector3 size=source.transform.lossyScale;bool alongX=size.x>size.z;float length=alongX?size.x:size.z;int count=Mathf.CeilToInt(length/.9f);
            for(int i=0;i<count;i++)Spawn(palette.fence,source.transform.position+(alongX?Vector3.right:Vector3.forward)*((i+.5f)/count-.5f)*length,new Vector3(length/count,size.y/1.3f,1),Quaternion.Euler(0,alongX?0:90,0));
        }
        private void Roof(Renderer source)
        {
            var filter=source.GetComponent<MeshFilter>();Vector3 size=Vector3.Scale(filter.sharedMesh.bounds.size,source.transform.lossyScale);source.enabled=false;
            Spawn(palette.gabledRoof,new Vector3(source.transform.position.x,source.bounds.min.y+.02f,source.transform.position.z),new Vector3(size.x/1.1f,Mathf.Max(1.7f,size.y)/.6f,size.z/1.1f),source.transform.rotation);
        }
        private void Window(Renderer source)
        {
            Vector3 size=source.transform.lossyScale;source.enabled=false;bool side=size.x<size.z;
            Spawn(palette.window,source.transform.position,new Vector3(side?size.z:size.x,size.y,1),Quaternion.Euler(0,side?-90:180,0));
        }
        private void HouseWindows(Renderer source)
        {
            for(int side=-1;side<=1;side+=2)Spawn(palette.window,source.transform.TransformPoint(new Vector3(side*.27f,.03f,-.505f)),new Vector3(1.4f,1.5f,1),source.transform.rotation*Quaternion.Euler(0,180,0));
            Spawn(palette.window,source.transform.TransformPoint(new Vector3(0,-.22f,-.51f)),new Vector3(1.25f,2.0f,1),source.transform.rotation*Quaternion.Euler(0,180,0));
        }
        private void SchoolEntry(Renderer source)
        {
            Spawn(palette.window,source.transform.position+new Vector3(0,-.9f,-6.08f),new Vector3(2.8f,2.8f,1),Quaternion.Euler(0,180,0));
            Spawn(palette.flowers,source.transform.position+new Vector3(-4,-2.4f,-6.8f),Vector3.one,Quaternion.identity);Spawn(palette.flowers,source.transform.position+new Vector3(4,-2.4f,-6.8f),Vector3.one,Quaternion.identity);
        }
        private void Headquarters()
        {
            var layout=Resources.Load<HouseLayout>("HouseLayout");float width=layout?layout.width:17,depth=layout?layout.depth:13;
            for(int i=-1;i<=1;i++)Spawn(palette.window,new Vector3(i*4.7f,2.15f,3+depth*.5f-.15f),new Vector3(1.8f,1.45f,1),Quaternion.Euler(0,180,0));
            for(int side=-1;side<=1;side+=2)for(int i=0;i<2;i++)Spawn(palette.window,new Vector3(side*(width*.5f-.15f),2.15f,1+i*5),new Vector3(1.6f,1.4f,1),Quaternion.Euler(0,side<0?90:-90,0));
            Spawn(palette.rug,new Vector3(-3,.145f,4.3f),new Vector3(4.1f,1,3.2f),Quaternion.identity);
            Spawn(palette.flowers,new Vector3(7,.15f,8),new Vector3(.65f,.65f,.65f),Quaternion.identity);
        }
        private void Garden()
        {
            for(int side=-1;side<=1;side+=2){Spawn(palette.tree,new Vector3(side*20,0,-12),Vector3.one,Quaternion.identity);Spawn(palette.tree,new Vector3(side*20,0,6),new Vector3(.9f,.9f,.9f),Quaternion.identity);Spawn(palette.flowers,new Vector3(side*15,.43f,-9),new Vector3(1.2f,1.2f,1.2f),Quaternion.identity);}
            Spawn(palette.bench,new Vector3(-14,0,-17),Vector3.one,Quaternion.Euler(0,90,0));
            for(int i=0;i<6;i++)Spawn(palette.shrub,new Vector3(11,1.6f,-9.5f-i*1.4f),new Vector3(2.4f,.7f,1.5f),Quaternion.identity);
        }
        private void Park()
        {
            var map=MappedNeighbourhood.Current;Vector3 center=map!=null?map.Find(NeighbourhoodStop.Park).Position:new Vector3(18,0,12);
            Spawn(palette.playground,center+new Vector3(8,0,7),Vector3.one,Quaternion.identity);
            Spawn(palette.bench,center+new Vector3(-5,0,6),Vector3.one,Quaternion.Euler(0,90,0));
            for(int i=0;i<8;i++){float angle=i*Mathf.PI/4;Spawn(palette.tree,center+new Vector3(Mathf.Cos(angle)*17,0,Mathf.Sin(angle)*17),Vector3.one*(.85f+(i%3)*.1f),Quaternion.Euler(0,i*43,0));}
            for(int i=0;i<3;i++)Spawn(palette.flowers,center+new Vector3(-7+i*1.8f,0,9),Vector3.one,Quaternion.identity);
        }
        private void FairyLights(Renderer canopy)
        {
            // Emissive mesh accents are shared and have no point lights or physics.
            for(int i=0;i<6;i++)
            {
                var bulb=GameObject.CreatePrimitive(PrimitiveType.Sphere);bulb.name="Fort fairy light";bulb.transform.SetParent(art,false);bulb.transform.position=canopy.transform.position+new Vector3(-1.1f+i*.44f,-.16f,-1.35f);bulb.transform.localScale=Vector3.one*.09f;bulb.GetComponent<Collider>().enabled=false;bulb.GetComponent<Renderer>().sharedMaterial=palette.warmLight;
            }
        }
        private static void Lighting()
        {
            RenderSettings.ambientMode=AmbientMode.Trilight;RenderSettings.ambientSkyColor=new Color(.72f,.78f,.84f);RenderSettings.ambientEquatorColor=new Color(.63f,.66f,.58f);RenderSettings.ambientGroundColor=new Color(.38f,.34f,.29f);
            RenderSettings.fog=true;RenderSettings.fogMode=FogMode.Linear;RenderSettings.fogColor=new Color(.64f,.75f,.81f);RenderSettings.fogStartDistance=90;RenderSettings.fogEndDistance=300;QualitySettings.shadowDistance=65;
            var sun=GameObject.Find("Daylight")?.GetComponent<Light>();if(sun){sun.color=new Color(1,.92f,.80f);sun.shadows=LightShadows.Soft;sun.shadowStrength=.55f;sun.shadowBias=.06f;}
        }
    }
}
