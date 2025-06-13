// MenuSystem/MainMenuWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MainMenuWidget.generated.h"

class UButton;
class UScrollBox;
class UMultiplayerSubsystem;
class UServerRowWidget;

UCLASS()
class ONLINEMULTIPLAYER_API UMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void Setup();
    void Teardown();

    // Called from ServerRowWidget to initiate joining
    void JoinServer(const FOnlineSessionSearchResult& SessionResult);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    // Widget bindings
    UPROPERTY(meta = (BindWidget))
    UButton* CreateButton;

    UPROPERTY(meta = (BindWidget))
    UButton* RefreshButton;

    UPROPERTY(meta = (BindWidget))
    UScrollBox* ServerListScrollBox;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UServerRowWidget> ServerRowClass;

private:
    // Button click handlers
    UFUNCTION()
    void OnCreateButtonClicked();

    UFUNCTION()
    void OnRefreshButtonClicked();

    // Delegate handlers for MultiplayerSubsystem
    UFUNCTION()
    void OnCreateSession(bool bWasSuccessful);
    void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
    void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

    UPROPERTY()
    TObjectPtr<UMultiplayerSubsystem> MultiplayerSubsystem;
};