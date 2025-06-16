#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UGameJoltSettings.generated.h"

/**
 * Creates a settings panel in Project Settings > Plugins > Game Jolt API
 * to hold the API credentials for the game.
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Game Jolt API"))
class UGameJoltSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** Your Game's ID, found on your Game Jolt dashboard. */
	UPROPERTY(Config, EditAnywhere, Category = "API Credentials")
	FString GameID;

	/** Your Game's Private Key, found on your Game Jolt dashboard. */
	UPROPERTY(Config, EditAnywhere, Category = "API Credentials", meta = (PasswordField = true))
	FString PrivateKey;
};