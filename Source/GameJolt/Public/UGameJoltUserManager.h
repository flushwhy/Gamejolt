#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UGameJoltTypes.h"
#include "UGameJoltUserManager.generated.h"

class UGameJoltSubsystem;

// FOnAuthUserComplete is declared in UGameJoltTypes.h - do NOT redeclare it here.

UCLASS(BlueprintType)
class GAMEJOLT_API UGameJoltUserManager : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UGameJoltSubsystem* InSubsystem);

	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Users")
	void AuthenticateUser(FOnAuthUserComplete OnComplete, const FString& Username, const FString& UserToken);

private:
	/*
	* To prevent overlapping requests which is a BluePrint issue.
	*/
	bool bAuthenticateUserInFlight = false;

	TWeakObjectPtr<UGameJoltSubsystem> SubsystemPtr;
};