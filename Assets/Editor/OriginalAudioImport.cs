using UnityEditor;
using UnityEngine;

namespace CodenameCheweenie.Editor
{
    public sealed class OriginalAudioImport : AssetPostprocessor
    {
        private void OnPreprocessAudio()
        {
            if (!assetPath.StartsWith("Assets/Resources/Audio/", System.StringComparison.Ordinal)) return;
            var importer = (AudioImporter)assetImporter;
            importer.forceToMono = false; importer.loadInBackground = false;
            var settings = importer.defaultSampleSettings;
            settings.preloadAudioData = true;
            settings.compressionFormat = AudioCompressionFormat.PCM;
            settings.loadType = AudioClipLoadType.DecompressOnLoad;
            settings.sampleRateSetting = AudioSampleRateSetting.PreserveSampleRate;
            importer.defaultSampleSettings = settings;
        }
    }
}
