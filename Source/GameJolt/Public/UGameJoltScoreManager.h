#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UGameJoltTypes.h" // Includes the FGameJoltScore and FGameJoltScoreTable structs
#include "UGameJoltScoreManager.generated.h"

// Forward declare the subsystem to avoid circular dependencies in headers
class UGameJoltSubsystem;

// Delegate declarations for asynchronous callbacks
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnFetchScoreTablesComplete, bool, bSuccess, const TArray<FGameJoltScoreTable>&, ScoreTables);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnFetchScoresComplete, bool, bSuccess, const TArray<FGameJoltScore>&, Scores);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnAddScoreComplete, bool, bSuccess, const FString&, ErrorMessage);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGetRankComplete, bool, bSuccess, int32, Rank);


/**
 * Manages all API calls related to Game Jolt scores and score tables.
 */
UCLASS(BlueprintType)
class GAMEJOLT_API UGameJoltScoreManager : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Initializes the manager with a reference to the main subsystem.
	 * This should be called by the UGameJoltSubsystem during its own initialization.
	 * @param InSubsystem The main Game Jolt subsystem instance.
	 */
	void Initialize(UGameJoltSubsystem* InSubsystem);

	/**
	 * Fetches all available score tables for the game.
	 * @param OnComplete Delegate executed when the request completes with the list of tables.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Scores")
	void FetchScoreTables(FOnFetchScoreTablesComplete OnComplete);

	/**
	 * Fetches scores from a specific score table.
	 * @param OnComplete Delegate executed when the request completes.
	 * @param TableID The ID of the table to fetch scores from. Use 0 for the primary table.
	 * @param Limit The maximum number of scores to return.
	 * @param bForUserOnly Set true to only fetch scores for the currently authenticated user.
	 * @param Username The username of the authenticated user (required if bForUserOnly is true).
	 * @param UserToken The user's token (required if bForUserOnly is true).
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Scores")
	void FetchScores(FOnFetchScoresComplete OnComplete, int32 TableID = 0, int32 Limit = 10, bool bForUserOnly = false, const FString& Username = TEXT(""), const FString& UserToken = TEXT(""));

	/**
	 * Adds a score for a guest user. This is a POST request.
	 * @param OnComplete Delegate executed when the request completes.
	 * @param GuestName The name of the guest submitting the score.
	 * @param ScoreString The score to display (e.g., "500 Points").
	 * @param SortValue The numerical value used for sorting the score.
	 * @param TableID The ID of the table to add the score to. Use 0 for the primary table.
	 * @param ExtraData Optional extra data to attach to the score.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Scores")
	void AddScoreForGuest(FOnAddScoreComplete OnComplete, const FString& GuestName, const FString& ScoreString, int32 SortValue, int32 TableID = 0, const FString& ExtraData = TEXT(""));

	/**
	 * Gets the rank of a specific score value in a table.
	 * @param OnComplete Delegate executed when the request completes.
	 * @param SortValue The numerical value to find the rank of.
	 * @param TableID The ID of the table to check against. Use 0 for the primary table.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Scores")
	void GetScoreRank(FOnGetRankComplete OnComplete, int32 SortValue, int32 TableID = 0);

private:
	/** A weak pointer to the subsystem to avoid circular references and dangling pointers. */
	TWeakObjectPtr<UGameJoltSubsystem> SubsystemPtr;
};