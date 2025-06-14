// MenuSystem/MainMenuWidget.cpp

#include "MenuSystem/MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "MenuSystem/MultiplayerSubsystem.h"
#include "MenuSystem/ServerRowWidget.h"

void UMainMenuWidget::Setup()
{
    UE_LOG(LogMultiplayerMenu, Log, TEXT("MainMenuWidget: Setup called. Adding to viewport and setting input mode."));
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
    UE_LOG(LogMultiplayerMenu, Log, TEXT("MainMenuWidget: Teardown called. Removing from parent and setting input mode."));
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
    UE_LOG(LogMultiplayerMenu, Log, TEXT("MainMenuWidget: JoinServer called."));
    if (MultiplayerSubsystem)
    {
        MultiplayerSubsystem->JoinSession(SessionResult);
    }
}

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();
    UE_LOG(LogMultiplayerMenu, Log, TEXT("MainMenuWidget: NativeConstruct called."));

    if (CreateButton) CreateButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnCreateButtonClicked);
    if (RefreshButton) RefreshButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnRefreshButtonClicked);

    UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance)
    {
        MultiplayerSubsystem = GameInstance->GetSubsystem<UMultiplayerSubsystem>();
        if (MultiplayerSubsystem)
        {
            UE_LOG(LogMultiplayerMenu, Log, TEXT("MainMenuWidget: Binding to MultiplayerSubsystem delegates."));
            MultiplayerSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &UMainMenuWidget::OnCreateSession);
            MultiplayerSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &UMainMenuWidget::OnFindSessions);
            MultiplayerSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &UMainMenuWidget::OnJoinSession);
        }
    }
}

void UMainMenuWidget::NativeDestruct()
{
    UE_LOG(LogMultiplayerMenu, Log, TEXT("MainMenuWidget: NativeDestruct called."));
    if (MultiplayerSubsystem)
    {
        UE_LOG(LogMultiplayerMenu, Log, TEXT("MainMenuWidget: Removing all delegate bindings from MultiplayerSubsystem."));
        MultiplayerSubsystem->MultiplayerOnCreateSessionComplete.RemoveAll(this);
        MultiplayerSubsystem->MultiplayerOnFindSessionsComplete.RemoveAll(this);
        MultiplayerSubsystem->MultiplayerOnJoinSessionComplete.RemoveAll(this);
    }
    Super::NativeDestruct();
}

void UMainMenuWidget::OnCreateButtonClicked()
{
    UE_LOG(LogMultiplayerMenu, Log, TEXT("MainMenuWidget: Create Button Clicked."));
    if (MultiplayerSubsystem)
    {
        CreateButton->SetIsEnabled(false);
        RefreshButton->SetIsEnabled(false);
        MultiplayerSubsystem->CreateSession(4, TEXT("PublicMatch"));
    }
}

void UMainMenuWidget::OnRefreshButtonClicked()
{
    UE_LOG(LogMultiplayerMenu, Log, TEXT("MainMenuWidget: Refresh Button Clicked."));
    if (MultiplayerSubsystem)
    {
        RefreshButton->SetIsEnabled(false);
        ServerListScrollBox->ClearChildren();
        MultiplayerSubsystem->FindSessions(10000);
    }
}

void UMainMenuWidget::OnCreateSession(bool bWasSuccessful)
{
    UE_LOG(LogMultiplayerMenu, Log, TEXT("MainMenuWidget: OnCreateSession callback received. bWasSuccessful: %d"), bWasSuccessful);
    // The subsystem now handles server travel on success.
    // We only need to handle failure here.
    if (!bWasSuccessful)
    {
        UE_LOG(LogMultiplayerMenu, Warning, TEXT("MainMenuWidget: Session creation failed. Re-enabling buttons."));
        CreateButton->SetIsEnabled(true);
        RefreshButton->SetIsEnabled(true);
    }
}

void UMainMenuWidget::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
    UE_LOG(LogMultiplayerMenu, Log, TEXT("MainMenuWidget: OnFindSessions callback received. bWasSuccessful: %d, Num Results: %d"), bWasSuccessful, SessionResults.Num());
    if (!bWasSuccessful || !ServerRowClass)
    {
        UE_LOG(LogMultiplayerMenu, Warning, TEXT("MainMenuWidget: Find sessions failed or ServerRowClass is not set. Re-enabling refresh button."));
        RefreshButton->SetIsEnabled(true);
        return;
    }

    for (const FOnlineSessionSearchResult& Result : SessionResults)
    {
        UServerRowWidget* ServerRow = CreateWidget<UServerRowWidget>(this, ServerRowClass);
        if (ServerRow)
        {
            FString MatchType;
            Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);
            UE_LOG(LogMultiplayerMenu, Log, TEXT("MainMenuWidget: Found Session - MatchType: %s, Ping: %dms"), *MatchType, Result.PingInMs);
            ServerRow->Setup(this, Result);
            ServerListScrollBox->AddChild(ServerRow);
        }
    }
    RefreshButton->SetIsEnabled(true);
}

void UMainMenuWidget::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
    UE_LOG(LogMultiplayerMenu, Log, TEXT("MainMenuWidget: OnJoinSession callback received. Result: %d"), Result);
    // The subsystem now handles client travel on success.
    // We only need to handle failure here.
    if (Result != EOnJoinSessionCompleteResult::Success)
    {
        UE_LOG(LogMultiplayerMenu, Warning, TEXT("MainMenuWidget: Join session failed. Re-enabling refresh button."));
        RefreshButton->SetIsEnabled(true);
    }
}