#pragma once

#include "CoreMinimal.h"
#include "UGameJoltTypes.h"
#include "UGameJoltDataStoreManager.generated.h"

class UGameJoltSubsystem;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnDataStoreOpComplete, bool, bSuccess, const FString&, ErrorMessage);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnDataStoreFetchComplete, bool, bSuccess, const FString&, Data, const FString&, ErrorMessage);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnDataStoreKeysComplete, bool, bSuccess, const TArray<FGameJoltDataKey>&, Keys, const FString&, ErrorMessage);


UCLASS(BlueprintType)
class GAMEJOLT_API UGameJoltDataStoreManager : public UObject
{
	GENERATED_BODY()
	
public:

	void Initialize(UGameJoltSubsystem* InSubsystem);
	
	UFUNCTION(BlueprintCallable, Category="Game Jolt|Data Store")
	void SetData(FOnDataStoreOpComplete OnComplete, const FString& Key, const FString& Value);

	UFUNCTION(BlueprintCallable, Category="Game Jolt|Data Store")
	void RemoveData(FOnDataStoreOpComplete OnComplete, const FString& Key);
	
	UFUNCTION(BlueprintCallable, Category="Game Jolt|Data Store")
	void FetchData(FOnDataStoreFetchComplete OnComplete, const FString& Key);

	UFUNCTION(BlueprintCallable, Category="Game Jolt|Data Store")
	void FetchKeys(FOnDataStoreKeysComplete OnComplete);

	//** User Data **//
	
	UFUNCTION(BlueprintCallable, Category="Game Jolt|Data Store|User")
	void SetUserData(FOnDataStoreOpComplete OnComplete, const FString& Username, const FString& UserToken, const FString& Key, const FString& Data);

	UFUNCTION(BlueprintCallable, Category="Game Jolt|Data Store|User")
	void FetchUserData(FOnDataStoreFetchComplete OnComplete, const FString& Username, const FString& UserToken, const FString& Key);

	UFUNCTION(BlueprintCallable, Category="Game Jolt|Data Store|User")
	void RemoveUserData(FOnDataStoreOpComplete OnComplete, const FString& Username, const FString& UserToken, const FString& Key);

	UFUNCTION(BlueprintCallable, Category="Game Jolt|Data Store|User")
	void FetchUserKeys(FOnDataStoreKeysComplete OnComplete, const FString& Username, const FString& UserToken);

private:
	TWeakObjectPtr<UGameJoltSubsystem> SubsystemPtr;
};
