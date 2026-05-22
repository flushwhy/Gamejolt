#include "UGameJoltBlueprintLibrary.h"
#include "UGameJoltSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

// ---------------------------------------------------------------------------
//  Internal helper
// ---------------------------------------------------------------------------

UGameJoltSubsystem* UGameJoltBlueprintLibrary::GetSubsystem(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		UE_LOG(LogTemp, Error, TEXT("GameJolt: WorldContextObject is null. Make sure 'self' is connected."));
		return nullptr;
	}

	const UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("GameJolt: Could not get World from WorldContextObject."));
		return nullptr;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("GameJolt: Could not get GameInstance from World."));
		return nullptr;
	}

	UGameJoltSubsystem* Subsystem = GameInstance->GetSubsystem<UGameJoltSubsystem>();
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("GameJolt: UGameJoltSubsystem not found. Make sure the plugin is enabled."));
	}

	return Subsystem;
}

// ---------------------------------------------------------------------------
//  Users
// ---------------------------------------------------------------------------

void UGameJoltBlueprintLibrary::Login(const UObject* WorldContextObject, const FString& Username, const FString& UserToken, FOnAuthUserComplete OnComplete)
{
	UGameJoltSubsystem* GJ = GetSubsystem(WorldContextObject);
	if (!GJ)
	{
		OnComplete.ExecuteIfBound(false, FGameJoltUser(), TEXT("GameJolt subsystem unavailable."));
		return;
	}
	GJ->UserManager->AuthenticateUser(OnComplete, Username, UserToken);
}

bool UGameJoltBlueprintLibrary::IsLoggedIn(const UObject* WorldContextObject)
{
	const UGameJoltSubsystem* GJ = GetSubsystem(WorldContextObject);
	return GJ ? GJ->IsUserAuthenticated() : false;
}

FString UGameJoltBlueprintLibrary::GetCurrentUsername(const UObject* WorldContextObject)
{
	const UGameJoltSubsystem* GJ = GetSubsystem(WorldContextObject);
	if (!GJ) return FString();

	FString Username, Token;
	GJ->GetActiveUser(Username, Token);
	return Username;
}

// ---------------------------------------------------------------------------
//  Sessions
// ---------------------------------------------------------------------------

void UGameJoltBlueprintLibrary::OpenSession(const UObject* WorldContextObject, FOnSessionComplete OnComplete)
{
	UGameJoltSubsystem* GJ = GetSubsystem(WorldContextObject);
	if (!GJ)
	{
		OnComplete.ExecuteIfBound(false, TEXT("GameJolt subsystem unavailable."));
		return;
	}
	GJ->SessionManager->OpenSession(OnComplete);
}

void UGameJoltBlueprintLibrary::CloseSession(const UObject* WorldContextObject, FOnSessionComplete OnComplete)
{
	UGameJoltSubsystem* GJ = GetSubsystem(WorldContextObject);
	if (!GJ)
	{
		OnComplete.ExecuteIfBound(false, TEXT("GameJolt subsystem unavailable."));
		return;
	}
	GJ->SessionManager->CloseSession(OnComplete);
}

void UGameJoltBlueprintLibrary::PingSession(const UObject* WorldContextObject, bool bIsActive)
{
	UGameJoltSubsystem* GJ = GetSubsystem(WorldContextObject);
	if (!GJ) return;
	GJ->SessionManager->PingSession(bIsActive);
}

// ---------------------------------------------------------------------------
//  Trophies
// ---------------------------------------------------------------------------

void UGameJoltBlueprintLibrary::FetchTrophies(const UObject* WorldContextObject, bool bOnlyAchieved, FOnFetchTrophiesComplete OnComplete)
{
	UGameJoltSubsystem* GJ = GetSubsystem(WorldContextObject);
	if (!GJ)
	{
		OnComplete.ExecuteIfBound(false, {}, TEXT("GameJolt subsystem unavailable."));
		return;
	}
	GJ->TrophyManager->FetchTrophies(bOnlyAchieved, OnComplete);
}

// ---------------------------------------------------------------------------
//  Scores
// ---------------------------------------------------------------------------

void UGameJoltBlueprintLibrary::FetchScores(const UObject* WorldContextObject, int32 TableID, int32 Limit, bool bCurrentUserOnly, FOnFetchScoresComplete OnComplete)
{
	UGameJoltSubsystem* GJ = GetSubsystem(WorldContextObject);
	if (!GJ)
	{
		OnComplete.ExecuteIfBound(false, {});
		return;
	}

	FString Username, Token;
	GJ->GetActiveUser(Username, Token);

	GJ->ScoreManager->FetchScores(OnComplete, TableID, Limit, bCurrentUserOnly, Username, Token);
}

void UGameJoltBlueprintLibrary::FetchScoreTables(const UObject* WorldContextObject, FOnFetchScoreTablesComplete OnComplete)
{
	UGameJoltSubsystem* GJ = GetSubsystem(WorldContextObject);
	if (!GJ)
	{
		OnComplete.ExecuteIfBound(false, {});
		return;
	}
	GJ->ScoreManager->FetchScoreTables(OnComplete);
}

void UGameJoltBlueprintLibrary::SubmitScore(const UObject* WorldContextObject, const FString& ScoreText, int32 SortValue, int32 TableID, FOnDataStoreOpComplete OnComplete)
{
	UGameJoltSubsystem* GJ = GetSubsystem(WorldContextObject);
	if (!GJ)
	{
		OnComplete.ExecuteIfBound(false, TEXT("GameJolt subsystem unavailable."));
		return;
	}
	GJ->ScoreManager->AddScore(OnComplete, SortValue, ScoreText, TableID);
}

void UGameJoltBlueprintLibrary::SubmitGuestScore(const UObject* WorldContextObject, const FString& GuestName, const FString& ScoreText, int32 SortValue, int32 TableID, FOnAddScoreComplete OnComplete)
{
	UGameJoltSubsystem* GJ = GetSubsystem(WorldContextObject);
	if (!GJ)
	{
		OnComplete.ExecuteIfBound(false, TEXT("GameJolt subsystem unavailable."));
		return;
	}
	GJ->ScoreManager->AddScoreForGuest(OnComplete, GuestName, ScoreText, SortValue, TableID);
}

void UGameJoltBlueprintLibrary::GetScoreRank(const UObject* WorldContextObject, int32 SortValue, int32 TableID, FOnGetRankComplete OnComplete)
{
	UGameJoltSubsystem* GJ = GetSubsystem(WorldContextObject);
	if (!GJ)
	{
		OnComplete.ExecuteIfBound(false, -1);
		return;
	}
	GJ->ScoreManager->GetScoreRank(OnComplete, SortValue, TableID);
}

// ---------------------------------------------------------------------------
//  Data Store
// ---------------------------------------------------------------------------

void UGameJoltBlueprintLibrary::SetGlobalData(const UObject* WorldContextObject, const FString& Key, const FString& Data, FOnDataStoreOpComplete OnComplete)
{
	UGameJoltSubsystem* GJ = GetSubsystem(WorldContextObject);
	if (!GJ)
	{
		OnComplete.ExecuteIfBound(false, TEXT("GameJolt subsystem unavailable."));
		return;
	}
	GJ->DataStoreManager->SetData(OnComplete, Key, Data);
}

void UGameJoltBlueprintLibrary::GetGlobalData(const UObject* WorldContextObject, const FString& Key, FOnDataStoreFetchComplete OnComplete)
{
	UGameJoltSubsystem* GJ = GetSubsystem(WorldContextObject);
	if (!GJ)
	{
		OnComplete.ExecuteIfBound(false, TEXT(""), TEXT("GameJolt subsystem unavailable."));
		return;
	}
	GJ->DataStoreManager->FetchData(OnComplete, Key);
}

void UGameJoltBlueprintLibrary::RemoveGlobalData(const UObject* WorldContextObject, const FString& Key, FOnDataStoreOpComplete OnComplete)
{
	UGameJoltSubsystem* GJ = GetSubsystem(WorldContextObject);
	if (!GJ)
	{
		OnComplete.ExecuteIfBound(false, TEXT("GameJolt subsystem unavailable."));
		return;
	}
	GJ->DataStoreManager->RemoveData(OnComplete, Key);
}

void UGameJoltBlueprintLibrary::SetUserData(const UObject* WorldContextObject, const FString& Key, const FString& Data, FOnDataStoreOpComplete OnComplete)
{
	UGameJoltSubsystem* GJ = GetSubsystem(WorldContextObject);
	if (!GJ)
	{
		OnComplete.ExecuteIfBound(false, TEXT("GameJolt subsystem unavailable."));
		return;
	}

	FString Username, Token;
	GJ->GetActiveUser(Username, Token);
	if (Username.IsEmpty())
	{
		OnComplete.ExecuteIfBound(false, TEXT("No authenticated user. Call GJ Login first."));
		return;
	}

	GJ->DataStoreManager->SetUserData(OnComplete, Username, Token, Key, Data);
}

void UGameJoltBlueprintLibrary::GetUserData(const UObject* WorldContextObject, const FString& Key, FOnDataStoreFetchComplete OnComplete)
{
	UGameJoltSubsystem* GJ = GetSubsystem(WorldContextObject);
	if (!GJ)
	{
		OnComplete.ExecuteIfBound(false, TEXT(""), TEXT("GameJolt subsystem unavailable."));
		return;
	}

	FString Username, Token;
	GJ->GetActiveUser(Username, Token);
	if (Username.IsEmpty())
	{
		OnComplete.ExecuteIfBound(false, TEXT(""), TEXT("No authenticated user. Call GJ Login first."));
		return;
	}

	GJ->DataStoreManager->FetchUserData(OnComplete, Username, Token, Key);
}

void UGameJoltBlueprintLibrary::RemoveUserData(const UObject* WorldContextObject, const FString& Key, FOnDataStoreOpComplete OnComplete)
{
	UGameJoltSubsystem* GJ = GetSubsystem(WorldContextObject);
	if (!GJ)
	{
		OnComplete.ExecuteIfBound(false, TEXT("GameJolt subsystem unavailable."));
		return;
	}

	FString Username, Token;
	GJ->GetActiveUser(Username, Token);
	if (Username.IsEmpty())
	{
		OnComplete.ExecuteIfBound(false, TEXT("No authenticated user. Call GJ Login first."));
		return;
	}

	GJ->DataStoreManager->RemoveUserData(OnComplete, Username, Token, Key);
}
