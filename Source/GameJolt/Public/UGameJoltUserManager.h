// UGameJoltUserManager.h (Corrected)

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UGameJoltTypes.h"
#include "UGameJoltUserManager.generated.h" 

class UGameJoltSubsystem;

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnAuthUserComplete, bool, bSuccess, const FGameJoltUser&, User, const FString&, ErrorMessage);

UCLASS(BlueprintType)
class GAMEJOLT_API UGameJoltUserManager : public UObject
{
	GENERATED_BODY()

public:
	
	void Initialize(UGameJoltSubsystem* InSubsystem);

	UFUNCTION(BlueprintCallable, Category = "Game Jolt|User")
	void AuthenticateUser(FOnAuthUserComplete OnComplete, const FString& Username, const FString& UserToken);

private:
	TWeakObjectPtr<UGameJoltSubsystem> SubsystemPtr;
};