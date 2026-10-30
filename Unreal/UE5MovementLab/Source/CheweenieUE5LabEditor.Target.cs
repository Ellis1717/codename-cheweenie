using UnrealBuildTool;
public class CheweenieUE5LabEditorTarget : TargetRules
{
    public CheweenieUE5LabEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        ExtraModuleNames.Add("CheweenieUE5Lab");
    }
}
