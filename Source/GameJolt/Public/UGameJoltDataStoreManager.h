#pragma once

#include "CoreMinimal.h"
#include "UGameJoltTypes.h"
#include "UGameJoltDataStoreManager.generated.h"

class UGameJoltSubsystem;

UCLASS(BlueprintType)
class GAMEJOLT_API UGameJoltDataStoreManager : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UGameJoltSubsystem* InSubsystem);

	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Data Store")
	void SetData(FOnDataStoreOpComplete OnComplete, const FString& Key, const FString& Data);

	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Data Store")
	void FetchData(FOnDataStoreFetchComplete OnComplete, const FString& Key);

	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Data Store")
	void RemoveData(FOnDataStoreOpComplete OnComplete, const FString& Key);

	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Data Store")
	void FetchKeys(FOnDataStoreKeysComplete OnComplete);

	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Data Store|User")
	void SetUserData(FOnDataStoreOpComplete OnComplete, const FString& Username, const FString& UserToken, const FString& Key, const FString& Data);

	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Data Store|User")
	void FetchUserData(FOnDataStoreFetchComplete OnComplete, const FString& Username, const FString& UserToken, const FString& Key);

	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Data Store|User")
	void RemoveUserData(FOnDataStoreOpComplete OnComplete, const FString& Username, const FString& UserToken, const FString& Key);

	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Data Store|User")
	void FetchUserKeys(FOnDataStoreKeysComplete OnComplete, const FString& Username, const FString& UserToken);

private:

	/*
	* To prevent overlapping requests which is a BluePrint issue.
	*/

	bool bSetDataInFlight		 = false;
	bool bFetchDataInFlight		 = false;
	bool bRemoveDataInFlight	 = false;
	bool bFetchKeysInFlight		 = false;
	bool bSetUserDataInFlight	 = false;
	bool bFetchUserDataInFlight	 = false;
	bool bRemoveUserDataInFlight = false;
	bool bFetchUserKeysInFlight	 = false;

	TWeakObjectPtr<UGameJoltSubsystem> SubsystemPtr;
};