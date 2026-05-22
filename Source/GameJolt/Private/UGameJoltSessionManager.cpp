#include "UGameJoltSessionManager.h"
#include "Engine/World.h"
#include "UGameJoltSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "TimerManager.h"

void UGameJoltSessionManager::Initialize(UGameJoltSubsystem* InSubsystem)
{
	SubsystemPtr = InSubsystem;
}

void UGameJoltSessionManager::OpenSession(FOnSessionComplete OnComplete)
{
	if (!SubsystemPtr.IsValid())
	{
		OnComplete.ExecuteIfBound(false, TEXT("Invalid Subsystem."));
		return;
	}

	FString User, Token;
	SubsystemPtr->GetActiveUser(User, Token);

	if (User.IsEmpty() || Token.IsEmpty())
	{
		OnComplete.ExecuteIfBound(false, TEXT("No authenticated user. Call AuthenticateUser first."));
		return;
	}

	// FIX: Was assigning undeclared 'Username'/'UserToken' variables instead of the
	// local 'User'/'Token' retrieved above.
	CurrentUsername = User;
	CurrentUserToken = Token;

	TMap<FString, FString> Params;
	Params.Add(TEXT("username"), User);
	Params.Add(TEXT("user_token"), Token);

	SubsystemPtr->MakeApiRequest(TEXT("/sessions/open"), Params, FHttpRequestCompleteDelegate::CreateLambda(
		[OnComplete, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			FString ErrorMessage;
			const bool bSuccess = SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage);

			if (bSuccess)
			{
				// Start automatic 30-second pings to keep the session alive.
				// Game Jolt closes sessions not pinged within 120 seconds.
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindLambda([this]()
					{
						PingSession();
					});
				GetWorld()->GetTimerManager().SetTimer(PingTimerHandle, TimerDelegate, 30.0f, true);
			}

			OnComplete.ExecuteIfBound(bSuccess, ErrorMessage);
		}));
}

void UGameJoltSessionManager::PingSession(bool bIsActive)
{
	if (!SubsystemPtr.IsValid() || CurrentUsername.IsEmpty())
	{
		return;
	}

	TMap<FString, FString> Params;
	Params.Add(TEXT("username"), CurrentUsername);
	Params.Add(TEXT("user_token"), CurrentUserToken);
	Params.Add(TEXT("status"), bIsActive ? TEXT("active") : TEXT("idle"));

	// Fire-and-forget — no callback needed for pings.
	SubsystemPtr->MakeApiRequest(TEXT("/sessions/ping"), Params, FHttpRequestCompleteDelegate());
}

void UGameJoltSessionManager::CloseSession(FOnSessionComplete OnComplete)
{
	if (!SubsystemPtr.IsValid() || !GetWorld())
	{
		OnComplete.ExecuteIfBound(false, TEXT("Invalid Subsystem or World."));
		return;
	}

	// Stop automatic pings immediately
	GetWorld()->GetTimerManager().ClearTimer(PingTimerHandle);

	TMap<FString, FString> Params;
	Params.Add(TEXT("username"), CurrentUsername);
	Params.Add(TEXT("user_token"), CurrentUserToken);

	SubsystemPtr->MakeApiRequest(TEXT("/sessions/close"), Params, FHttpRequestCompleteDelegate::CreateLambda(
		[OnComplete, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			FString ErrorMessage;
			const bool bSuccess = SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage);
			OnComplete.ExecuteIfBound(bSuccess, ErrorMessage);
		}));

	// Clear stored credentials regardless of server response
	CurrentUsername.Empty();
	CurrentUserToken.Empty();
}