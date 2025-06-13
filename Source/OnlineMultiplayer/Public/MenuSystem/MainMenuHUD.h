// MenuSystem/MainMenuHUD.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainMenuHUD.generated.h"

class UMainMenuWidget;

UCLASS()
class ONLINEMULTIPLAYER_API AMainMenuHUD : public AHUD
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

    UPROPERTY()
    UMainMenuWidget* MainMenuWidget;
};