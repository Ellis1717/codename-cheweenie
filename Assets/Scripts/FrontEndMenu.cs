using UnityEngine;
namespace CodenameCheweenie
{
    public sealed class FrontEndMenu : MonoBehaviour
    {
        public bool ShowingTitle {get;private set;}
        public bool GuideOpen=>page>0;
        public int GuideCategory=>page;
        public int GuideTab=>guideTab;
        public bool Transitioning=>transitionFrame==Time.frameCount;
        private MartyInput controls;
        private CozySettings settings;
        private BrandTheme theme;
        private GuideContent guide;
        private int selected,page,guideTab,transitionFrame;
        private bool confirmNewGame,stickReleased=true,horizontalReleased=true,guideFromGame,guideFromSettings;
        private float playTimeScale=1,guideTimeScale=1;
        private bool HasProgress=>GameSave.Data.stage>0||GameSave.Data.backyardState>0||GameSave.Data.neighbourhoodState>0||GameSave.Data.lakituFriendship>0||GameSave.Data.rioFriendship>0;
        public void Initialize(MartyInput input,CozySettings options){controls=input;settings=options;GameSave.Load();ShowTitle();}
        public void ShowTitle()
        {
            if(!ShowingTitle)playTimeScale=Time.timeScale>0?Time.timeScale:1;
            ShowingTitle=true;controls.Blocked=true;Time.timeScale=0;page=selected=guideTab=0;confirmNewGame=false;transitionFrame=Time.frameCount;
        }
        public void StartAdventure(bool newGame)
        {
            if(newGame)GameSave.NewGame();
            page=0;ShowingTitle=false;controls.Blocked=false;controls.SuppressForFrame();Time.timeScale=playTimeScale;transitionFrame=Time.frameCount;
        }
        public void SettingsClosed(){transitionFrame=Time.frameCount;}
        public void OpenGuide(int category,bool fromSettings=false)
        {
            guideFromGame=!ShowingTitle;guideFromSettings=fromSettings;if(fromSettings)settings.Close();
            guideTimeScale=Time.timeScale>0?Time.timeScale:1;page=Mathf.Clamp(category,1,4);guideTab=0;controls.Blocked=true;Time.timeScale=0;transitionFrame=Time.frameCount;
        }
        private void CloseGuide()
        {
            page=0;transitionFrame=Time.frameCount;
            if(guideFromGame){Time.timeScale=guideTimeScale;controls.Blocked=false;controls.SuppressForFrame();}
            if(guideFromSettings)settings.OpenFromTitle();
        }
        private void ChangeCategory(int category){page=(category+3)%4+1;guideTab=0;transitionFrame=Time.frameCount;}
        private void ChangeTab(int direction){int count=GuideContent.TabNames(page).Length;guideTab=(guideTab+direction+count)%count;transitionFrame=Time.frameCount;}
        private void Update()
        {
            if(Transitioning||settings.IsOpen)return;
            if(!ShowingTitle&&!GuideOpen){if(controls.KeyboardUtilityAvailable(KeyCode.F1)&&GameControls.GetKeyDown(KeyCode.F1))OpenGuide(1);return;}
            if(GameControls.CancelPressed||GuideOpen&&(GameControls.PausePressed||GameControls.GetKeyDown(KeyCode.F1)))
            {
                if(GuideOpen)CloseGuide();else if(confirmNewGame)confirmNewGame=false;else if(GameControls.GetKeyDown(KeyCode.Escape))settings.OpenFromTitle();return;
            }
            if(!GuideOpen&&GameControls.PausePressed){settings.OpenFromTitle();return;}
            if(!GuideOpen&&GameControls.GetKeyDown(KeyCode.F1)){OpenGuide(1);return;}
            float axis=GameControls.GetAxisRaw("DogStickY");if(Mathf.Abs(axis)<.3f)stickReleased=true;
            int direction=GameControls.GetKeyDown(KeyCode.DownArrow)?1:GameControls.GetKeyDown(KeyCode.UpArrow)?-1:0;
            if(stickReleased&&Mathf.Abs(axis)>.6f){direction=axis>0?1:-1;stickReleased=false;}
            if(direction!=0){if(GuideOpen)ChangeCategory(page+direction);else{selected=(selected+direction+8)%8;confirmNewGame=false;}return;}
            if(GuideOpen)
            {
                float horizontal=GameControls.GetAxisRaw("DogStickX");if(Mathf.Abs(horizontal)<.3f)horizontalReleased=true;
                int step=GameControls.GetKeyDown(KeyCode.RightArrow)?1:GameControls.GetKeyDown(KeyCode.LeftArrow)?-1:0;
                if(horizontalReleased&&Mathf.Abs(horizontal)>.6f){step=horizontal>0?1:-1;horizontalReleased=false;}
                if(step!=0)ChangeTab(step);else if(GameControls.SubmitPressed)ChangeTab(1);
            }
            else if(GameControls.SubmitPressed)Activate(selected);
        }
        private void Activate(int row)
        {
            if(Transitioning)return;transitionFrame=Time.frameCount;if(row!=1)confirmNewGame=false;GetComponent<GameAudio>()?.Play(SoundCue.Menu);
            if(row==0)StartAdventure(false);
            else if(row==1){if(HasProgress&&!confirmNewGame){confirmNewGame=true;return;}StartAdventure(true);}
            else if(row==2)settings.OpenFromTitle();else if(row<7)OpenGuide(row-2);else Application.Quit();
        }
        private void OnGUI()
        {
            if(!ShowingTitle&&!GuideOpen)return;if(theme==null)theme=new BrandTheme();if(guide==null)guide=new GuideContent();
            int depth=GUI.depth;GUI.depth=10;Matrix4x4 old=GUI.matrix;float scale=Mathf.Min(Screen.width/1280f,Screen.height/800f);
            float width=Screen.width/scale,height=Screen.height/scale;GUI.matrix=Matrix4x4.Scale(Vector3.one*scale);theme.Background(width,height);
            if(!settings.IsOpen){if(GuideOpen)DrawGuide(width,height);else DrawTitle(height);}GUI.matrix=old;GUI.depth=depth;
        }
        private void DrawTitle(float height)
        {
            theme.Paw(64,54,40);GUI.Label(new Rect(119,57,450,32),"MARTY'S COZY ADVENTURE",BrandTheme.Text(16,BrandTheme.Amber,true));
            GUI.Label(new Rect(64,124,490,45),"Codename ....",BrandTheme.Text(32,BrandTheme.Cream,true));
            GUI.Label(new Rect(60,164,610,95),"Cheweenie",BrandTheme.Text(76,BrandTheme.Cream,true));
            GUI.Label(new Rect(66,268,435,64),"A little dog. A home full of adventures.\nWith Marty, Lakitu & Rio.",BrandTheme.Text(20,BrandTheme.Cream));
            string[] labels={HasProgress?"Continue adventure":"Start adventure",confirmNewGame?"Confirm new adventure":"New adventure","Settings","Controls & camera","Adventure guide","Meet the crew","About the demo","Quit"};
            for(int i=0;i<labels.Length;i++){float y=i<3?357+i*52:513+(i-3)*42;if(BrandTheme.Button(new Rect(68,y,386,i<3?44:36),labels[i],selected==i,i==0))Activate(i);}
            if(confirmNewGame)GUI.Label(new Rect(68,721,480,30),"Resets missions/friendship. Settings stay saved.",BrandTheme.Text(15,BrandTheme.Cream));
            for(int i=0;i<3;i++){float x=676+i*178;guide.DrawArt(new Rect(x,534,164,164),i);if(BrandTheme.Button(new Rect(x,700,164,32),GuideContent.CrewNames[i],false)){OpenGuide(3);guideTab=i;}}
            GUI.Label(new Rect(68,height-44,1120,30),"D-pad / stick: choose · Enter / A / Cross: select · B / Circle: back · F1: controls",BrandTheme.Text(16,BrandTheme.Cream));
        }
        private void DrawGuide(float width,float height)
        {
            BrandTheme.Fill(new Rect(44,32,width-88,height-96),new Color(.035f,.055f,.08f,.97f));
            GUI.Label(new Rect(68,48,1000,32),BrandTheme.Title+"  /  FIELD GUIDE",BrandTheme.Text(22,BrandTheme.Amber,true));
            string[] categories={"Controls","Adventure","Meet the crew","About"};
            for(int i=0;i<4;i++)if(BrandTheme.Button(new Rect(68,140+i*56,220,46),categories[i],page==i+1))ChangeCategory(i+1);
            GUI.Label(new Rect(68,386,210,170),"Take your time.\nThe game is paused.\n\nUp / Down: section\nLeft / Right: page\nA / Cross: next page\nB / Circle: back",BrandTheme.Text(16,BrandTheme.Cream));
            var tabs=GuideContent.TabNames(page);
            for(int i=0;i<tabs.Length;i++)if(BrandTheme.Button(new Rect(324+i*286,118,274,42),tabs[i],guideTab==i)){guideTab=i;transitionFrame=Time.frameCount;}
            guide.Draw(page,guideTab,controls);
            if(BrandTheme.Button(new Rect(68,height-126,220,44),guideFromSettings?"Back to settings":guideFromGame?"Resume adventure":"Back to title",true))CloseGuide();
            GUI.Label(new Rect(324,height-100,890,40),"Saved bindings are shown live. Change buttons in Settings.  F1 / B / Circle: back.",BrandTheme.Text(15,BrandTheme.Cream));
        }
        private void OnDestroy(){theme?.Dispose();if(ShowingTitle||GuideOpen){Time.timeScale=playTimeScale;if(controls)controls.Blocked=false;}}
    }
}
