using UnityEngine;

namespace CodenameCheweenie
{
    // Original schematic play space: these dimensions are not a real house/yard survey.
    public sealed class BackyardBuilder : MonoBehaviour
    {
        public void Build()
        {
            Color wood = new Color(.45f, .31f, .2f), hedge = new Color(.16f, .34f, .22f);
            Box("West garden fence", new Vector3(-17, .55f, -13), new Vector3(.25f, 1.3f, 12), wood);
            Box("East garden fence north", new Vector3(17, .55f, -8.5f), new Vector3(.25f, 1.3f, 3), wood);
            Box("East garden fence south", new Vector3(17, .55f, -17.5f), new Vector3(.25f, 1.3f, 3), wood);
            if (MappedNeighbourhood.Current != null)
            {
                Box("South garden fence west", new Vector3(-10, .55f, -19), new Vector3(14, 1.3f, .25f), wood);
                Box("South garden fence east", new Vector3(10, .55f, -19), new Vector3(14, 1.3f, .25f), wood);
            }
            else Box("South garden fence", new Vector3(0, .55f, -19), new Vector3(34, 1.3f, .25f), wood);
            Box("West entry fence", new Vector3(-12.75f, .55f, -7), new Vector3(8.5f, 1.3f, .25f), wood);
            Box("East entry fence", new Vector3(12.75f, .55f, -7), new Vector3(8.5f, 1.3f, .25f), wood);

            // Short optional hops lead toward an accessible ramp; the main quest marker stays clear.
            for (int i = 0; i < 4; i++)
            {
                float height = .15f + (i % 2) * .16f;
                Box("Scent stepping stone " + (i + 1), new Vector3(-2 - i * 1.6f, height * .5f - .1f, -10.5f - i * .6f),
                    new Vector3(1.3f, height, 1.1f), new Color(.58f, .57f, .52f));
            }
            Marker("Stepping-stone scent trail", new Vector3(-4, .65f, -11.5f), BackyardClue.ScentTrail,
                "Keeterman's scent trail! The stepping stones point toward the raised lookout and the hedge passage.");

            // Cut a ramp channel through the front edge instead of burying the incline in a box collider.
            Box("Raised backyard lookout", new Vector3(-10, 1.05f, -15.25f), new Vector3(5, .4f, 4.5f), wood);
            Box("Lookout west landing strip", new Vector3(-11.875f, 1.05f, -12.75f), new Vector3(1.25f, .4f, .5f), wood);
            Box("Lookout east landing strip", new Vector3(-8.125f, 1.05f, -12.75f), new Vector3(1.25f, .4f, .5f), wood);
            Vector3 bottom = new Vector3(-10, -.1f, -9), top = new Vector3(-10, 1.25f, -13);
            Vector3 slope = top - bottom;
            var ramp = Box("Lookout ramp", (bottom + top) * .5f - Vector3.up * .1f,
                new Vector3(2.5f, .2f, slope.magnitude), wood);
            ramp.transform.rotation = Quaternion.LookRotation(slope.normalized);
            for (int side = -1; side <= 1; side += 2)
                Box("Lookout side rail", new Vector3(-10 + side * 2.4f, 1.8f, -15), new Vector3(.18f, 1.1f, 5), wood);
            Box("Lookout back rail", new Vector3(-10, 1.8f, -17.4f), new Vector3(5, 1.1f, .18f), wood);
            Marker("Raised lookout scent", new Vector3(-10, 1.85f, -15), BackyardClue.Lookout,
                "From the lookout, Marty spots a ribbon beyond the hedge passage. Two paws up for the view!");

            for (int side = -1; side <= 1; side += 2)
                Box("Paw passage hedge", new Vector3(11 + side * 1.4f, .8f, -13), new Vector3(.6f, 1.8f, 8), hedge);
            Box("Paw passage canopy", new Vector3(11, 1.7f, -13), new Vector3(3.4f, .35f, 8), hedge);
            Marker("Secret paw passage", new Vector3(11, .55f, -13), BackyardClue.PawPassage,
                "A secret paw passage! Marty fits under the hedge canopy. The ribbon scent leads out the far end.");

            Box("Ribbon cache cushion", new Vector3(14.5f, .02f, -16.5f), new Vector3(1.7f, .24f, 1.4f), new Color(.28f, .38f, .57f));
            Marker("Rio's blanket ribbon", new Vector3(14.5f, .65f, -16.5f), BackyardClue.Ribbon,
                "Marty found Rio's blanket ribbon! Bring it back to Miss Priss in headquarters.");
            Box("Ribbon tail", new Vector3(14.5f, .2f, -16.5f), new Vector3(1.2f, .08f, .18f), new Color(.85f, .49f, .21f));
            for (int side = -1; side <= 1; side += 2)
                Box("Garden planter", new Vector3(side * 15, .15f, -9), new Vector3(2, .5f, 1), wood);
        }
        private GameObject Box(string label, Vector3 position, Vector3 scale, Color color)
        {
            var go = GameObject.CreatePrimitive(PrimitiveType.Cube); go.name = label; go.transform.SetParent(transform, false);
            go.transform.localPosition = position; go.transform.localScale = scale;
            go.GetComponent<Renderer>().sharedMaterial = WorldMaterials.For(color); return go;
        }
        private void Marker(string label, Vector3 position, BackyardClue clue, string dialogue)
        {
            var go = GameObject.CreatePrimitive(PrimitiveType.Sphere); go.name = label; go.transform.SetParent(transform, false);
            go.transform.localPosition = position; go.transform.localScale = Vector3.one * .45f; go.GetComponent<Collider>().enabled = false;
            var target = go.AddComponent<InteractableTarget>(); target.stage = -1; target.backyardClue = clue; target.dialogue = dialogue;
            go.AddComponent<BackyardMarker>().clue = clue;
        }
    }
}
