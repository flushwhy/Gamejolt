#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UGameJoltTypes.h"
#include "UGameJoltScoreManager.generated.h"

class UGameJoltSubsystem;

UCLASS(BlueprintType)
class GAMEJOLT_API UGameJoltScoreManager : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UGameJoltSubsystem* InSubsystem);

	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Scores")
	void FetchScoreTables(FOnFetchScoreTablesComplete OnComplete);

	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Scores")
	void FetchScores(FOnFetchScoresComplete OnComplete, int32 TableID = 0, int32 Limit = 10, bool bForUserOnly = false, const FString& Username = TEXT(""), const FString& UserToken = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Scores")
	void AddScoreForGuest(FOnAddScoreComplete OnComplete, const FString& GuestName, const FString& ScoreString, int32 SortValue, int32 TableID = 0, const FString& ExtraData = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Scores")
	void GetScoreRank(FOnGetRankComplete OnComplete, int32 SortValue, int32 TableID = 0);

	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Scores")
	void AddScore(FOnDataStoreOpComplete OnComplete, int32 SortValue, FString ScoreText, int32 TableID = 0);

private:
	/*
	* To prevent overlapping requests which is a BluePrint issue.
	*/
	bool bFetchingScoresTablesInFlight = false;
	bool bFetchingScoresInFlight = false;
	bool bAddScoreForGuestInFlight = false;
	bool bGetScoreRankInFlight = false;
	bool bAddScoreInFlight = false;


	TWeakObjectPtr<UGameJoltSubsystem> SubsystemPtr;
};