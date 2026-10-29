using System.Collections.Generic;
using UnityEngine;

namespace CodenameCheweenie
{
    public enum CatLook { Lakitu, Rio }

    // Original stylized geometry and coat paint, guided by the owner's private cat photos.
    public sealed class CatVisual : MonoBehaviour
    {
        public CatLook look;
        private Transform model;
        private readonly List<Mesh> meshes = new List<Mesh>();
        private readonly List<Transform> paws = new List<Transform>(), eyes = new List<Transform>();
        private Texture2D coat;
        private Material faceMaterial;
        private Vector3 previousPosition;
        private float phase;
        public static readonly Color BlackFur = new Color(.025f, .03f, .035f);
        public static readonly Color WhiteFur = new Color(.94f, .94f, .93f);
        public static Color FaceCoat(CatLook cat, Vector3 direction)
        {
            Vector3 p = direction.normalized;
            if (cat == CatLook.Rio) return p.y < -.45f && p.z > .25f ? WhiteFur : BlackFur;
            bool dark = p.y > .44f || p.z < -.1f && p.y > -.35f;
            float patch = Mathf.Pow((p.x + .48f) / .65f, 2) + Mathf.Pow((p.y - .10f) / .66f, 2);
            if (patch < 1 && p.x < -.06f && p.z > .1f && p.y > -.36f) dark = true;
            if (Mathf.Abs(p.x) < .07f + .06f * (.7f - p.y) && p.z > .25f && p.y > -.25f && p.y < .76f) dark = false;
            return dark ? BlackFur : WhiteFur;
        }
        private void Start()
        {
            model = new GameObject(look + " | reference-based cat rig").transform; model.SetParent(transform, false);
            bool lakitu = look == CatLook.Lakitu;
            Part("Coat body", new Vector3(0, -.13f, -.02f), new Vector3(lakitu ? .64f : .72f, .56f, lakitu ? .94f : 1.02f), lakitu ? WhiteFur : BlackFur);
            if (lakitu) Part("Black saddle patch", new Vector3(0, .02f, -.12f), new Vector3(.62f, .36f, .8f), BlackFur);
            Part("White chest bib", new Vector3(0, -.02f, .41f), new Vector3(lakitu ? .5f : .43f, .45f, .18f), WhiteFur);
            BuildFace();
            for (int side = -1; side <= 1; side += 2)
            {
                Ear(side);
                var eye = new GameObject("Blinking eye").transform; eye.SetParent(model, false); eye.localPosition = new Vector3(side * .17f, .32f, .58f); eyes.Add(eye);
                Part("Eye rim", Vector3.zero, new Vector3(.126f, .14f, .04f), BlackFur, eye);
                Part("Olive iris", new Vector3(0, 0, .022f), new Vector3(.09f, .105f, .025f), new Color(.56f, .6f, .3f), eye);
                Part("Pupil", new Vector3(0, 0, .037f), new Vector3(.042f, .082f, .018f), Color.black, eye);
                Part("Eye glint", new Vector3(-.016f, .023f, .048f), Vector3.one * .022f, Color.white, eye);
                Part("Muzzle", new Vector3(side * .058f, .205f, .615f), new Vector3(.12f, .1f, .085f), lakitu ? WhiteFur : BlackFur);
                for (int line = 0; line < 3; line++)
                {
                    Vector3 a = new Vector3(side * .09f, .20f, .64f), b = new Vector3(side * .37f, .21f + (line - 1) * .055f, .65f);
                    var whisker = Part("White whisker", (a + b) * .5f, new Vector3(.007f, .007f, Vector3.Distance(a, b)), WhiteFur);
                    whisker.localRotation = Quaternion.LookRotation(b - a);
                }
            }
            Part("Nose", new Vector3(0, .235f, .666f), new Vector3(.075f, .052f, .045f), lakitu ? new Color(.8f, .48f, .51f) : new Color(.05f, .035f, .035f));
            Part("White chin", new Vector3(0, .15f, .62f), new Vector3(.16f, .075f, .07f), WhiteFur);
            if (lakitu)
            {
                Part("Dark collar", new Vector3(0, .02f, .33f), new Vector3(.49f, .065f, .32f), new Color(.22f, .1f, .11f));
                Part("Round collar tag", new Vector3(0, -.035f, .52f), new Vector3(.085f, .1f, .026f), new Color(.64f, .55f, .3f));
            }
            for (int i = 0; i < 4; i++) paws.Add(Part("Paw " + i, new Vector3(i % 2 == 0 ? -.21f : .21f, -.4f, i < 2 ? .28f : -.31f), new Vector3(.17f, .2f, .24f), lakitu ? WhiteFur : BlackFur));
            Vector3[] tail = lakitu ? new[] { new Vector3(0, -.03f, -.48f), new Vector3(0, .22f, -.68f), new Vector3(-.12f, .5f, -.68f) }
                : new[] { new Vector3(0, -.08f, -.5f), new Vector3(.3f, -.03f, -.64f), new Vector3(.45f, .1f, -.4f) };
            for (int i = 1; i < tail.Length; i++)
            {
                var piece = Part("Black tail", (tail[i - 1] + tail[i]) * .5f, new Vector3(.12f, .12f, Vector3.Distance(tail[i - 1], tail[i]) + .12f), BlackFur);
                piece.localRotation = Quaternion.LookRotation(tail[i] - tail[i - 1]);
            }
            previousPosition = transform.position;
        }
        private void BuildFace()
        {
            const int width = 256, height = 128;
            coat = new Texture2D(width, height, TextureFormat.RGB24, false) { name = look + " photo-guided coat", wrapMode = TextureWrapMode.Repeat, filterMode = FilterMode.Bilinear };
            var pixels = new Color[width * height];
            for (int y = 0; y < height; y++) for (int x = 0; x < width; x++)
            {
                float theta = (1 - (y + .5f) / height) * Mathf.PI, phi = (x + .5f) / width * Mathf.PI * 2;
                pixels[y * width + x] = FaceCoat(look, new Vector3(Mathf.Sin(theta) * Mathf.Cos(phi), Mathf.Cos(theta), Mathf.Sin(theta) * Mathf.Sin(phi)));
            }
            coat.SetPixels(pixels); coat.Apply();
            coat.wrapModeV = TextureWrapMode.Clamp;
            faceMaterial = new Material(WorldMaterials.For(Color.white)) { name = look + " face material", mainTexture = coat };
            const int rings = 20, segments = 32;
            var vertices = new Vector3[(rings + 1) * (segments + 1)]; var uv = new Vector2[vertices.Length]; var triangles = new int[rings * segments * 6];
            int index = 0;
            for (int r = 0; r <= rings; r++) for (int s = 0; s <= segments; s++)
            {
                float theta = r / (float)rings * Mathf.PI, phi = s / (float)segments * Mathf.PI * 2;
                int at = r * (segments + 1) + s;
                vertices[at] = new Vector3(Mathf.Sin(theta) * Mathf.Cos(phi), Mathf.Cos(theta), Mathf.Sin(theta) * Mathf.Sin(phi)) * .5f; uv[at] = new Vector2(s / (float)segments, 1 - r / (float)rings);
                if (r == rings || s == segments) continue;
                int b = at + segments + 1;
                triangles[index++] = at; triangles[index++] = at + 1; triangles[index++] = b;
                triangles[index++] = at + 1; triangles[index++] = b + 1; triangles[index++] = b;
            }
            var mesh = new Mesh { name = look + " UV head", vertices = vertices, uv = uv, triangles = triangles }; mesh.RecalculateNormals(); mesh.RecalculateBounds(); meshes.Add(mesh);
            var head = new GameObject("Reference-patterned head"); head.transform.SetParent(model, false); head.transform.localPosition = new Vector3(0, .24f, .36f); head.transform.localScale = new Vector3(.56f, .5f, .56f);
            head.AddComponent<MeshFilter>().sharedMesh = mesh; head.AddComponent<MeshRenderer>().sharedMaterial = faceMaterial;
        }
        private void Ear(int side)
        {
            var mesh = new Mesh { name = "Pointed cat ear" };
            mesh.vertices = new[] { new Vector3(-.11f, 0, .035f), new Vector3(.11f, 0, .035f), new Vector3(side * .035f, .25f, .025f), new Vector3(-.11f, 0, -.035f), new Vector3(.11f, 0, -.035f), new Vector3(side * .035f, .25f, -.025f) };
            mesh.triangles = new[] { 0, 1, 2, 3, 5, 4, 0, 3, 4, 0, 4, 1, 1, 4, 5, 1, 5, 2, 2, 5, 3, 2, 3, 0 }; mesh.RecalculateNormals(); meshes.Add(mesh);
            var ear = new GameObject("Pointed black ear"); ear.transform.SetParent(model, false); ear.transform.localPosition = new Vector3(side * .19f, .44f, .32f);
            ear.AddComponent<MeshFilter>().sharedMesh = mesh; ear.AddComponent<MeshRenderer>().sharedMaterial = WorldMaterials.For(BlackFur);
            Part("Inner ear", new Vector3(side * .19f, .53f, .37f), new Vector3(.105f, .17f, .025f), new Color(.37f, .24f, .25f));
        }
        private Transform Part(string label, Vector3 position, Vector3 scale, Color color, Transform parent = null)
        {
            var go = GameObject.CreatePrimitive(PrimitiveType.Sphere); go.name = label; go.transform.SetParent(parent ? parent : model, false); go.transform.localPosition = position; go.transform.localScale = scale;
            go.GetComponent<Collider>().enabled = false; go.GetComponent<Renderer>().sharedMaterial = WorldMaterials.For(color); return go.transform;
        }
        private void LateUpdate()
        {
            if (!model) return;
            float speed = Vector3.Distance(transform.position, previousPosition) / Mathf.Max(.001f, Time.deltaTime); previousPosition = transform.position;
            phase += Mathf.Min(speed, 4) * Time.deltaTime * 6;
            for (int i = 0; i < paws.Count; i++) paws[i].localRotation = Quaternion.Euler(Mathf.Sin(phase + (i == 0 || i == 3 ? 0 : Mathf.PI)) * Mathf.Min(speed * 10, 22), 0, 0);
            float blink = Mathf.Repeat(Time.time + (look == CatLook.Rio ? 2 : 0), 5.8f) > 5.63f ? .12f : 1;
            foreach (var eye in eyes) eye.localScale = new Vector3(1, blink, 1);
        }
        private void OnDestroy()
        { foreach (var mesh in meshes) if (mesh) Destroy(mesh); if (faceMaterial) Destroy(faceMaterial); if (coat) Destroy(coat); }
    }
}
