Shader "Hidden/Cheweenie/VisualLab/Filmic"
{
 Properties {_MainTex("Source",2D)="white"{}}
 SubShader {Cull Off ZWrite Off ZTest Always Pass {
 CGPROGRAM
 #pragma vertex vert_img
 #pragma fragment frag
 #include "UnityCG.cginc"
 sampler2D _MainTex;float _Exposure;
 float3 aces(float3 x){return saturate((x*(2.51*x+.03))/(x*(2.43*x+.59)+.14));}
 fixed4 frag(v2f_img i):SV_Target{
 float3 c=tex2D(_MainTex,i.uv).rgb*_Exposure;
 c*=float3(1.025,1.005,.985);
 c=aces(c);
 float2 p=i.uv-.5;c*=1-.13*dot(p,p);
 return float4(c,1);
 }
 ENDCG
 }}
}
