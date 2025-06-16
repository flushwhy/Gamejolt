// GameJoltTypes.h (Corrected and Complete)

#pragma once

#include "CoreMinimal.h"
#include "UGameJoltTypes.generated.h"

/**
 * @brief Represents the difficulty of a Game Jolt Trophy. 
 */
UENUM(BlueprintType) // FIX: Added UENUM macro to make it visible to Unreal's systems
enum class EGameJoltTrophyDifficulty : uint8
{
    Bronze          UMETA(DisplayName = "Bronze"),
    Silver          UMETA(DisplayName = "Silver"),
    Gold            UMETA(DisplayName = "Gold"),
    Platinum        UMETA(DisplayName = "Platinum"),
};

/**
 * @brief Represents a Game Jolt Trophy.
 */
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

    // FIX: Changed from FString to the enum for better type safety
    UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Trophy")
    EGameJoltTrophyDifficulty difficulty;

    UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Trophy")
    FString image_url;

    UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Trophy")
    FString achieved; // "false" or timestamp
};

/**
 * @brief Represents a Game Jolt User.
 */
USTRUCT(BlueprintType)
struct FGameJoltUser
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|User")
    FString id;

    UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|User")
    FString type; // User, Developer. Mod and so on

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
    FString status; // Active, Banned
};

/**
 * @brief Represents a score entry from a score table.
 */
USTRUCT(BlueprintType)
struct FGameJoltScore
{
    GENERATED_BODY()

    /** The score string. E.g., "500 Points". */
    UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score")
    FString score;

    /** The numerical value of the score used for sorting. */
    UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score")
    int32 sort = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score")
    FString extra_data;

    UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score")
    FString user_id;

    UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score")
    FString user; // Username, if the score belongs to a registered user.

    UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score")
    FString guest; // Guest name, if the score was submitted by a guest.

    UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score")
    FString stored;

    UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Score")
    int32 stored_timestamp = 0;
};

/**
 * @brief Represents a single score table for the game.
 */
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


/**
 * @brief Represents a key from the game's data store.
 */
USTRUCT(BlueprintType)
struct FGameJoltDataKey
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Data Store")
    FString key;
};

/**
 * @brief Represents a user's friend relationship.
 */
USTRUCT(BlueprintType)
struct FGameJoltFriend
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Game Jolt|Friend")
    FString friend_id;
};

/**
 * @brief Represents the server time from the Game Jolt API.
 */
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