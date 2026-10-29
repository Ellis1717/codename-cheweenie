using UnityEngine;
using System.Collections.Generic;

namespace CodenameCheweenie
{
    // Shared title/settings identity. Text stays live rather than being baked into the cover.
    public sealed class BrandTheme
    {
        public const string Title = "Codename .... Cheweenie";
        public static readonly Color Cream = new Color(.98f, .93f, .82f);
        public static readonly Color Amber = new Color(.96f, .69f, .34f);
        private static readonly Dictionary<(int, Color, bool, bool), GUIStyle> Styles = new Dictionary<(int, Color, bool, bool), GUIStyle>();
        [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.SubsystemRegistration)]
        private static void ResetStyles() { Styles.Clear(); }
        private readonly Texture2D cover;
        private readonly Texture2D shade;
        private readonly Texture2D circle;
        public BrandTheme()
        {
            cover = Resources.Load<Texture2D>("Branding/Cover");
            shade = new Texture2D(128, 1) { hideFlags = HideFlags.HideAndDontSave, wrapMode = TextureWrapMode.Clamp };
            for (int x = 0; x < 128; x++) shade.SetPixel(x, 0, new Color(.035f, .045f, .065f, Mathf.Lerp(.97f, .08f, x / 127f)));
            shade.Apply();
            circle = new Texture2D(64, 64) { hideFlags = HideFlags.HideAndDontSave, wrapMode = TextureWrapMode.Clamp };
            for (int y = 0; y < 64; y++) for (int x = 0; x < 64; x++)
                circle.SetPixel(x, y, new Color(1, 1, 1, Mathf.Clamp01(32 - Vector2.Distance(new Vector2(x, y), new Vector2(31.5f, 31.5f)))));
            circle.Apply();
        }
        public void Dispose() { Object.Destroy(shade); Object.Destroy(circle); }
        public void Background(float width, float height)
        {
            Fill(new Rect(0, 0, width, height), new Color(.06f, .08f, .11f));
            GUI.color = Color.white;
            if (cover) GUI.DrawTexture(new Rect(0, 0, width, height), cover, ScaleMode.ScaleAndCrop);
            GUI.DrawTexture(new Rect(0, 0, width, height), shade, ScaleMode.StretchToFill);
        }
        public static void Fill(Rect rect, Color color)
        {
            Color old = GUI.color; GUI.color = color; GUI.DrawTexture(rect, Texture2D.whiteTexture); GUI.color = old;
        }
        public void Paw(float x, float y, float size)
        {
            Color old = GUI.color; GUI.color = Amber;
            GUI.DrawTexture(new Rect(x + size * .23f, y + size * .43f, size * .55f, size * .48f), circle);
            for (int i = 0; i < 4; i++)
                GUI.DrawTexture(new Rect(x + i * size * .23f, y + (i == 0 || i == 3 ? .25f : .08f) * size, size * .25f, size * .3f), circle);
            GUI.color = old;
        }
        public static GUIStyle Text(int size, Color color, bool bold = false)
        { return Style(size, color, bold, false); }
        private static GUIStyle Style(int size, Color color, bool bold, bool button)
        {
            var key = (size, color, bold, button);
            if (Styles.TryGetValue(key, out var existing)) return existing;
            var style = new GUIStyle(GUI.skin.label) { fontSize = size, wordWrap = true, fontStyle = bold ? FontStyle.Bold : FontStyle.Normal };
            foreach (var state in new[] { style.normal, style.hover, style.active, style.focused, style.onNormal, style.onHover, style.onActive, style.onFocused })
            { state.textColor = color; state.background = null; }
            if (button) { style.alignment = TextAnchor.MiddleLeft; style.padding = new RectOffset(22, 14, 0, 0); }
            Styles[key] = style; return style;
        }
        public static bool Button(Rect rect, string text, bool selected, bool primary = false)
        {
            bool hovered = rect.Contains(GameControls.PointerPosition(GUI.matrix));
            Fill(rect, primary || selected || hovered ? Amber : new Color(.09f, .12f, .16f, .94f));
            var style = Style(20, primary || selected || hovered ? new Color(.10f, .09f, .08f) : Cream, true, true);
            GUI.Label(rect, text, style);
            return GameControls.TryPointerClick(rect, GUI.matrix);
        }
    }
}
