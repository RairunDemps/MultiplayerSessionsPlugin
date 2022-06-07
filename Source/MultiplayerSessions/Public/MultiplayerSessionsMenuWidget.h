// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsMenuWidget.generated.h"

class UButton;
class UMultiplayerSessionsSubsystem;

UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void Setup(int32 NumPublicConnections = 4, FString MatchType = FString(TEXT("FreeForAll")),
        FString LobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/Lobby")));

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* HostButton;

    UPROPERTY(meta = (BindWidget))
    UButton* JoinButton;

    void NativeOnInitialized() override;
    void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld);

private:
    UFUNCTION()
    void OnHostButtonClicked();

    UFUNCTION()
    void OnJoinButtonClicked();

    UFUNCTION()
    void OnHostSessionComplete(bool bWasSuccessful);

    UFUNCTION()
    void OnDestroySessionComplete(bool bWasSuccessful);

    UFUNCTION()
    void OnStartSessionComplete(bool bWasSuccessful);

    void OnFindSessionComplete(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful);
    void OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result, FString TravelURL);

    void TearDown();
    void BindDelegates();

    UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

    int32 PublicConnectionsNumber;
    FString TypeOfMatch;
    FString PathForLobby;

    int32 MaxSearchResults = 10000;
};
