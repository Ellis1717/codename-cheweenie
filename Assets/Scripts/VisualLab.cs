using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering;
namespace CodenameCheweenie
{
    // Separate experimental launch flag. The normal game and its retained defaults are unchanged.
    public sealed class VisualLab:MonoBehaviour
    {
        public static bool Requested=>Array.IndexOf(Environment.GetCommandLineArgs(),"-cheweenie-visual-lab")>=0;
        public bool Ready {get;private set;}
        private readonly List<UnityEngine.Object> owned=new List<UnityEngine.Object>();
        private int oldAA,oldPixelLights;private ShadowQuality oldShadows;private ShadowResolution oldResolution;private bool capturedQuality;
        public void Configure(CanineVisual dog){dog.visualPrefab=Resources.Load<GameObject>("VisualLab/MartyAnatomy");}
        private IEnumerator Start()
        {
            yield return null;
            oldAA=QualitySettings.antiAliasing;oldPixelLights=QualitySettings.pixelLightCount;oldShadows=QualitySettings.shadows;oldResolution=QualitySettings.shadowResolution;capturedQuality=true;
            var camera=Camera.main;camera.allowHDR=true;camera.allowMSAA=true;camera.fieldOfView=52;camera.gameObject.AddComponent<VisualLabPost>();
            QualitySettings.antiAliasing=4;QualitySettings.shadows=ShadowQuality.All;QualitySettings.shadowResolution=ShadowResolution.VeryHigh;QualitySettings.shadowDistance=80;QualitySettings.pixelLightCount=4;
            RenderSettings.ambientMode=AmbientMode.Trilight;RenderSettings.ambientSkyColor=new Color(.37f,.43f,.54f);RenderSettings.ambientEquatorColor=new Color(.27f,.24f,.22f);RenderSettings.ambientGroundColor=new Color(.14f,.12f,.10f);
            foreach(var light in GetComponentsInChildren<Light>())if(light.type==LightType.Directional){light.intensity=1.25f;light.color=new Color(1,.89f,.73f);light.transform.rotation=Quaternion.Euler(38,-42,0);light.shadows=LightShadows.Soft;light.shadowBias=.035f;}
            foreach(var dog in GetComponentsInChildren<CanineVisual>())dog.gameObject.AddComponent<VisualLabFur>().Apply(.014f);
            foreach(var cat in GetComponentsInChildren<CatVisual>())cat.gameObject.AddComponent<VisualLabFur>().Apply(.023f);
            foreach(var rig in GetComponentsInChildren<MartyModelRig>())RefineEyes(rig.eyes,.8f);
            foreach(var rig in GetComponentsInChildren<CatModelRig>())RefineEyes(rig.eyes,.8f);
            foreach(var renderer in GetComponentsInChildren<MeshRenderer>()){
                if(renderer.name.Contains("eye")||renderer.name.Contains("Eye")){var material=new Material(renderer.sharedMaterial);if(material.HasProperty("_Glossiness"))material.SetFloat("_Glossiness",.8f);renderer.sharedMaterial=material;owned.Add(material);}
            }
            var cloth=new Material(Resources.Load<Material>("Branding/World")){name="Lab woven blue blanket",color=new Color(.16f,.27f,.45f)};cloth.SetFloat("_Glossiness",.18f);
            var weave=WeaveNormal();cloth.SetTexture("_BumpMap",weave);cloth.EnableKeyword("_NORMALMAP");cloth.SetFloat("_BumpScale",.4f);cloth.SetTextureScale("_BumpMap",new Vector2(6,6));owned.Add(cloth);owned.Add(weave);
            DrapedCloth(cloth);
            LightAt("Warm fort lantern",new Vector3(-1.25f,1.1f,6.5f),new Color(1,.68f,.35f),2.3f,5);
            LightAt("Window bounce",new Vector3(-5,2.6f,1),new Color(.68f,.8f,1),1.3f,9);
            var probe=new GameObject("Local reflection probe").AddComponent<ReflectionProbe>();probe.transform.SetParent(transform,false);probe.transform.position=new Vector3(0,1.5f,3);probe.size=new Vector3(22,10,24);probe.mode=ReflectionProbeMode.Realtime;probe.refreshMode=ReflectionProbeRefreshMode.OnAwake;probe.resolution=128;probe.intensity=.45f;probe.timeSlicingMode=ReflectionProbeTimeSlicingMode.AllFacesAtOnce;
            Ready=true;
            int surfaces=0;foreach(var fur in GetComponentsInChildren<VisualLabFur>())surfaces+=fur.SurfaceCount;
            Debug.Log("VISUAL_LAB_READY fur_surfaces="+surfaces+" model=photo-guided alternative; save storage isolated by build product.");
        }
        private Texture2D WeaveNormal()
        {
            int size=128;var texture=new Texture2D(size,size,TextureFormat.RGBA32,false,true){name="Original woven normal",wrapMode=TextureWrapMode.Repeat};
            for(int y=0;y<size;y++)for(int x=0;x<size;x++){float a=Mathf.Sin(x*Mathf.PI/4)*.18f,b=Mathf.Sin(y*Mathf.PI/4)*.18f;var n=new Vector3(a,b,1).normalized;texture.SetPixel(x,y,new Color(n.x*.5f+.5f,n.y*.5f+.5f,n.z*.5f+.5f,1));}texture.Apply();return texture;
        }
        private void LightAt(string name,Vector3 position,Color color,float intensity,float range)
        {var light=new GameObject(name).AddComponent<Light>();light.transform.SetParent(transform,false);light.transform.position=position;light.type=LightType.Point;light.color=color;light.intensity=intensity;light.range=range;light.shadows=LightShadows.Soft;light.shadowStrength=.6f;}
        private void RefineEyes(Transform[] joints,float scale)
        {foreach(var joint in joints)foreach(Transform child in joint){child.localScale*=scale;child.localPosition*=scale;}}
        private void DrapedCloth(Material material)
        {
            int width=48,depth=40;var vertices=new Vector3[(width+1)*(depth+1)];var uv=new Vector2[vertices.Length];var triangles=new List<int>();
            for(int z=0;z<=depth;z++)for(int x=0;x<=width;x++){float u=x/(float)width,v=z/(float)depth;float edge=Mathf.Pow(Mathf.Abs(u-.5f)*2,3);float y=2.35f-edge*1.35f+Mathf.Sin(u*35+v*4)*.045f+Mathf.Sin(v*27)*.025f;int i=z*(width+1)+x;vertices[i]=new Vector3((u-.5f)*4,y,4.8f+v*3.8f);uv[i]=new Vector2(u*3,v*3);}
            for(int z=0;z<depth;z++)for(int x=0;x<width;x++){int a=z*(width+1)+x,b=a+width+1;triangles.Add(a);triangles.Add(b);triangles.Add(a+1);triangles.Add(a+1);triangles.Add(b);triangles.Add(b+1);}
            var mesh=new Mesh{name="Draped blanket canopy"};mesh.vertices=vertices;mesh.uv=uv;mesh.triangles=triangles.ToArray();mesh.RecalculateNormals();mesh.RecalculateBounds();owned.Add(mesh);
            var cloth=new GameObject("Soft folded fort canopy");cloth.transform.SetParent(transform,false);cloth.AddComponent<MeshFilter>().sharedMesh=mesh;cloth.AddComponent<MeshRenderer>().sharedMaterial=material;
            var back=Instantiate(mesh);back.name="Canopy underside";var reverse=back.triangles;for(int i=0;i<reverse.Length;i+=3){int t=reverse[i];reverse[i]=reverse[i+2];reverse[i+2]=t;}back.triangles=reverse;back.RecalculateNormals();owned.Add(back);
            var underside=new GameObject("Blanket underside");underside.transform.SetParent(cloth.transform,false);underside.AddComponent<MeshFilter>().sharedMesh=back;underside.AddComponent<MeshRenderer>().sharedMaterial=material;
            var starMaterial=new Material(material){name="Warm fairy glow",color=new Color(1,.69f,.3f)};starMaterial.EnableKeyword("_EMISSION");starMaterial.SetColor("_EmissionColor",new Color(2,1.1f,.35f));owned.Add(starMaterial);
            for(int i=0;i<11;i++){var glow=GameObject.CreatePrimitive(PrimitiveType.Sphere);glow.name="Fort fairy glow";glow.transform.SetParent(transform,false);glow.transform.position=new Vector3(-1.65f+i*.33f,1.75f+Mathf.Sin(i*.31f)*.1f,4.85f);glow.transform.localScale=Vector3.one*.055f;glow.GetComponent<Collider>().enabled=false;glow.GetComponent<Renderer>().sharedMaterial=starMaterial;}
        }
        private void OnGUI(){GUI.Label(new Rect(Screen.width-380,12,370,36),"VISUAL LAB · experimental · separate save",BrandTheme.Text(13,BrandTheme.Amber,true));}
        private void OnDestroy(){foreach(var asset in owned)if(asset)Destroy(asset);if(capturedQuality){QualitySettings.antiAliasing=oldAA;QualitySettings.pixelLightCount=oldPixelLights;QualitySettings.shadows=oldShadows;QualitySettings.shadowResolution=oldResolution;}}
    }
}
