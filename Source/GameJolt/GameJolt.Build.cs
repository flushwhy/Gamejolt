using UnrealBuildTool;
using System.IO; 

public class GameJolt : ModuleRules
{
    public GameJolt(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[]
        {
            Path.Combine(ModuleDirectory, "Public")
        });

        PrivateIncludePaths.AddRange(new string[]
        {
            Path.Combine(ModuleDirectory, "Private")
        });

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core", "HTTP", "Json", "JsonUtilities", "UMG", "DeveloperSettings"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "CoreUObject", "Engine", "Slate", "SlateCore"
        });
    }
}