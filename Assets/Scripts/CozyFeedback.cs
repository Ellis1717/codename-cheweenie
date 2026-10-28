using UnityEngine;
namespace CodenameCheweenie
{
    public sealed class CozyFeedback : MonoBehaviour
    {
        private AudioSource source;
        private AudioClip chime;
        private Material sparkMaterial;
        private void Awake()
        {
            source = gameObject.AddComponent<AudioSource>(); source.playOnAwake = false;
            const int rate = 22050; var samples = new float[rate / 3];
            for (int i = 0; i < samples.Length; i++)
            {
                float t = (float)i / rate;
                samples[i] = Mathf.Sin(t * 2 * Mathf.PI * 660) * Mathf.Exp(-t * 14) * .18f;
            }
            chime = AudioClip.Create("Original cozy chime", samples.Length, 1, rate, false); chime.SetData(samples, 0);
            var shader = UnityEngine.Rendering.GraphicsSettings.currentRenderPipeline == null
                ? Shader.Find("Particles/Standard Unlit") : Shader.Find("Universal Render Pipeline/Particles/Unlit");
            if (shader) sparkMaterial = new Material(shader);
        }
        public void Play(Vector3 position)
        {
            GameSave.Load(); source.PlayOneShot(chime, GameSave.Data.volume);
            if (!GameSave.Data.particles || !sparkMaterial) return;
            var go = new GameObject("Cozy sparkles"); go.transform.position = position;
            var system = go.AddComponent<ParticleSystem>(); system.Stop(true, ParticleSystemStopBehavior.StopEmittingAndClear);
            var main = system.main; main.loop = false; main.startLifetime = 1.2f; main.startSpeed = .7f;
            main.startSize = .09f; main.startColor = new Color(1, .8f, .4f); main.maxParticles = 20;
            var emission = system.emission; emission.enabled = false;
            system.GetComponent<ParticleSystemRenderer>().sharedMaterial = sparkMaterial;
            system.Emit(16); Destroy(go, 2);
        }
        private void OnDestroy() { if (chime) Destroy(chime); if (sparkMaterial) Destroy(sparkMaterial); }
    }
}
