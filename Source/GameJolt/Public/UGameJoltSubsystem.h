#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Dom/JsonObject.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UGameJoltSubsystem.generated.h"

class UGameJoltUserManager;
class UGameJoltSessionManager;
class UGameJoltTrophyManager;
class UGameJoltScoreManager;
class UGameJoltDataStoreManager;

/**
 * The main subsystem for interacting with the Game Jolt API.
 */
UCLASS(BlueprintType)
class GAMEJOLT_API UGameJoltSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Jolt|Managers")
	TObjectPtr<UGameJoltTrophyManager> TrophyManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Jolt|Managers")
	TObjectPtr<UGameJoltScoreManager> ScoreManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Jolt|Managers")
	TObjectPtr<UGameJoltDataStoreManager> DataStoreManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Jolt|Managers")
	TObjectPtr<UGameJoltUserManager> UserManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Jolt|Managers")
	TObjectPtr<UGameJoltSessionManager> SessionManager;

	/**
	 * Makes an API request to the Game Jolt server.
	 */
	void MakeApiRequest(const FString& Endpoint, const TMap<FString, FString>& Parameters, const FHttpRequestCompleteDelegate& OnComplete, bool bIsPostRequest);

	/**
	 * Parses the JSON response from the Game Jolt API.
	 */
	TSharedPtr<FJsonObject> ParseResponse(const FHttpResponsePtr& Response) const;

	/**
	 * Checks if the HTTP response was successful.
	 */
	bool IsResponseSuccessful(const FHttpResponsePtr& Response, bool bWasSuccessful, FString& OutErrorMessage) const;

	/**
	 * Gets the User Manager.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Subsystem")
	UGameJoltUserManager* GetUserManager() const { return UserManager; }

	/**
	 * Gets the Session Manager.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Subsystem")
	UGameJoltSessionManager* GetSessionManager() const { return SessionManager; }

private:
	FString GameID;
	FString PrivateKey;

	FString GenerateSignature(const FString& Url);
};