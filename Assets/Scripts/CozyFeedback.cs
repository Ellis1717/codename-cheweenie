using UnityEngine;
namespace CodenameCheweenie
{
    public sealed class CozyFeedback : MonoBehaviour
    {
        private GameAudio audioBus;
        private Material sparkMaterial;
        private void Awake()
        {
            audioBus = GetComponent<GameAudio>();
            var template = Resources.Load<Material>("Branding/Sparkles");
            if (template) sparkMaterial = new Material(template);
            else
            {
                var shader = UnityEngine.Rendering.GraphicsSettings.currentRenderPipeline == null
                    ? Shader.Find("Particles/Standard Unlit") : Shader.Find("Universal Render Pipeline/Particles/Unlit");
                if (shader) sparkMaterial = new Material(shader);
            }
        }
        public void Play(Vector3 position, SoundCue cue = SoundCue.Discovery)
        {
            GameSave.Load(); if (audioBus) audioBus.Play(cue);
            if (!GameSave.Data.particles || !sparkMaterial) return;
            var go = new GameObject("Cozy sparkles"); go.transform.position = position;
            var system = go.AddComponent<ParticleSystem>(); system.Stop(true, ParticleSystemStopBehavior.StopEmittingAndClear);
            var main = system.main; main.loop = false; main.startLifetime = 1.2f; main.startSpeed = .7f;
            main.startSize = .09f; main.startColor = new Color(1, .8f, .4f); main.maxParticles = 20;
            var emission = system.emission; emission.enabled = false;
            system.GetComponent<ParticleSystemRenderer>().sharedMaterial = sparkMaterial;
            system.Emit(16); Destroy(go, 2);
        }
        private void OnDestroy() { if (sparkMaterial) Destroy(sparkMaterial); }
    }
}
