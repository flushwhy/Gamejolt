#include "UGameJoltUserManager.h"
#include "UGameJoltSubsystem.h"
#include "UGameJoltTypes.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"

void UGameJoltUserManager::Initialize(UGameJoltSubsystem* InSubsystem)
{
	SubsystemPtr = InSubsystem;
}

void UGameJoltUserManager::AuthenticateUser(FOnAuthUserComplete OnComplete, const FString& Username, const FString& UserToken)
{
	if (!SubsystemPtr.IsValid())
	{
		OnComplete.ExecuteIfBound(false, FGameJoltUser(), TEXT("Invalid Subsystem."));
		return;
	}

	TMap<FString, FString> Params;
	Params.Add(TEXT("username"), Username);
	Params.Add(TEXT("user_token"), UserToken);

	// This is a GET request, so the last parameter is 'false'.
	SubsystemPtr->MakeApiRequest(TEXT("/users/auth"), Params, FHttpRequestCompleteDelegate::CreateLambda(
		[OnComplete, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			FGameJoltUser AuthenticatedUser;
			FString ErrorMessage;

			if (SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage))
			{
				const TSharedPtr<FJsonObject> JsonObject = SubsystemPtr->ParseResponse(Response);
				if (JsonObject.IsValid())
				{
					// This is where the magic happens! Convert the JSON object directly to our struct.
					if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &AuthenticatedUser, 0, 0))
					{
						// Success!
						OnComplete.ExecuteIfBound(true, AuthenticatedUser, TEXT(""));
						return;
					}
					else
					{
						ErrorMessage = TEXT("Failed to convert JSON response to User struct.");
					}
				}
				else
				{
					ErrorMessage = TEXT("Failed to parse valid JSON from a successful response.");
				}
			}

			// If we reach here, something failed. ErrorMessage will be set.
			OnComplete.ExecuteIfBound(false, AuthenticatedUser, ErrorMessage);

		}), false);
}