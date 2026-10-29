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
        private void Awake() { if (tail) tailRest = tail.localRotation; }
        public void Animate(float speed)
        {
            phase += Mathf.Min(speed,4)*Time.deltaTime*6;
            for(int i=0;i<legs.Length;i++) legs[i].localRotation=Quaternion.Euler(Mathf.Sin(phase+(i==0||i==3?0:Mathf.PI))*Mathf.Min(speed*9,24),0,0);
            float blink=Mathf.Repeat(Time.time+(look==CatLook.Rio?2:0),5.8f)>5.63f?.12f:1;
            foreach(var eye in eyes)eye.localScale=new Vector3(1,blink,1);
            if(tail)tail.localRotation=tailRest*Quaternion.Euler(0,Mathf.Sin(Time.time*(look==CatLook.Lakitu?2.2f:1.1f))*(look==CatLook.Lakitu?15:7),0);
        }
    }
}
