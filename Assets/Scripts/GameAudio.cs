using UnityEngine;

namespace CodenameCheweenie
{
    public sealed class GameAudio : MonoBehaviour
    {
        public AudioClip homeMusic, backyardMusic;
        private AudioSource homeSource, backyardSource, effectsSource;
        private readonly AudioClip[] cues = new AudioClip[9];
        private readonly float[] nextCue = new float[9];
        private FrontEndMenu frontEnd;
        private CozySettings settings;
        private MartyController dog;
        private float backyardBlend;
        private void Awake()
        {
            GameSave.Load();
            if (!homeMusic) homeMusic = Resources.Load<AudioClip>("Audio/Home");
            if (!backyardMusic) backyardMusic = Resources.Load<AudioClip>("Audio/Backyard");
            homeSource = MakeSource(true); backyardSource = MakeSource(true); effectsSource = MakeSource(false);
            homeSource.clip = homeMusic; backyardSource.clip = backyardMusic;
            foreach (SoundCue cue in System.Enum.GetValues(typeof(SoundCue)))
            {
                cues[(int)cue] = Resources.Load<AudioClip>("Audio/" + cue);
                if (!cues[(int)cue]) Debug.LogWarning("Missing sound asset: Audio/" + cue);
            }
            double start = AudioSettings.dspTime + .15;
            if (homeMusic) homeSource.PlayScheduled(start);
            if (backyardMusic) backyardSource.PlayScheduled(start);
            if (!homeMusic || !backyardMusic) Debug.LogWarning("Music assets are missing. Run Validation/AudioAssets.csproj to regenerate them.");
        }
        private AudioSource MakeSource(bool loop)
        {
            var source = gameObject.AddComponent<AudioSource>(); source.playOnAwake = false;
            source.loop = loop; source.spatialBlend = 0; source.volume = 0; source.ignoreListenerPause = true;
            source.priority = loop ? 64 : 128;
            return source;
        }
        private void Update()
        {
            if (!frontEnd) frontEnd = GetComponent<FrontEndMenu>();
            if (!settings) settings = GetComponent<CozySettings>();
            if (!dog) dog = Object.FindFirstObjectByType<MartyController>();
            bool title = frontEnd && frontEnd.ShowingTitle;
            float target = !title && dog ? Mathf.Clamp01((-5 - dog.transform.position.z) / 5f) : 0;
            backyardBlend = Mathf.MoveTowards(backyardBlend, target, Time.unscaledDeltaTime * .55f);
            float level = AudioPreferences.Unit(GameSave.Data.musicVolume, AudioPreferences.DefaultMusic);
            if (settings && settings.IsOpen) level *= .7f;
            if (!title && dog && dog.IsCozy) level *= .65f;
            // Both forty-second loops share a tempo and DSP start, so transitions stay in phase.
            homeSource.volume = Mathf.Cos(backyardBlend * Mathf.PI * .5f) * level;
            backyardSource.volume = Mathf.Sin(backyardBlend * Mathf.PI * .5f) * level;
            effectsSource.volume = AudioPreferences.Unit(GameSave.Data.volume, .6f);
        }
        public void Play(SoundCue cue)
        {
            int index = (int)cue;
            if (index < 0 || index >= cues.Length || !cues[index]) return;
            if (cue != SoundCue.Menu && Time.timeScale <= 0) return;
            if (Time.unscaledTime < nextCue[index]) return;
            nextCue[index] = Time.unscaledTime + (cue == SoundCue.Step ? .12f : .06f);
            effectsSource.volume = AudioPreferences.Unit(GameSave.Data.volume, .6f);
            effectsSource.PlayOneShot(cues[index]);
        }
        private void OnDestroy()
        {
            if (homeSource) { homeSource.Stop(); Destroy(homeSource); }
            if (backyardSource) { backyardSource.Stop(); Destroy(backyardSource); }
            if (effectsSource) { effectsSource.Stop(); Destroy(effectsSource); }
        }
    }
}
