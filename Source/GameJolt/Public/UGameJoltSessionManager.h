#pragma once

#include "CoreMinimal.h"
#include "Engine/TimerHandle.h"
#include "UGameJoltSessionManager.generated.h"

class UGameJoltSubsystem;

// A generic delegate for session-related API calls.
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnSessionComplete, bool, bSuccess, const FString&, ErrorMessage);

/**
 * Manages opening, pinging, and closing a game session for an authenticated user.
 */
UCLASS(BlueprintType)
class GAMEJOLT_API UGameJoltSessionManager : public UObject
{
	GENERATED_BODY()

public:
	/** Links this manager to the main subsystem. */
	void Initialize(UGameJoltSubsystem* InSubsystem);

	/**
	 * Opens a new game session for the user. This should be called after a user is successfully authenticated.
	 * On success, it will automatically start pinging the session every 30 seconds.
	 * @param OnComplete Delegate to call with the result.
	 * @param Username The authenticated user's username.
	 * @param UserToken The authenticated user's token.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Session")
	void OpenSession(FOnSessionComplete OnComplete, const FString& Username, const FString& UserToken);

	/**
	 * Pings the current session to keep it active. This is typically called automatically.
	 * @param bIsActive Pings with an 'active' status. Set to false for 'idle'.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Session")
	void PingSession(bool bIsActive = true);

	/**
	 * Closes the current game session. This should be called when the player logs out or quits the game.
	 * This will stop the automatic session pings.
	 * @param OnComplete Delegate to call with the result.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Session")
	void CloseSession(FOnSessionComplete OnComplete);

private:
	TWeakObjectPtr<UGameJoltSubsystem> SubsystemPtr;

	// Timer for automatically pinging the session
	FTimerHandle PingTimerHandle;

	// Store credentials for automatic pinging
	FString CurrentUsername;
	FString CurrentUserToken;
};
