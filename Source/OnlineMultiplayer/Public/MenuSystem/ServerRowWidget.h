// MenuSystem/ServerRowWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "ServerRowWidget.generated.h"

class UMainMenuWidget;
class UButton;
class UTextBlock;

UCLASS()
class ONLINEMULTIPLAYER_API UServerRowWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void Setup(UMainMenuWidget* InParent, const FOnlineSessionSearchResult& InSearchResult);

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void OnJoinButtonClicked();

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ServerNameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PlayerCountText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PingText;

    UPROPERTY(meta = (BindWidget))
    UButton* JoinButton;

    UPROPERTY()
    UMainMenuWidget* Parent;

    FOnlineSessionSearchResult SearchResult;
};