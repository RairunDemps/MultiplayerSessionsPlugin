// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"

typedef TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> IOnlineSessionPtr;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiplayerHostSessionComplete, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMultiplayerFindSessionComplete, const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMultiplayerJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result, FString TravelURL);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiplayerDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiplayerStartSessionComplete, bool, bWasSuccessful);

UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UMultiplayerSessionsSubsystem();

    FOnMultiplayerHostSessionComplete OnMultiplayerHostSessionComplete;
    FOnMultiplayerFindSessionComplete OnMultiplayerFindSessionComplete;
    FOnMultiplayerJoinSessionComplete OnMultiplayerJoinSessionComplete;
    FOnMultiplayerDestroySessionComplete OnMultiplayerDestroySessionComplete;
    FOnMultiplayerStartSessionComplete OnMultiplayerStartSessionComplete;

    void HostSession(int32 NumPublicConnections, FString MatchType);
    void FindSessions(int32 MaxSearchResults);
    void JoinSession(const FOnlineSessionSearchResult& SessionResult);
    void DestroySession();
    void StartSession();

protected:
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnFindSessionsComplete(bool bWasSuccessful);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
    void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

private:
    IOnlineSessionPtr SessionInterface;
    TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
    TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

    FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
    FDelegateHandle CreateSessionCompleteDelegateHandle;
    FOnFindSessionsCompleteDelegate FindSessionCompleteDelegate;
    FDelegateHandle FindSessionCompleteDelegateHandle;
    FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
    FDelegateHandle JoinSessionCompleteDelegateHandle;
    FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
    FDelegateHandle StartSessionCompleteDelegateHandle;
    FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
    FDelegateHandle DestroySessionCompleteDelegateHandle;

    int32 PublicConnectionsNumber = 4;
    FString TypeOfMatch = "FreeForAll";
    bool bCreateSession = false;

    void CreateSession(int32 NumPublicConnections, FString MatchType);
    void InitializeLastSessionSettings(int32 NumPublicConnections, FString MatchType);
};
