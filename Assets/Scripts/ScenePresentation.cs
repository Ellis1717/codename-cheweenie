using System.Collections.Generic;
using UnityEngine;

namespace CodenameCheweenie
{
    // Own the prototype's view during play without deleting the user's scene objects.
    public sealed class ScenePresentation : MonoBehaviour
    {
        private readonly List<Behaviour> suspended = new List<Behaviour>();
        private Color ambient;
        public void TakeOver()
        {
            ambient = RenderSettings.ambientLight;
            foreach (var camera in Object.FindObjectsByType<Camera>(FindObjectsSortMode.None)) Suspend(camera);
            foreach (var listener in Object.FindObjectsByType<AudioListener>(FindObjectsSortMode.None)) Suspend(listener);
            foreach (var light in Object.FindObjectsByType<Light>(FindObjectsSortMode.None)) if (light.type == LightType.Directional) Suspend(light);
        }
        private void Suspend(Behaviour component)
        {
            if (!component.enabled || !component.gameObject.scene.IsValid() || !component.gameObject.scene.isLoaded || component.transform.IsChildOf(transform)) return;
            suspended.Add(component); component.enabled = false;
        }
        private void OnDestroy()
        {
            foreach (var component in suspended) if (component) component.enabled = true;
            RenderSettings.ambientLight = ambient;
        }
    }
}
