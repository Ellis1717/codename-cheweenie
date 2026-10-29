using UnityEngine;

namespace CodenameCheweenie
{
    // Default schematic block; an ignored local map can supply the real surrounding streets.
    public sealed class NeighbourhoodBuilder : MonoBehaviour
    {
        private readonly Color path = new Color(.72f, .7f, .6f), wood = new Color(.48f, .33f, .22f);
        public void Build()
        {
            if (MappedNeighbourhood.Current != null) { gameObject.AddComponent<MappedNeighbourhoodBuilder>().Build(MappedNeighbourhood.Current); return; }
            // Sit below the original lawn and street to avoid coplanar surfaces flickering.
            Box("Neighbourhood lawn", new Vector3(24, -.36f, -3), new Vector3(92, .5f, 64), new Color(.3f, .53f, .3f));
            Box("Quiet walking street", new Vector3(32, -.13f, -3), new Vector3(8, .06f, 54), new Color(.29f, .32f, .35f));
            Box("West pavement", new Vector3(26, -.1f, -3), new Vector3(4, .12f, 54), path);
            Box("East pavement", new Vector3(38, -.1f, -3), new Vector3(4, .12f, 54), path);
            Box("Garden gate path", new Vector3(21, -.1f, -13), new Vector3(10, .12f, 4), path);
            Box("Flower garden path", new Vector3(47, -.1f, -13), new Vector3(18, .12f, 4), path);
            Box("Park path", new Vector3(22, -.1f, 8), new Vector3(12, .12f, 4), path);
            Crossing(-13); Crossing(8);
            for (int i = 0; i < 3; i++) House(48, -24 + i * 20, i);
            for (int i = 0; i < 5; i++) Tree(16 + (i % 2) * 6, -2 + i * 6);
            Tree(58, -14); Tree(59, 12);
            Box("Park pond border", new Vector3(16, .08f, 12), new Vector3(5, .36f, 7), new Color(.57f, .56f, .48f));
            Box("Park pond water", new Vector3(16, .275f, 12), new Vector3(4.4f, .04f, 6.4f), new Color(.29f, .62f, .7f), false);
            Bench(21, 16); Bench(21, 3);
            for (int i = 0; i < 5; i++)
            {
                Box("Flower bed", new Vector3(43 + i * 2.2f, .03f, -9.5f), new Vector3(1.7f, .26f, 1.7f), wood);
                Sphere("Flowers", new Vector3(43 + i * 2.2f, .55f, -9.5f), new Vector3(1.2f, .55f, 1.2f), i % 2 == 0 ? new Color(.9f, .47f, .57f) : BrandTheme.Amber);
            }
            Box("Mailbox post", new Vector3(28.5f, .5f, -19), new Vector3(.16f, 1.2f, .16f), wood);
            Box("Blue mailbox", new Vector3(28.5f, 1.2f, -19), new Vector3(.7f, .45f, .7f), new Color(.28f, .45f, .68f));
            Sign("NEIGHBOURHOOD →", 18.5f, -10.5f);
            Sign("PARK", 24, 14); Sign("FLOWER GARDEN", 48, -11);
            Marker("Mailbox sniff stop", new Vector3(27, .65f, -19), NeighbourhoodStop.Mailbox, "Mailbox stamp! Marty catches a hundred interesting morning-walk smells.");
            Marker("Neighbour's flower garden", new Vector3(47, .65f, -10), NeighbourhoodStop.Garden, "Garden stamp! A bright row of flowers. Marty politely takes in all the new smells.");
            Marker("Little neighbourhood park", new Vector3(23, .65f, 12), NeighbourhoodStop.Park, "Park stamp! Grass, trees and a tiny pond. A perfect place for a Marty Party.");
            Marker("Welcome-home patrol sign", new Vector3(18.5f, .65f, -13), NeighbourhoodStop.Home, "Neighbourhood patrol complete! Mr. Marty is the best little walking buddy. Time for a cozy rest at home!");
            Box("East boundary hedge", new Vector3(66, .9f, -3), new Vector3(.6f, 2, 56), new Color(.16f, .34f, .22f));
            Box("North boundary hedge", new Vector3(42, .9f, 24), new Vector3(48, 2, .6f), new Color(.16f, .34f, .22f));
            Box("South boundary hedge", new Vector3(42, .9f, -30), new Vector3(48, 2, .6f), new Color(.16f, .34f, .22f));
        }
        private void Crossing(float z)
        { for (int i = 0; i < 7; i++) Box("Pedestrian crossing", new Vector3(29 + i, -.089f, z), new Vector3(.6f, .015f, 3), Color.white, false); }
        private void House(float x, float z, int index)
        {
            Color[] colors = { new Color(.78f, .6f, .47f), new Color(.53f, .65f, .73f), new Color(.75f, .73f, .49f) };
            Box("Neighbour house", new Vector3(x, 2.4f, z), new Vector3(10, 5, 8), colors[index]);
            var roof = Box("Gabled roof", new Vector3(x, 5.2f, z), new Vector3(1, 1, 1), new Color(.4f, .3f, .3f), false);
            var mesh = new Mesh { name = "Original simple gabled roof" };
            mesh.vertices = new[] { new Vector3(-5.6f, 0, -4.6f), new Vector3(5.6f, 0, -4.6f), new Vector3(0, 2.4f, -4.6f), new Vector3(-5.6f, 0, 4.6f), new Vector3(5.6f, 0, 4.6f), new Vector3(0, 2.4f, 4.6f) };
            mesh.triangles = new[] { 0, 2, 1, 3, 4, 5, 0, 3, 5, 0, 5, 2, 1, 2, 5, 1, 5, 4 };
            mesh.RecalculateNormals(); roof.GetComponent<MeshFilter>().sharedMesh = mesh; roof.AddComponent<OwnedRoofMesh>();
            Box("Front door", new Vector3(x - 5.02f, 1, z), new Vector3(.04f, 2.2f, 1.3f), wood, false);
            for (int side = -1; side <= 1; side += 2)
                Box("Front window", new Vector3(x - 5.04f, 2.4f, z + side * 2.5f), new Vector3(.04f, 1.5f, 1.6f), new Color(.67f, .83f, .87f), false);
            Box("Neighbour front walk", new Vector3(41, -.1f, z), new Vector3(6, .12f, 1.8f), path);
        }
        private void Tree(float x, float z)
        {
            Box("Tree trunk", new Vector3(x, 1.3f, z), new Vector3(.45f, 2.8f, .45f), wood);
            // Camera obstruction probes include the canopy, keeping Marty visible beneath trees.
            var canopy = Sphere("Leaf canopy", new Vector3(x, 3.5f, z), new Vector3(3.3f, 3, 3.3f), new Color(.24f, .43f, .28f));
            canopy.GetComponent<Collider>().enabled = true;
        }
        private void Bench(float x, float z)
        {
            Box("Park bench", new Vector3(x, .45f, z), new Vector3(2.6f, .2f, .85f), wood);
            Box("Bench back", new Vector3(x, .95f, z + .4f), new Vector3(2.6f, .85f, .12f), wood);
            for (int side = -1; side <= 1; side += 2) Box("Bench leg", new Vector3(x + side, .1f, z), new Vector3(.18f, .6f, .6f), wood);
        }
        private void Sign(string text, float x, float z)
        {
            Box("Walking sign post", new Vector3(x, .9f, z), new Vector3(.15f, 2, .15f), wood);
            var label = new GameObject(text); label.transform.SetParent(transform, false); label.transform.position = new Vector3(x, 2.2f, z);
            var lettering = label.AddComponent<TextMesh>(); lettering.text = text; lettering.fontSize = 36; lettering.characterSize = .07f; lettering.anchor = TextAnchor.MiddleCenter; lettering.color = BrandTheme.Cream;
            lettering.font = Resources.GetBuiltinResource<Font>("LegacyRuntime.ttf");
            label.GetComponent<MeshRenderer>().sharedMaterial = lettering.font.material;
            label.AddComponent<WalkingSign>();
        }
        private void Marker(string label, Vector3 position, NeighbourhoodStop stop, string dialogue)
        {
            var go = Sphere(label, position, Vector3.one * .55f, BrandTheme.Amber);
            var target = go.AddComponent<InteractableTarget>(); target.stage = -1; target.neighbourhoodStop = stop; target.dialogue = dialogue;
            go.AddComponent<NeighbourhoodMarker>().stop = stop;
        }
        private GameObject Sphere(string label, Vector3 position, Vector3 scale, Color color)
        {
            var go = GameObject.CreatePrimitive(PrimitiveType.Sphere); Configure(go, label, position, scale, color); go.GetComponent<Collider>().enabled = false; return go;
        }
        private GameObject Box(string label, Vector3 position, Vector3 scale, Color color, bool solid = true)
        {
            var go = GameObject.CreatePrimitive(PrimitiveType.Cube); Configure(go, label, position, scale, color); go.GetComponent<Collider>().enabled = solid; return go;
        }
        private void Configure(GameObject go, string label, Vector3 position, Vector3 scale, Color color)
        { go.name = label; go.transform.SetParent(transform, false); go.transform.localPosition = position; go.transform.localScale = scale; go.GetComponent<Renderer>().sharedMaterial = WorldMaterials.For(color); }
    }
    public sealed class WalkingSign : MonoBehaviour
    {
        private Camera view;
        private void LateUpdate() { if (!view) view = Camera.main; if (view) transform.rotation = view.transform.rotation; }
    }
    public sealed class OwnedRoofMesh : MonoBehaviour
    { private void OnDestroy() { var filter = GetComponent<MeshFilter>(); if (filter && filter.sharedMesh) Destroy(filter.sharedMesh); } }
}
