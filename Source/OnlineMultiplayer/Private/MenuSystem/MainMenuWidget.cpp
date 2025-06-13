// MenuSystem/MainMenuWidget.cpp

#include "MenuSystem/MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "MenuSystem/MultiplayerSubsystem.h"
#include "MenuSystem/ServerRowWidget.h"

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
        MultiplayerSubsystem = GameInstance->GetSubsystem<UMultiplayerSubsystem>();
        if (MultiplayerSubsystem)
        {
            MultiplayerSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &UMainMenuWidget::OnCreateSession);
            MultiplayerSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &UMainMenuWidget::OnFindSessions);
            MultiplayerSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &UMainMenuWidget::OnJoinSession);
        }
    }
}

void UMainMenuWidget::NativeDestruct()
{
    if (MultiplayerSubsystem)
    {
        MultiplayerSubsystem->MultiplayerOnCreateSessionComplete.RemoveAll(this);
        MultiplayerSubsystem->MultiplayerOnFindSessionsComplete.RemoveAll(this);
        MultiplayerSubsystem->MultiplayerOnJoinSessionComplete.RemoveAll(this);
    }
    Super::NativeDestruct();
}

void UMainMenuWidget::OnCreateButtonClicked()
{
    if (MultiplayerSubsystem)
    {
        CreateButton->SetIsEnabled(false);
        RefreshButton->SetIsEnabled(false);
        MultiplayerSubsystem->CreateSession(4, TEXT("PublicMatch"));
    }
}

void UMainMenuWidget::OnRefreshButtonClicked()
{
    if (MultiplayerSubsystem)
    {
        RefreshButton->SetIsEnabled(false);
        ServerListScrollBox->ClearChildren();
        MultiplayerSubsystem->FindSessions(10000);
    }
}

void UMainMenuWidget::OnCreateSession(bool bWasSuccessful)
{
    // The subsystem now handles server travel on success.
    // We only need to handle failure here.
    if (!bWasSuccessful)
    {
        CreateButton->SetIsEnabled(true);
        RefreshButton->SetIsEnabled(true);
    }
}

void UMainMenuWidget::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
    if (!bWasSuccessful || !ServerRowClass)
    {
        RefreshButton->SetIsEnabled(true);
        return;
    }

    for (const FOnlineSessionSearchResult& Result : SessionResults)
    {
        UServerRowWidget* ServerRow = CreateWidget<UServerRowWidget>(this, ServerRowClass);
        if (ServerRow)
        {
            ServerRow->Setup(this, Result);
            ServerListScrollBox->AddChild(ServerRow);
        }
    }
    RefreshButton->SetIsEnabled(true);
}

void UMainMenuWidget::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
    // The subsystem now handles client travel on success.
    // We only need to handle failure here.
    if (Result != EOnJoinSessionCompleteResult::Success)
    {
        RefreshButton->SetIsEnabled(true);
    }
}