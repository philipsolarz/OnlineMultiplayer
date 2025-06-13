// MenuSystem/ServerRowWidget.cpp

#include "MenuSystem/ServerRowWidget.h"
#include "MenuSystem/MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "OnlineSubsystem.h"


void UServerRowWidget::Setup(UMainMenuWidget* InParent, const FOnlineSessionSearchResult& InSearchResult)
{
    Parent = InParent;
    SearchResult = InSearchResult;

    // Set server name from the "MatchType" setting, as defined in your MultiplayerSubsystem
    FString MatchType;
    SearchResult.Session.SessionSettings.Get(FName("MatchType"), MatchType);
    ServerNameText->SetText(FText::FromString(MatchType));

    // Set player count
    int32 CurrentPlayers = SearchResult.Session.SessionSettings.NumPublicConnections - SearchResult.Session.NumOpenPublicConnections;
    int32 MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
    PlayerCountText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), CurrentPlayers, MaxPlayers)));

    // Set ping
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