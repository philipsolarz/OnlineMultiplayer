// MenuSystem/ServerRowWidget.cpp

#include "MenuSystem/ServerRowWidget.h"
#include "MenuSystem/MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "OnlineSubsystem.h"
#include "StrafeMultiplayer/Public/MultiplayerSessionTypes.h"

void UServerRowWidget::Setup(UMainMenuWidget* InParent, const FOnlineSessionSearchResult& InSearchResult)
{
    Parent = InParent;
    SearchResult = InSearchResult;

    FString MapName = "N/A";
    SearchResult.Session.SessionSettings.Get(SESSION_KEY_MAP_NAME, MapName);
    FString GameMode = "N/A";
    SearchResult.Session.SessionSettings.Get(SESSION_KEY_GAME_MODE, GameMode);

    // API FIX: Correctly check if the server is dedicated using our custom setting
    bool bIsDedicatedFlag = false;
    SearchResult.Session.SessionSettings.Get(SESSION_KEY_IS_DEDICATED, bIsDedicatedFlag);

    if (bIsDedicatedFlag)
    {
        ServerNameText->SetText(FText::FromString(FString::Printf(TEXT("DEDI - %s (%s)"), *GameMode, *MapName)));
    }
    else
    {
        FString HostName = SearchResult.Session.OwningUserName;
        ServerNameText->SetText(FText::FromString(FString::Printf(TEXT("%s's Game - %s"), *HostName, *GameMode)));
    }

    int32 CurrentPlayers = SearchResult.Session.SessionSettings.NumPublicConnections - SearchResult.Session.NumOpenPublicConnections;
    int32 MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
    PlayerCountText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), CurrentPlayers, MaxPlayers)));

    PingText->SetText(FText::FromString(FString::FromInt(SearchResult.PingInMs) + "ms"));
}

void UServerRowWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (JoinButton)
    {
        JoinButton->OnClicked.AddDynamic(this, &UServerRowWidget::OnJoinButtonClicked);
    }
}

void UServerRowWidget::OnJoinButtonClicked()
{
    if (Parent)
    {
        JoinButton->SetIsEnabled(false);
        Parent->JoinServer(SearchResult);
    }
}