using UnrealBuildTool;
public class CheweenieUE5Lab : ModuleRules
{
    public CheweenieUE5Lab(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] {"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Json", "AudioMixer"});
    }
}
