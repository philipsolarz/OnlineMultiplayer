// OnlineMultiplayerServer.Target.cs

using UnrealBuildTool;
using System.Collections.Generic;

public class OnlineMultiplayerServerTarget : TargetRules
{
    public OnlineMultiplayerServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
        ExtraModuleNames.Add("OnlineMultiplayer");
        
        // Disable things we don't need on a server
        bUsesSteam = true;
    }
}