#pragma once

#include "CoreMinimal.h"
#include "UGameJoltTypes.generated.h"

// -----------------------------------------------------------------------
//  Enums
// -----------------------------------------------------------------------

UENUM(BlueprintType)
enum class EGameJoltTrophyDifficulty : uint8
{
	Bronze   UMETA(DisplayName = "Bronze"),
	Silver   UMETA(DisplayName = "Silver"),
	Gold     UMETA(DisplayName = "Gold"),
	Platinum UMETA(DisplayName = "Platinum"),
};

// -----------------------------------------------------------------------
//  Structs
// -----------------------------------------------------------------------

USTRUCT(BlueprintType)
struct FGameJoltTrophy
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Trophy")
	FString id;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Trophy")
	FString title;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Trophy")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Trophy")
	EGameJoltTrophyDifficulty difficulty;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Trophy")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Trophy")
	FString achieved;
};

USTRUCT(BlueprintType)
struct FGameJoltUser
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|User")
	FString id;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|User")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|User")
	FString username;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|User")
	FString avatar_url;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|User")
	FString signed_up;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|User")
	int32 signed_up_timestamp = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|User")
	FString last_logged_in;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|User")
	int32 last_logged_in_timestamp = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|User")
	FString status;
};

USTRUCT(BlueprintType)
struct FGameJoltScore
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score")
	FString score;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score")
	int32 sort = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score")
	FString extra_data;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score")
	FString user_id;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score")
	FString user;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score")
	FString guest;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score")
	FString stored;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score")
	int32 stored_timestamp = 0;
};

USTRUCT(BlueprintType)
struct FGameJoltScoreTable
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score Table")
	FString id;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score Table")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score Table")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score Table")
	bool primary = false;
};

USTRUCT(BlueprintType)
struct FGameJoltDataKey
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Data Store")
	FString key;
};

USTRUCT(BlueprintType)
struct FGameJoltFriend
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Friend")
	FString friend_id;
};

USTRUCT(BlueprintType)
struct FGameJoltServerTime
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Time")
	int32 timestamp = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Time")
	FString timezone;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Time")
	int32 year = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Time")
	int32 month = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Time")
	int32 day = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Time")
	int32 hour = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Time")
	int32 minute = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Time")
	int32 second = 0;
};

// -----------------------------------------------------------------------
//  Delegates
//  Declared here so all managers can include just UGameJoltTypes.h
//  without needing to cross-include each other's headers.
// -----------------------------------------------------------------------

// Users
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnAuthUserComplete,
	bool, bSuccess,
	const FGameJoltUser&, User,
	const FString&, ErrorMessage);

// Sessions
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnSessionComplete,
	bool, bSuccess,
	const FString&, ErrorMessage);

// Trophies
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnFetchTrophiesComplete,
	bool, bSuccess,
	const TArray<FGameJoltTrophy>&, Trophies,
	const FString&, ErrorMessage);

// Scores
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnFetchScoreTablesComplete,
	bool, bSuccess,
	const TArray<FGameJoltScoreTable>&, ScoreTables);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnFetchScoresComplete,
	bool, bSuccess,
	const TArray<FGameJoltScore>&, Scores);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnAddScoreComplete,
	bool, bSuccess,
	const FString&, ErrorMessage);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGetRankComplete,
	bool, bSuccess,
	int32, Rank);

// Data Store
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnDataStoreOpComplete,
	bool, bSuccess,
	const FString&, ErrorMessage);

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnDataStoreFetchComplete,
	bool, bSuccess,
	const FString&, Data,
	const FString&, ErrorMessage);

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnDataStoreKeysComplete,
	bool, bSuccess,
	const TArray<FGameJoltDataKey>&, Keys,
	const FString&, ErrorMessage);