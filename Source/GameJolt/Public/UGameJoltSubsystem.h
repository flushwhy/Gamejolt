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
	 * Sets the active user for API requests. This should be called after a successful login.
	 */

	UFUNCTION(BlueprintCallable, Category = "Game Jolt")
	void SetActiveUser(const FString& InUsername, const FString& InUserToken)
	{
		CurrentUsername = InUsername;
		CurrentUserToken = InUserToken;
		bIsUserAuthenticated = !InUsername.IsEmpty() && !InUserToken.IsEmpty();
	}

	void GetActiveUser(FString& OutUsername, FString& OutUserToken) const
	{
		OutUsername = CurrentUsername;
		OutUserToken = CurrentUserToken;
	}

	bool IsUserAuthenticated() const
	{
		return bIsUserAuthenticated;
	}

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

	protected:
		FString CurrentUsername;
		FString CurrentUserToken;
		bool bIsUserAuthenticated = false;

private:
	FString GameID;
	FString PrivateKey;

	FString GenerateSignature(const FString& Url);
};