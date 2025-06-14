// MenuSystem/MainMenuWidget.cpp

#include "MenuSystem/MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "StrafeMultiplayer/Public/StrafeMultiplayerSubsystem.h"
#include "MenuSystem/ServerRowWidget.h"
#include "Kismet/GameplayStatics.h"

void UMainMenuWidget::Setup()
{
    this->AddToViewport();
    this->SetVisibility(ESlateVisibility::Visible);

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        FInputModeUIOnly InputModeData;
        InputModeData.SetWidgetToFocus(this->TakeWidget());
        InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PlayerController->SetInputMode(InputModeData);
        PlayerController->SetShowMouseCursor(true);
    }
}

void UMainMenuWidget::Teardown()
{
    this->RemoveFromParent();
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        FInputModeGameOnly InputModeData;
        PlayerController->SetInputMode(InputModeData);
        PlayerController->SetShowMouseCursor(false);
    }
}

void UMainMenuWidget::JoinServer(const FOnlineSessionSearchResult& SessionResult)
{
    if (MultiplayerSubsystem)
    {
        MultiplayerSubsystem->JoinSession(SessionResult);
    }
}

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (CreateButton) CreateButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnCreateButtonClicked);
    if (RefreshButton) RefreshButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnRefreshButtonClicked);

    UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance)
    {
        MultiplayerSubsystem = GameInstance->GetSubsystem<UStrafeMultiplayerSubsystem>();
        if (MultiplayerSubsystem)
        {
            MultiplayerSubsystem->OnCreateSessionComplete.AddDynamic(this, &UMainMenuWidget::OnCreateSessionComplete);
            MultiplayerSubsystem->OnFindLobbiesComplete.AddUObject(this, &UMainMenuWidget::OnFindLobbiesComplete);
            MultiplayerSubsystem->OnFindDedicatedServersComplete.AddUObject(this, &UMainMenuWidget::OnFindDedicatedServersComplete);
            MultiplayerSubsystem->OnJoinSessionComplete.AddUObject(this, &UMainMenuWidget::OnJoinSessionComplete);
        }
    }
}

void UMainMenuWidget::NativeDestruct()
{
    // UObject delegates are auto-unbound on destruction
    Super::NativeDestruct();
}

void UMainMenuWidget::OnCreateButtonClicked()
{
    if (MultiplayerSubsystem)
    {
        CreateButton->SetIsEnabled(false);
        RefreshButton->SetIsEnabled(false);

        FStrafeGameSessionSettings SessionSettings;
        SessionSettings.MaxPlayers = 8;
        SessionSettings.GameMode = TEXT("Arena");
        SessionSettings.MapName = TEXT("DM-Deck");
        SessionSettings.GameModeSettings = TEXT("TimeLimit=10,FragLimit=25");
        SessionSettings.bIsDedicated = false;

        MultiplayerSubsystem->CreateSession(SessionSettings);
    }
}

void UMainMenuWidget::OnRefreshButtonClicked()
{
    if (MultiplayerSubsystem)
    {
        RefreshButton->SetIsEnabled(false);
        ServerListScrollBox->ClearChildren();

        FindSessionsRequestCount = 2;
        MultiplayerSubsystem->FindLobbies();
        MultiplayerSubsystem->FindDedicatedServers();
    }
}

void UMainMenuWidget::OnCreateSessionComplete(EMultiplayerSessionResult Result)
{
    if (Result == EMultiplayerSessionResult::Success)
    {
        UWorld* World = GetWorld();
        if (World)
        {
            // API FIX: Removed the unused LastSettings variable.
            // In a real game, you would get the map from the settings used to create the session.
            World->ServerTravel(TEXT("/Game/FirstPerson/LvL_FirstPerson?listen"));
        }
    }
    else
    {
        CreateButton->SetIsEnabled(true);
        RefreshButton->SetIsEnabled(true);
    }
}

void UMainMenuWidget::OnFindLobbiesComplete(const TArray<FOnlineSessionSearchResult>& SessionResults, EMultiplayerSessionResult Result)
{
    FindSessionsRequestCount--;
    if (FindSessionsRequestCount <= 0)
    {
        RefreshButton->SetIsEnabled(true);
    }

    if (Result == EMultiplayerSessionResult::Success)
    {
        for (const FOnlineSessionSearchResult& SearchResult : SessionResults)
        {
            UServerRowWidget* ServerRow = CreateWidget<UServerRowWidget>(this, ServerRowClass);
            if (ServerRow)
            {
                ServerRow->Setup(this, SearchResult);
                ServerListScrollBox->AddChild(ServerRow);
            }
        }
    }
}

void UMainMenuWidget::OnFindDedicatedServersComplete(const TArray<FOnlineSessionSearchResult>& SessionResults, EMultiplayerSessionResult Result)
{
    FindSessionsRequestCount--;
    if (FindSessionsRequestCount <= 0)
    {
        RefreshButton->SetIsEnabled(true);
    }

    if (Result == EMultiplayerSessionResult::Success)
    {
        for (const FOnlineSessionSearchResult& SearchResult : SessionResults)
        {
            UServerRowWidget* ServerRow = CreateWidget<UServerRowWidget>(this, ServerRowClass);
            if (ServerRow)
            {
                ServerRow->Setup(this, SearchResult);
                ServerListScrollBox->AddChild(ServerRow);
            }
        }
    }
}

void UMainMenuWidget::OnJoinSessionComplete(EMultiplayerSessionResult Result, const FString& ConnectString)
{
    if (Result == EMultiplayerSessionResult::Success)
    {
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
        if (PlayerController)
        {
            PlayerController->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);
        }
    }
    else
    {
        RefreshButton->SetIsEnabled(true);
    }
}