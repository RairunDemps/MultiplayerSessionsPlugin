// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSessionsMenuWidget.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogMultiplayerSessionsMenuWidget, All, All)

void UMultiplayerSessionsMenuWidget::Setup(int32 NumPublicConnections, FString MatchType, FString LobbyPath)
{
    UWorld* const World = GetWorld();
    if (!World) return;

    UGameInstance* const GameInstance = World->GetGameInstance();
    if (!GameInstance) return;

    APlayerController* const Controller = World->GetFirstPlayerController();
    if (!Controller) return;

    MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
    if (!MultiplayerSessionsSubsystem) return;

    bIsFocusable = true;
    PublicConnectionsNumber = NumPublicConnections;
    TypeOfMatch = MatchType;
    PathForLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
    BindDelegates();

    FInputModeUIOnly InputModeData;
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputModeData.SetWidgetToFocus(TakeWidget());
    Controller->SetInputMode(InputModeData);
    Controller->SetShowMouseCursor(true);
    SetVisibility(ESlateVisibility::Visible);
    AddToViewport();
}

void UMultiplayerSessionsMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (HostButton)
    {
        HostButton->OnClicked.AddDynamic(this, &ThisClass::OnHostButtonClicked);
    }

    if (JoinButton)
    {
        JoinButton->OnClicked.AddDynamic(this, &ThisClass::OnJoinButtonClicked);
    }
}

void UMultiplayerSessionsMenuWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
    TearDown();

    Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}

void UMultiplayerSessionsMenuWidget::OnHostButtonClicked()
{
    if (!MultiplayerSessionsSubsystem) return;

    HostButton->SetIsEnabled(false);
    MultiplayerSessionsSubsystem->HostSession(PublicConnectionsNumber, TypeOfMatch);
}

void UMultiplayerSessionsMenuWidget::OnJoinButtonClicked()
{
    if (!MultiplayerSessionsSubsystem) return;

    JoinButton->SetIsEnabled(false);
    MultiplayerSessionsSubsystem->FindSessions(MaxSearchResults);
}

void UMultiplayerSessionsMenuWidget::TearDown()
{
    UWorld* const World = GetWorld();
    if (!World) return;

    APlayerController* const Controller = World->GetFirstPlayerController();
    if (!Controller) return;

    FInputModeGameOnly InputModeData;
    Controller->SetInputMode(InputModeData);
    Controller->SetShowMouseCursor(true);
    SetVisibility(ESlateVisibility::Hidden);
}

void UMultiplayerSessionsMenuWidget::BindDelegates()
{
    MultiplayerSessionsSubsystem->OnMultiplayerHostSessionComplete.AddDynamic(this, &ThisClass::OnHostSessionComplete);
    MultiplayerSessionsSubsystem->OnMultiplayerDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySessionComplete);
    MultiplayerSessionsSubsystem->OnMultiplayerStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSessionComplete);
    MultiplayerSessionsSubsystem->OnMultiplayerFindSessionComplete.AddUObject(this, &ThisClass::OnFindSessionComplete);
    MultiplayerSessionsSubsystem->OnMultiplayerJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSessionComplete);
}

void UMultiplayerSessionsMenuWidget::OnHostSessionComplete(bool bWasSuccessful)
{
    if (!bWasSuccessful || !GetWorld())
    {
        HostButton->SetIsEnabled(true);
        return;
    }

    GetWorld()->ServerTravel(PathForLobby);
}

void UMultiplayerSessionsMenuWidget::OnFindSessionComplete(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful)
{
    if (!bWasSuccessful || !MultiplayerSessionsSubsystem)
    {
        JoinButton->SetIsEnabled(true);
        return;
    }

    for (const FOnlineSessionSearchResult& SearchResult : SearchResults)
    {
        FString MatchType;
        SearchResult.Session.SessionSettings.Get(FName(TEXT("MatchType")), MatchType);
        if (MatchType.Equals(TypeOfMatch))
        {
            MultiplayerSessionsSubsystem->JoinSession(SearchResult);
            return;
        }
    }
}

void UMultiplayerSessionsMenuWidget::OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result, FString TravelURL)
{
    if (EOnJoinSessionCompleteResult::Success != Result || !GetWorld() || !GetWorld()->GetFirstPlayerController())
    {
        JoinButton->SetIsEnabled(true);
        return;
    }

    APlayerController* const Controller = GetWorld()->GetFirstPlayerController();
    Controller->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
}

void UMultiplayerSessionsMenuWidget::OnDestroySessionComplete(bool bWasSuccessful) {}

void UMultiplayerSessionsMenuWidget::OnStartSessionComplete(bool bWasSuccessful) {}
