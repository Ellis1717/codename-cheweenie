using UnityEngine;
namespace CodenameCheweenie
{
    public sealed class CanineVisual : MonoBehaviour
    {
        public GameObject visualPrefab;
        private Transform model;
        private Animator animator;
        private Transform[] legs;
        private float phase;
        private MartyModelRig rig;
        private Vector3 restScale;
        public bool UsesAuthoredModel => rig != null;
        private void Start()
        {
            if (!visualPrefab) visualPrefab = Resources.Load<GameObject>("MartyVisual");
            model = visualPrefab ? Instantiate(visualPrefab, transform).transform : new GameObject("Procedural canine rig").transform;
            model.SetParent(transform, false);
            restScale = model.localScale;
            rig = model.GetComponent<MartyModelRig>();
            if (!visualPrefab)
            {
                Color tan = new Color(.59f, .34f, .18f);
                Part("Long body", new Vector3(0, -.22f, 0), new Vector3(.6f, .45f, 1.2f), tan);
                Part("Head", new Vector3(0, .05f, .65f), new Vector3(.52f, .5f, .5f), tan);
                Part("Muzzle", new Vector3(0, -.04f, .92f), new Vector3(.28f, .22f, .36f), tan);
                Part("Nose", new Vector3(0, -.02f, 1.1f), Vector3.one * .12f, Color.black);
                for (int side = -1; side <= 1; side += 2)
                {
                    Part("Eye", new Vector3(side * .2f, .15f, .84f), Vector3.one * .08f, Color.black);
                    Part("Floppy ear", new Vector3(side * .3f, -.04f, .6f), new Vector3(.16f, .5f, .22f), new Color(.3f, .16f, .08f));
                }
                legs = new Transform[4];
                for (int i = 0; i < 4; i++) legs[i] = Part("Paw " + i,
                    new Vector3(i % 2 == 0 ? -.23f : .23f, -.57f, i < 2 ? .4f : -.4f), new Vector3(.15f, .35f, .2f), tan);
                Part("Tail", new Vector3(0, -.05f, -.7f), new Vector3(.12f, .4f, .12f), tan).localRotation = Quaternion.Euler(-40, 0, 0);
            }
            foreach (var collider in model.GetComponentsInChildren<Collider>()) collider.enabled = false;
            animator = model.GetComponentInChildren<Animator>();
            if (animator) animator.applyRootMotion = false;
        }
        private Transform Part(string label, Vector3 position, Vector3 scale, Color color)
        {
            var go = GameObject.CreatePrimitive(PrimitiveType.Sphere); go.name = label;
            go.transform.SetParent(model, false); go.transform.localPosition = position; go.transform.localScale = scale;
            go.GetComponent<Collider>().enabled = false;
            go.GetComponent<Renderer>().sharedMaterial = WorldMaterials.For(color);
            return go.transform;
        }
        public void Animate(float speed, bool cozy)
        {
            if (!model) return;
            phase += Time.deltaTime * speed * 3;
            bool between = GetComponent<MartyController>().IsBetween;
            model.localScale = Vector3.Scale(restScale, cozy ? new Vector3(1, rig ? .78f : .55f, 1) : between ? new Vector3(.7f, 1, 1) : Vector3.one);
            if (rig) rig.Animate(speed, cozy, between);
            if (legs != null) for (int i = 0; i < legs.Length; i++)
                legs[i].localRotation = Quaternion.Euler(Mathf.Sin(phase + (i == 0 || i == 3 ? 0 : Mathf.PI)) * Mathf.Min(speed * 6, 30), 0, 0);
            if (!animator || !animator.runtimeAnimatorController) return;
            foreach (var parameter in animator.parameters)
            {
                if (parameter.name == "Speed" && parameter.type == AnimatorControllerParameterType.Float) animator.SetFloat("Speed", speed);
                if (parameter.name == "Cozy" && parameter.type == AnimatorControllerParameterType.Bool) animator.SetBool("Cozy", cozy);
            }
        }
    }
}
