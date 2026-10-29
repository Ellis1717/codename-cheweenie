using System.Collections.Generic;
using UnityEngine;

namespace CodenameCheweenie
{
    public sealed class WorldMaterials : MonoBehaviour
    {
        private static readonly Dictionary<Color, Material> Palette = new Dictionary<Color, Material>();
        public static Material For(Color color)
        {
            if (Palette.TryGetValue(color, out var material) && material) return material;
            var template = Resources.Load<Material>("Branding/World");
            if (template) material = new Material(template);
            else
            {
                var shader = UnityEngine.Rendering.GraphicsSettings.currentRenderPipeline == null ? Shader.Find("Standard") : Shader.Find("Universal Render Pipeline/Lit");
                if (!shader) throw new System.InvalidOperationException("Missing world material. Run Cheweenie > Prepare runtime materials before building.");
                material = new Material(shader);
            }
            material.color = color; material.enableInstancing = true; material.name = "Cheweenie palette " + ColorUtility.ToHtmlStringRGB(color);
            if (material.HasProperty("_Glossiness")) material.SetFloat("_Glossiness", .12f);
            Palette[color] = material; return material;
        }
        [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.SubsystemRegistration)]
        private static void Reset()
        {
            foreach (var material in Palette.Values) if (material) Destroy(material);
            Palette.Clear();
        }
        private void OnDestroy() { Reset(); }
    }
}
