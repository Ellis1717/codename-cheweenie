Shader "Cheweenie/VisualLab/SoftFur"
{
 Properties { _MainTex("Coat",2D)="white"{} _Color("Tint",Color)=(1,1,1,1) _Density("Strands per metre",Float)=210 _Cutoff("Cutoff",Range(0,1))=.5 }
 SubShader {
 Tags { "RenderType"="TransparentCutout" "Queue"="AlphaTest+2" }
 Cull Off
 CGPROGRAM
 #pragma surface surf Standard fullforwardshadows vertex:vert alphatest:_Cutoff
 #pragma target 3.0
 sampler2D _MainTex; fixed4 _Color;float _Density;
 struct Input {float2 uv_MainTex;float3 furPos;float layer;};
 void vert(inout appdata_full v,out Input o){UNITY_INITIALIZE_OUTPUT(Input,o);o.furPos=v.texcoord2.xyz;o.layer=v.texcoord1.x;}
 void surf(Input i,inout SurfaceOutputStandard o){
 float3 cell=floor(i.furPos*_Density);
 float strand=frac(sin(dot(cell,float3(12.9898,78.233,37.719)))*43758.5453);
 clip(strand-lerp(.12,.96,i.layer));
 fixed3 coat=tex2D(_MainTex,i.uv_MainTex).rgb*_Color.rgb;
 o.Albedo=coat*lerp(.94,1.03,i.layer);o.Metallic=0;o.Smoothness=.12;o.Occlusion=1;o.Alpha=1;
 }
 ENDCG
 }
 Fallback "Transparent/Cutout/Diffuse"
}
