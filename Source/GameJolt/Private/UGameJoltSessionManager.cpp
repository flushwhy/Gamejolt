#include "UGameJoltSessionManager.h"
#include "Engine/World.h"
#include "UGameJoltSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "TimerManager.h"

void UGameJoltSessionManager::Initialize(UGameJoltSubsystem* InSubsystem)
{
	SubsystemPtr = InSubsystem;
}

void UGameJoltSessionManager::OpenSession(FOnSessionComplete OnComplete, const FString& Username, const FString& UserToken)
{
	if (!SubsystemPtr.IsValid() || !GetWorld())
	{
		OnComplete.ExecuteIfBound(false, TEXT("Invalid Subsystem or World."));
		return;
	}

	// Store credentials for automatic pings
	CurrentUsername = Username;
	CurrentUserToken = UserToken;

	TMap<FString, FString> Params;
	Params.Add(TEXT("username"), Username);
	Params.Add(TEXT("user_token"), UserToken);

	// This is a POST request, so the last parameter is 'true'.
	SubsystemPtr->MakeApiRequest(TEXT("/sessions/open"), Params, FHttpRequestCompleteDelegate::CreateLambda(
		[OnComplete, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			FString ErrorMessage;
			const bool bSuccess = SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage);

			if (bSuccess)
			{

				FTimerDelegate TimerDelegate;
				TimerDelegate.BindLambda([this]()
				{
					PingSession(); // Call our function with its default parameter (true)
				});

				GetWorld()->GetTimerManager().SetTimer(PingTimerHandle, TimerDelegate, 30.0f, true);
			}
			
			OnComplete.ExecuteIfBound(bSuccess, ErrorMessage);
		}), true);
}

void UGameJoltSessionManager::PingSession(bool bIsActive)
{
	if (!SubsystemPtr.IsValid() || CurrentUsername.IsEmpty())
	{
		// Don't ping if we don't have a valid user
		return;
	}

	TMap<FString, FString> Params;
	Params.Add(TEXT("username"), CurrentUsername);
	Params.Add(TEXT("user_token"), CurrentUserToken);
	Params.Add(TEXT("status"), bIsActive ? TEXT("active") : TEXT("idle"));

	// Ping is a fire-and-forget POST request. We don't need a callback.
	SubsystemPtr->MakeApiRequest(TEXT("/sessions/ping"), Params, FHttpRequestCompleteDelegate(), true);
}

void UGameJoltSessionManager::CloseSession(FOnSessionComplete OnComplete)
{
	if (!SubsystemPtr.IsValid() || !GetWorld())
	{
		OnComplete.ExecuteIfBound(false, TEXT("Invalid Subsystem or World."));
		return;
	}

	// Stop the automatic pings immediately
	GetWorld()->GetTimerManager().ClearTimer(PingTimerHandle);

	TMap<FString, FString> Params;
	Params.Add(TEXT("username"), CurrentUsername);
	Params.Add(TEXT("user_token"), CurrentUserToken);

	// This is a POST request.
	SubsystemPtr->MakeApiRequest(TEXT("/sessions/close"), Params, FHttpRequestCompleteDelegate::CreateLambda(
		[OnComplete, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			FString ErrorMessage;
			const bool bSuccess = SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage);
			OnComplete.ExecuteIfBound(bSuccess, ErrorMessage);
		}), true);

	// Clear the stored credentials
	CurrentUsername.Empty();
	CurrentUserToken.Empty();
}