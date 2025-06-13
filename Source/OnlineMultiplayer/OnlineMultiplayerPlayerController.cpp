// Copyright Epic Games, Inc. All Rights Reserved.


#include "OnlineMultiplayerPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "OnlineMultiplayerCameraManager.h"

AOnlineMultiplayerPlayerController::AOnlineMultiplayerPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = AOnlineMultiplayerCameraManager::StaticClass();
}

void AOnlineMultiplayerPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}
	}
}
