using UnityEngine;
namespace CodenameCheweenie
{
    // Presentation-only pages; bindings and progress are read live rather than duplicated in artwork.
    public sealed class GuideContent
    {
        public static readonly string[] CrewNames={"Marty","Lakitu","Rio"};
        private static readonly string[][] Tabs={new[]{"Actions","Camera views","Menus & recovery"},new[]{"Home missions","Backyard safari","Neighbourhood"},CrewNames,new[]{"The demo"}};
        private readonly Texture2D[] art={Resources.Load<Texture2D>("Branding/MartyTrail"),Resources.Load<Texture2D>("Branding/LakituPatrol"),Resources.Load<Texture2D>("Branding/RioCozy")};
        public static string[] TabNames(int category)=>Tabs[Mathf.Clamp(category-1,0,3)];
        public void DrawArt(Rect rect,int index)
        {
            BrandTheme.Fill(rect,new Color(.11f,.16f,.19f));
            if(art[index])GUI.DrawTexture(rect,art[index],ScaleMode.ScaleToFit);
            else GUI.Label(rect,CrewNames[index],BrandTheme.Text(24,BrandTheme.Cream,true));
        }
        private static void Heading(string text,string subtitle)
        {
            GUI.Label(new Rect(324,184,870,40),text,BrandTheme.Text(28,BrandTheme.Cream,true));
            GUI.Label(new Rect(324,230,870,52),subtitle,BrandTheme.Text(17,BrandTheme.Cream));
        }
        private static void PanelText(Rect rect,string text,int size)
        {
            var style=BrandTheme.Text(size,BrandTheme.Cream);
            while(size>17&&style.CalcHeight(new GUIContent(text),rect.width)>rect.height)style=BrandTheme.Text(--size,BrandTheme.Cream);
            GUI.Label(rect,text,style);
        }
        private static void Text(string text,int y=298)=>PanelText(new Rect(324,y,860,360),text,20);
        private static void Row(int row,string action,string keyboard,string pad)
        {
            float y=292+row*40;
            if(row%2==0)BrandTheme.Fill(new Rect(324,y,856,38),new Color(.11f,.15f,.19f));
            GUI.Label(new Rect(336,y+6,270,30),action,BrandTheme.Text(17,BrandTheme.Cream,true));
            GUI.Label(new Rect(610,y+6,250,30),keyboard,BrandTheme.Text(17,BrandTheme.Amber));
            GUI.Label(new Rect(864,y+6,300,30),pad,BrandTheme.Text(17,BrandTheme.Amber));
        }
        public void Draw(int category,int tab,MartyInput controls)
        {
            if(category==1)DrawControls(tab,controls);
            else if(category==2)DrawAdventure(tab);
            else if(category==3)DrawCrew(tab);
            else
            {
                Heading("A little dog. A whole crew.","Codename .... Cheweenie  ·  Version "+Application.version);
                Text("An original family adventure starring Marty, Lakitu and Rio.\n\nMarty is named after Martin St-Louis. His specialties are Cheeween Cozy and Cheween in Between.\n\nOriginal music, playful family cameos and a cozy world for little paws.\n\nProgress and settings save locally. New Adventure resets missions and friendship after confirmation; your settings stay.\n\nHave fun, sniff everything, and make room for the cuddles.");
            }
        }
        private static void DrawControls(int tab,MartyInput controls)
        {
            if(tab==0)
            {
                Heading("Your current controls",GameSave.Data.playStationLabels?"PlayStation labels selected · light stick = slow walk":"Xbox labels selected · light stick = slow walk");
                Row(0,"ACTION","KEYBOARD","CONTROLLER");Row(1,"Move","WASD / arrows","Left stick / D-pad");
                string[] actions={"Jump","Sniff / greet","Sprint (hold)","Cheeween Cozy","Cheween in Between","Pause / settings"};
                for(int i=0;i<6;i++)Row(i+2,actions[i],controls.keyboard[i].ToString().Replace("Left","L ").Replace("Right","R "),MartyInput.GamepadLabel(controls.gamepad[i]));
                GUI.Label(new Rect(324,624,860,44),"Settings → Rebind device → choose an action → press a key/button. Conflicts swap. Escape / Menu / Options cancels capture.",BrandTheme.Text(16,BrandTheme.Cream));
            }
            else if(tab==1)
            {
                Heading("Pick your point of view","V / R3 cycles views. Settings → Camera also selects a view; Left / Right changes distance.");
                Text("COZY · A steady overview. Movement follows the world's directions.\n\nEXPLORE · A closer orbit camera. Movement follows where the camera faces.\n\nLOW · Get down near pet-eye level. Movement follows the camera.\n\nLOOK · Right stick, or hold right mouse and drag. Looking from Cozy enters Explore.\nRECENTER · Home / RB / R1 turns the view behind Marty (north in Cozy).\n\nYour view and distance save. Custom action bindings take priority over camera shortcut buttons.");
            }
            else
            {
                Heading("Menus, maps and getting home","Menus always use A / Cross to confirm and B / Circle to return, regardless of action bindings.");
                Text("NAVIGATE · Arrows, D-pad or left stick. Enter / A / Cross confirms.\nPAUSE · Escape / Menu / Options.\nHELP · F1 during play; F1 / Y / Triangle from paused Settings.\nFIELD GUIDE · Up / Down changes section; Left / Right changes page.\n\nMAP · M / View / Share opens the map. Close it with the same button.\nSTUCK? · Pause → Return Marty home. Missions and friendships stay saved.\n\nAUDIO · Settings has separate music/effects volume.\nBINDINGS · Choose Xbox/PlayStation labels, rebind or reset buttons.\nNEW ADVENTURE · Requires confirmation and keeps your settings.");
            }
        }
        private static void DrawAdventure(int tab)
        {
            var data=GameSave.Data;
            if(tab==0)
            {
                Heading("Make yourself at home","Home mission progress: "+data.stage+" / 4 · Walk up to a marker and use the action shown on the HUD.");
                Text("1 · Greet Lakitu / Keeterman in headquarters.\n2 · Find Rio / Miss Priss and say hello.\n3 · Sniff the backyard zone. Fox Ellis has a very important grass report.\n4 · Use Cheeween Cozy at the blue blanket fort. Leo Ellis requests five more minutes.\n\nCHEEWEEN COZY · Snuggle at the fort with your Cozy action.\nCHEWEEN IN BETWEEN · Near the cuddle spot, use Between to squeeze in.\n\nExplore the upstairs ramp, return outside, and follow Keeterman's scent safari when the home mission is complete.");
            }
            else if(tab==1)
            {
                Heading("Keeterman's scent safari","Available after the home mission · Backyard progress is saved independently.");
                Text("SNIFF THREE CLUES · The stepping stones, raised lookout and secret hedge passage. You can discover these in any order.\n\nFIND THE RIBBON · Once all three clues are found, follow the hedge passage to Rio's ribbon cache.\n\nBRING IT HOME · Greet Rio back in headquarters to return her ribbon.\n\nUse the ramps for an easy route. Solid fences, cushions and furniture block movement; walk around them.\n\nNeed a break? Progress saves automatically; Continue brings the mission back.");
            }
            else
            {
                Heading("A neighbourhood for little paws","The map works from the start · Use M / View / Share to see the walking stops.");
                Text("LEAVE THE GARDEN · Follow the open gate and paths into the walking area. The local map uses the south gate; the fictional block uses the east gate.\n\nFOLLOW THE MARKERS · Visit the marked sniff stops, the park and the school stop when your selected map includes it. Then return to Home.\n\nEXPLORE TOGETHER · Lakitu can become a patrol companion after his home quest. Rio prefers a well-managed blanket.\n\nRECOVER · Pause → Return Marty home if you get turned around.\n\nBuildings, terrain and connecting paths are simplified for play.");
            }
        }
        private void DrawCrew(int tab)
        {
            string[] titles={"Marty · the little explorer","Lakitu · chief of the scent patrol","Rio · blanket quality control"};
            string[] aliases={"Mr. Marty / Marty Man / Marty Party / Marty McFly","Keetz / Keeterman","Sis / Little Miss / Miss Priss"};
            Heading(titles[tab],aliases[tab]);DrawArt(new Rect(324,296,350,350),tab);
            string[] bios={"A little Chiweenie named after Martin St-Louis, with a nose for adventure.\n\nSignature moves\nCheeween Cozy: blanket snuggles.\nCheween in Between: squeeze into the cuddles.\n\nSpecial equipment\nBig eyes. Short legs. Unlimited enthusiasm.","Curious, energetic and always ready to investigate a leaf.\n\nMake friends\nSay hello, then sniff the backyard to finish his companion quest.\n\nPatrol partner\nOnce his quest is complete, Keeterman can follow Marty on adventures.","Miss Priss appreciates the finer things: a tidy ribbon and a very good blanket.\n\nMake friends\nSay hello and get Cheeween Cozy to earn her approval.\n\nA favour for Sis\nFind her missing ribbon outside and bring it back neatly."};
            PanelText(new Rect(708,298,465,350),bios[tab],20);
            string[] captions={"Marty on patrol: every leaf deserves a sniff.","Keeterman's latest clue: probably a leaf. Definitely worth investigating.","Miss Priss approves. Please leave the blanket exactly like this."};
            GUI.Label(new Rect(324,656,860,22),captions[tab],BrandTheme.Text(14,BrandTheme.Cream));
        }
    }
}
