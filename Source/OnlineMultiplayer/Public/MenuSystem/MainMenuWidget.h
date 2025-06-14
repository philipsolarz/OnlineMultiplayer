// MenuSystem/MainMenuWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "StrafeMultiplayer/Public/MultiplayerSessionTypes.h"
#include "MainMenuWidget.generated.h"

class UButton;
class UScrollBox;
class UStrafeMultiplayerSubsystem;
class UServerRowWidget;

UCLASS()
class ONLINEMULTIPLAYER_API UMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void Setup();
    void Teardown();
    void JoinServer(const FOnlineSessionSearchResult& SessionResult);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UPROPERTY(meta = (BindWidget))
    UButton* CreateButton;

    UPROPERTY(meta = (BindWidget))
    UButton* RefreshButton;

    UPROPERTY(meta = (BindWidget))
    UScrollBox* ServerListScrollBox;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UServerRowWidget> ServerRowClass;

private:
    UFUNCTION()
    void OnCreateButtonClicked();

    UFUNCTION()
    void OnRefreshButtonClicked();

    // Delegate handlers for the new subsystem
    UFUNCTION()
    void OnCreateSessionComplete(EMultiplayerSessionResult Result);

    // The member function signatures must match the delegate signatures exactly
    void OnFindLobbiesComplete(const TArray<FOnlineSessionSearchResult>& SessionResults, EMultiplayerSessionResult Result);
    void OnFindDedicatedServersComplete(const TArray<FOnlineSessionSearchResult>& SessionResults, EMultiplayerSessionResult Result);
    void OnJoinSessionComplete(EMultiplayerSessionResult Result, const FString& ConnectString);

    UPROPERTY()
    TObjectPtr<UStrafeMultiplayerSubsystem> MultiplayerSubsystem;

    int32 FindSessionsRequestCount = 0;
};