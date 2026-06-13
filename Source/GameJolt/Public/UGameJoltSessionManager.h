#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UGameJoltTypes.h"
#include "UGameJoltSessionManager.generated.h"

class UGameJoltSubsystem;

UCLASS(BlueprintType)
class GAMEJOLT_API UGameJoltSessionManager : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UGameJoltSubsystem* InSubsystem);

	/**
	 * Opens a session for the currently logged-in user and starts automatic 30-second pings.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Sessions")
	void OpenSession(FOnSessionComplete OnComplete);

	/**
	 * Closes the active session and stops automatic pings.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Sessions")
	void CloseSession(FOnSessionComplete OnComplete);

	/**
	 * Manually pings the session. Not needed if you used OpenSession (auto-ping is handled).
	 * @param bIsActive  True = player is active; False = player is idle.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Sessions")
	void PingSession(bool bIsActive = true);

private:

	/*
	* To prevent overlapping requests which is a BluePrint issue.
	*/
	bool bOpenSessionInFlight = false;
	bool bCloseSessionInFlight = false;
	//bool bPingSessionInFlight = false; // Not because they are Fire and forget right now.

	TWeakObjectPtr<UGameJoltSubsystem> SubsystemPtr;

	FString CurrentUsername;
	FString CurrentUserToken;

	FTimerHandle PingTimerHandle;
};