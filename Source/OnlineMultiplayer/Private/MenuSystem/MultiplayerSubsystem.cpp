// MenuSystem/MultiplayerSubsystem.cpp

#include "MenuSystem/MultiplayerSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "GameFramework/PlayerController.h"

// Define the custom log category
DEFINE_LOG_CATEGORY(LogMultiplayerMenu);

UMultiplayerSubsystem::UMultiplayerSubsystem() :
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
	UE_LOG(LogMultiplayerMenu, Log, TEXT("MultiplayerSubsystem Constructor Called."));
}

void UMultiplayerSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	UE_LOG(LogMultiplayerMenu, Log, TEXT("CreateSession called with NumPublicConnections: %d, MatchType: %s"), NumPublicConnections, *MatchType);

	if (!IsValidSessionInterface())
	{
		UE_LOG(LogMultiplayerMenu, Warning, TEXT("CreateSession: SessionInterface is not valid."));
		MultiplayerOnCreateSessionComplete.Broadcast(false);
		return;
	}

	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		UE_LOG(LogMultiplayerMenu, Log, TEXT("CreateSession: Found an existing session. Destroying it before creating a new one."));
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = NumPublicConnections;
		LastMatchType = MatchType;

		DestroySession();
		return; // DestroySession will trigger OnDestroySessionComplete, which will re-trigger CreateSession
	}

	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;
	// We use Lobbies for matchmaking with the Steam game server browser
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LastSessionSettings->BuildUniqueId = 1;

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		UE_LOG(LogMultiplayerMenu, Error, TEXT("CreateSession: Call to SessionInterface->CreateSession failed."));
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}
	else
	{
		UE_LOG(LogMultiplayerMenu, Log, TEXT("CreateSession: Session creation request sent successfully."));
	}
}

void UMultiplayerSubsystem::FindSessions(int32 MaxSearchResults)
{
	UE_LOG(LogMultiplayerMenu, Log, TEXT("FindSessions called with MaxSearchResults: %d"), MaxSearchResults);

	if (!IsValidSessionInterface())
	{
		UE_LOG(LogMultiplayerMenu, Warning, TEXT("FindSessions: SessionInterface is not valid."));
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		UE_LOG(LogMultiplayerMenu, Error, TEXT("FindSessions: Call to SessionInterface->FindSessions failed."));
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
	else
	{
		UE_LOG(LogMultiplayerMenu, Log, TEXT("FindSessions: Session search request sent successfully."));
	}
}

void UMultiplayerSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	UE_LOG(LogMultiplayerMenu, Log, TEXT("JoinSession called."));

	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogMultiplayerMenu, Warning, TEXT("JoinSession: SessionInterface is not valid."));
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		UE_LOG(LogMultiplayerMenu, Error, TEXT("JoinSession: Call to SessionInterface->JoinSession failed."));
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
	else
	{
		UE_LOG(LogMultiplayerMenu, Log, TEXT("JoinSession: Join session request sent successfully."));
	}
}

void UMultiplayerSubsystem::DestroySession()
{
	UE_LOG(LogMultiplayerMenu, Log, TEXT("DestroySession called."));
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogMultiplayerMenu, Warning, TEXT("DestroySession: SessionInterface is not valid."));
		MultiplayerOnDestroySessionComplete.Broadcast(false);
		return;
	}

	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		UE_LOG(LogMultiplayerMenu, Error, TEXT("DestroySession: Call to SessionInterface->DestroySession failed."));
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		MultiplayerOnDestroySessionComplete.Broadcast(false);
	}
	else
	{
		UE_LOG(LogMultiplayerMenu, Log, TEXT("DestroySession: Destroy session request sent successfully."));
	}
}

void UMultiplayerSubsystem::StartSession()
{
	UE_LOG(LogMultiplayerMenu, Log, TEXT("StartSession called (not used in this example)."));
}

bool UMultiplayerSubsystem::IsValidSessionInterface()
{
	if (!SessionInterface)
	{
		IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
		if (Subsystem)
		{
			SessionInterface = Subsystem->GetSessionInterface();
			UE_LOG(LogMultiplayerMenu, Log, TEXT("Acquired SessionInterface from OnlineSubsystem: %s"), *Subsystem->GetSubsystemName().ToString());
		}
		else
		{
			UE_LOG(LogMultiplayerMenu, Error, TEXT("Could not get OnlineSubsystem."));
		}
	}
	return SessionInterface.IsValid();
}

void UMultiplayerSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogMultiplayerMenu, Log, TEXT("OnCreateSessionComplete: SessionName=%s, bWasSuccessful=%d"), *SessionName.ToString(), bWasSuccessful);
	if (SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	if (bWasSuccessful)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			UE_LOG(LogMultiplayerMenu, Log, TEXT("OnCreateSessionComplete: Server traveling to /Game/FirstPerson/LvL_FirstPerson?listen"));
			World->ServerTravel(TEXT("/Game/FirstPerson/LvL_FirstPerson?listen"));
		}
	}

	MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	UE_LOG(LogMultiplayerMenu, Log, TEXT("OnFindSessionsComplete: bWasSuccessful=%d"), bWasSuccessful);

	if (SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	if (!bWasSuccessful || !LastSessionSearch.IsValid())
	{
		UE_LOG(LogMultiplayerMenu, Warning, TEXT("OnFindSessionsComplete: Search failed or LastSessionSearch is invalid."));
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	UE_LOG(LogMultiplayerMenu, Log, TEXT("OnFindSessionsComplete: Found %d sessions."), LastSessionSearch->SearchResults.Num());
	MultiplayerOnFindSessionsComplete.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
}

void UMultiplayerSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogMultiplayerMenu, Log, TEXT("OnJoinSessionComplete: SessionName=%s, Result=%d"), *SessionName.ToString(), Result);

	if (SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString Address;
		if (SessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
		{
			UE_LOG(LogMultiplayerMenu, Log, TEXT("OnJoinSessionComplete: Client traveling to %s"), *Address);
			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{

				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
		else
		{
			UE_LOG(LogMultiplayerMenu, Warning, TEXT("OnJoinSessionComplete: Could not get resolved connect string."));
		}
	}
	else
	{
		UE_LOG(LogMultiplayerMenu, Warning, TEXT("OnJoinSessionComplete: Join was not successful. Result code: %d"), Result);
	}

	MultiplayerOnJoinSessionComplete.Broadcast(Result);
}

void UMultiplayerSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogMultiplayerMenu, Log, TEXT("OnDestroySessionComplete: SessionName=%s, bWasSuccessful=%d"), *SessionName.ToString(), bWasSuccessful);

	if (SessionInterface)
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}
	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false;
		UE_LOG(LogMultiplayerMenu, Log, TEXT("OnDestroySessionComplete: Re-creating session."));
		CreateSession(LastNumPublicConnections, LastMatchType);
	}
	MultiplayerOnDestroySessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogMultiplayerMenu, Log, TEXT("OnStartSessionComplete (not used): SessionName=%s, bWasSuccessful=%d"), *SessionName.ToString(), bWasSuccessful);
}