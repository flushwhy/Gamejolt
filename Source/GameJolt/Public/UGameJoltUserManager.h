#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UGameJoltTypes.h"
#include "UGameJoltUserManager.generated.h"

class UGameJoltSubsystem;

/**
 * Delegate for user authentication completion.
 * @param bSuccess - Whether the authentication was successful.
 * @param User - The authenticated user data.
 * @param ErrorMessage - Error message if authentication failed.
 */
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnAuthUserComplete, bool, bSuccess, const FGameJoltUser&, User, const FString&, Error