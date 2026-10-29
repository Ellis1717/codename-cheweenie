using UnityEngine;
namespace CodenameCheweenie
{
    [RequireComponent(typeof(Camera))]
    public sealed class VisualLabPost:MonoBehaviour
    {
        private Material material;
        private void OnRenderImage(RenderTexture source,RenderTexture destination)
        {
            if(!material){var shader=Resources.Load<Shader>("VisualLab/Filmic");if(shader&&shader.isSupported)material=new Material(shader);}
            if(material){material.SetFloat("_Exposure",.85f);Graphics.Blit(source,destination,material);}else Graphics.Blit(source,destination);
        }
        private void OnDestroy(){if(material)Destroy(material);}
    }
}
