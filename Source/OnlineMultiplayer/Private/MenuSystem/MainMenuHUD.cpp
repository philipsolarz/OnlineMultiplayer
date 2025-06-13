// MenuSystem/MainMenuHUD.cpp

#include "MenuSystem/MainMenuHUD.h"
#include "MenuSystem/MainMenuWidget.h"
#include "Blueprint/UserWidget.h"

void AMainMenuHUD::BeginPlay()
{
    Super::BeginPlay();

    if (MainMenuWidgetClass)
    {
        MainMenuWidget = CreateWidget<UMainMenuWidget>(GetOwningPlayerController(), MainMenuWidgetClass);
        if (MainMenuWidget)
        {
            MainMenuWidget->Setup();
        }
    }
}