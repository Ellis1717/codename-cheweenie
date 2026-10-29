using UnityEngine;

namespace CodenameCheweenie
{
    public sealed class CatModelRig : MonoBehaviour
    {
        public CatLook look;
        public Transform[] legs, eyes;
        public Transform tail;
        private float phase;
        private Quaternion tailRest;
        public GroundedGait Gait {get;private set;}
        private void Awake() { if (tail) tailRest = tail.localRotation; }
        public void Animate(float speed)
        {
            if(!Gait){var feet=new Transform[legs.Length];for(int i=0;i<feet.Length;i++)feet[i]=legs[i].Find("Paw "+i);Gait=gameObject.AddComponent<GroundedGait>();Gait.Configure(legs,feet,look.ToString());}
            Gait.Animate(false,false);phase=Gait.Cycle;
            float blink=Mathf.Repeat(Time.time+(look==CatLook.Rio?2:0),5.8f)>5.63f?.12f:1;
            foreach(var eye in eyes)eye.localScale=new Vector3(1,blink,1);
            if(tail)tail.localRotation=tailRest*Quaternion.Euler(0,Mathf.Sin(Time.time*(look==CatLook.Lakitu?2.2f:1.1f))*(look==CatLook.Lakitu?15:7),0);
        }
    }
}
