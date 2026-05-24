#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UGameJoltTypes.h"
#include "UGameJoltTrophyManager.generated.h"

class UGameJoltSubsystem;

UCLASS(BlueprintType)
class GAMEJOLT_API UGameJoltTrophyManager : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UGameJoltSubsystem* InSubsystem);

	/**
	 * Fetches trophies for the currently logged-in user.
	 * @param bAchieved  True = only return earned trophies. False = return all trophies.
	 * @param OnComplete Fires when done. Trophies array is populated on success.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Trophies")
	void FetchTrophies(bool bAchieved, FOnFetchTrophiesComplete OnComplete);

private:
	/*
	* To prevent overlapping requests which is a BluePrint issue.
	*/
	bool bFetchTrophiesInFlight = false;

	TWeakObjectPtr<UGameJoltSubsystem> SubsystemPtr;
};