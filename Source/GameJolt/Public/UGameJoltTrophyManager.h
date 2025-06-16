#pragma once

#include "CoreMinimal.h"
#include "UGameJoltScoreManager.h"
#include "UObject/NoExportTypes.h"
#include "UGameJoltTypes.h"
#include "UGamejoltTrophyManager.generated.h"



class UGameJoltSubsystem;

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnFetchTrophiesComplete, bool, bSuccess, const TArray<FGameJoltTrophy>&, Trophies, const FString&, ErrorMessage);

UCLASS(BlueprintType)
class GAMEJOLT_API UGameJoltTrophyManager : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UGameJoltSubsystem* InSubsystem);

	/**
	 * Fetches trophies for a user.
	 * @param bAchieved A boolen to determine if the user has achieved the trophy
	 * @param OnComplete What to do when the request is complete
	 */
	UFUNCTION(BlueprintCallable, Category="Game Jolt|Trophies")
	void FetchTrophies(bool bAchieved, FOnFetchTrophiesComplete OnComplete);

private:
	TWeakObjectPtr<UGameJoltSubsystem> SubsystemPtr;
};
