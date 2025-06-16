#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Dom/JsonObject.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Dom/JsonObject.h"
#include "UGameJoltSubsystem.generated.h"

class UGameJoltUserManager;
class UGameJoltSessionManager;
class UGameJoltTrophyManager;
class UGameJoltScoreManager;
class UGameJoltDataStoreManager;

UCLASS(BlueprintType)
class GAMEJOLT_API UGameJoltSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
    
public:
	
   	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Game Jolt|Managers")
	TObjectPtr<UGameJoltTrophyManager> TrophyManager;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Game Jolt|Managers")
	TObjectPtr<UGameJoltScoreManager> ScoreManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Game Jolt|Managers")
	TObjectPtr<UGameJoltDataStoreManager> DataStoreManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Game Jolt|Managers")
	TObjectPtr<UGameJoltUserManager> UserManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Game Jolt|Managers")
	TObjectPtr<UGameJoltSessionManager> SessionManager;
	
	
	void MakeApiRequest(const FString& Endpoint, const TMap<FString, FString>& Parameters, const FHttpRequestCompleteDelegate& OnComplete, bool bIsPostRequest);

	TSharedPtr<FJsonObject> ParseResponse(const FHttpResponsePtr& Response) const;
	bool IsResponseSuccessful(const FHttpResponsePtr& Response, bool bWasSuccessful, FString& OutErrorMessage) const;

	
private:
	FString GameID;
	FString PrivateKey;

	FString GenerateSignature(const FString &Url);
};