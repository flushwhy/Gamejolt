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

	SubsystemPtr->MakeApiRequest(TEXT("/users/auth"), Params, FHttpRequestCompleteDelegate::CreateLambda(
		[OnComplete, Username, UserToken, Weakthis = TWeakObjectPtr<UGameJoltUserManager>(this)](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (!Weakthis.IsValid()) return;
			FGameJoltUser AuthenticatedUser;
			FString ErrorMessage;

			if (Weakthis->SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage))
			{
				const TSharedPtr<FJsonObject> JsonObject = Weakthis->SubsystemPtr->ParseResponse(Response);
				if (JsonObject.IsValid())
				{
					if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &AuthenticatedUser, 0, 0))
					{
						Weakthis->SubsystemPtr->SetActiveUser(Username, UserToken);
						OnComplete.ExecuteIfBound(true, AuthenticatedUser, TEXT(""));
						return;
					}
					ErrorMessage = TEXT("Failed to convert JSON response to User struct.");
				}
				else
				{
					ErrorMessage = TEXT("Failed to parse valid JSON from a successful response.");
				}
			}

			OnComplete.ExecuteIfBound(false, AuthenticatedUser, ErrorMessage);
		}));
}