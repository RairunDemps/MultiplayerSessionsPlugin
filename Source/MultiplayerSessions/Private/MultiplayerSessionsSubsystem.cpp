// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogMultiplayerSessionSubsystem, All, All)

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
    : CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
      FindSessionCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
      JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
      StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete)),
      DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete))
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem) return;

    SessionInterface = Subsystem->GetSessionInterface();
    UE_LOG(LogMultiplayerSessionSubsystem, Display, TEXT("Online Subsystem found %s"), *Subsystem->GetSubsystemName().ToString())
}

void UMultiplayerSessionsSubsystem::HostSession(int32 NumPublicConnections, FString MatchType)
{
    if (!SessionInterface.IsValid()) return;

    FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
    if (ExistingSession)
    {
        PublicConnectionsNumber = NumPublicConnections;
        TypeOfMatch = MatchType;
        bCreateSession = true;
        DestroySession();
        return;
    }

    CreateSession(NumPublicConnections, MatchType);
}

void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
    if (!SessionInterface.IsValid()) return;

    InitializeLastSessionSettings(NumPublicConnections, MatchType);
    if (!LastSessionSettings.IsValid()) return;

    UWorld* World = GetWorld();
    if (!World) return;

    const ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController();
    if (!LocalPlayer) return;

    CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);
    if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
        OnMultiplayerHostSessionComplete.Broadcast(false);
    }
}

void UMultiplayerSessionsSubsystem::InitializeLastSessionSettings(int32 NumPublicConnections, FString MatchType)
{
    LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
    if (!LastSessionSettings.IsValid()) return;

    const FName SubsystemName = IOnlineSubsystem::Get()->GetSubsystemName();
    LastSessionSettings->bIsLANMatch = SubsystemName.IsEqual(TEXT("NULL"), ENameCase::CaseSensitive);
    LastSessionSettings->NumPublicConnections = NumPublicConnections;
    LastSessionSettings->bShouldAdvertise = true;
    LastSessionSettings->bUsesPresence = true;
    LastSessionSettings->bAllowJoinInProgress = true;
    LastSessionSettings->bAllowJoinViaPresence = true;
    LastSessionSettings->bUseLobbiesIfAvailable = true;
    LastSessionSettings->BuildUniqueId = GetBuildUniqueId();
    LastSessionSettings->Set(FName(TEXT("MatchType")), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
}

void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
    if (!SessionInterface.IsValid())
    {
        OnMultiplayerFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
        return;
    }

    LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
    if (!LastSessionSearch.IsValid() || !GetWorld() || !GetWorld()->GetFirstLocalPlayerFromController())
    {
        OnMultiplayerFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
        return;
    }

    const FName SubsystemName = IOnlineSubsystem::Get()->GetSubsystemName();
    LastSessionSearch->bIsLanQuery = SubsystemName.IsEqual(TEXT("NULL"), ENameCase::CaseSensitive);
    FindSessionCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegate);
    LastSessionSearch->MaxSearchResults = MaxSearchResults;
    LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
    {
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegateHandle);
        OnMultiplayerFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
    }
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
    if (!SessionInterface.IsValid() || !GetWorld() || !GetWorld()->GetFirstLocalPlayerFromController())
    {
        OnMultiplayerJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError, TEXT(""));
        return;
    }

    JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
    {
        SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
        OnMultiplayerJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError, TEXT(""));
    }
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
    if (!SessionInterface.IsValid())
    {
        OnMultiplayerDestroySessionComplete.Broadcast(false);
        return;
    }

    DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);
    if (!SessionInterface->DestroySession(NAME_GameSession))
    {
        SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
        OnMultiplayerDestroySessionComplete.Broadcast(false);
    }
}

void UMultiplayerSessionsSubsystem::StartSession() {}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
    }

    OnMultiplayerHostSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegateHandle);
    }

    if (!LastSessionSearch.IsValid() || LastSessionSearch->SearchResults.Num() <= 0)
    {
        OnMultiplayerFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
        return;
    }

    OnMultiplayerFindSessionComplete.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    FString TravelURL;
    if (!SessionInterface.IsValid() || !SessionInterface->GetResolvedConnectString(SessionName, TravelURL))
    {
        OnMultiplayerJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError, TEXT(""));
        return;
    }

    SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
    OnMultiplayerJoinSessionComplete.Broadcast(Result, TravelURL);
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful) {}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
    }

    if (bWasSuccessful && bCreateSession)
    {
        bCreateSession = false;
        CreateSession(PublicConnectionsNumber, TypeOfMatch);
    }

    OnMultiplayerDestroySessionComplete.Broadcast(bWasSuccessful);
}
