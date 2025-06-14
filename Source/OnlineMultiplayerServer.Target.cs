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
        //bUsesSteam = true;

        //
        // Steam Dedicated Server settings
        // IMPORTANT: Replace these with your own values from the Steamworks partner backend.
        //
        GlobalDefinitions.Add("UE_PROJECT_STEAMPRODUCTNAME=\"Spacewar\"");
        GlobalDefinitions.Add("UE_PROJECT_STEAMGAMEDIR=\"Spacewar\"");
        GlobalDefinitions.Add("UE_PROJECT_STEAMGAMEDESC=\"My Game Description\"");
        GlobalDefinitions.Add("UE_PROJECT_STEAMSHIPPINGID=480");
    }
}