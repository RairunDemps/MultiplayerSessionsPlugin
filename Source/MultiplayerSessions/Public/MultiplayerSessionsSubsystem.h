// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"

class IOnlineSubsystem;

typedef TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> IOnlineSessionPtr;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiplayerHostSessionComplete, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMultiplayerFindSessionComplete, const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMultiplayerJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result, FString TravelURL);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiplayerDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiplayerStartSessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiplayerLoginComplete, bool, bWasSuccessful);

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
    FOnMultiplayerLoginComplete OnMultiplayerLoginComplete;

    void HostSession(int32 InPublicConnectionsNumber, FString InTypeOfMatch);
    void FindSessions(int32 MaxSearchResults);
    void JoinSession(const FOnlineSessionSearchResult& SessionResult);
    void DestroySession();
    void StartSession();
    void Login();

    void Initialize(FSubsystemCollectionBase& Collection) override;
    void Deinitialize() override;

protected:
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnFindSessionsComplete(bool bWasSuccessful);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
    void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
    void OnLoginComplete(int LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

private:
    IOnlineSessionPtr SessionInterface;
    IOnlineSubsystem* Subsystem;
    TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
    TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

    FDelegateHandle CreateSessionCompleteDelegateHandle;
    FDelegateHandle FindSessionCompleteDelegateHandle;
    FDelegateHandle JoinSessionCompleteDelegateHandle;
    FDelegateHandle StartSessionCompleteDelegateHandle;
    FDelegateHandle DestroySessionCompleteDelegateHandle;
    FDelegateHandle EOSLoginDelegateHandle;

    int32 PublicConnectionsNumber = 4;
    FString TypeOfMatch = "FreeForAll";
    bool bCreateSession = false;
    bool bLoggedIn = false;
    int32 HostingPlayerNumber = 0;

    void CreateSession(int32 InPublicConnectionsNumber, FString InTypeOfMatch);
    void InitializeLastSessionSettings(int32 InPublicConnectionsNumber, FString InTypeOfMatch);
};
