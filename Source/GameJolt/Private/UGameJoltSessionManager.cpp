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
	if (bOpenSessionInFlight)
	{

		OnComplete.ExecuteIfBound(false, TEXT("OpenSession already in progress."));
		return;
	}
	FString User, Token;
	SubsystemPtr->GetActiveUser(User, Token);

	if (User.IsEmpty() || Token.IsEmpty())
	{
		OnComplete.ExecuteIfBound(false, TEXT("No authenticated user. Call AuthenticateUser first."));
		return;
	}

	bOpenSessionInFlight = true;
	CurrentUsername = User;
	CurrentUserToken = Token;

	TMap<FString, FString> Params;
	Params.Add(TEXT("username"), User);
	Params.Add(TEXT("user_token"), Token);

	SubsystemPtr->MakeApiRequest(TEXT("/sessions/open"), Params, FHttpRequestCompleteDelegate::CreateLambda(
		[OnComplete, Weakthis = TWeakObjectPtr<UGameJoltSessionManager>(this)](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (!Weakthis.IsValid() || !Weakthis->SubsystemPtr.IsValid()) return;
			FString ErrorMessage;
			const bool bSuccess = Weakthis->SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage);

			if (bSuccess)
			{
				// Start automatic 30-second pings to keep the session alive.
				// Game Jolt closes sessions not pinged within 120 seconds.
				if (UWorld* World = Weakthis->GetWorld())
				{
					FTimerDelegate TimerDelegate;
					TimerDelegate.BindLambda([Weakthis]()
					{
						if (Weakthis.IsValid()) Weakthis->PingSession();
					});
					World->GetTimerManager().SetTimer(Weakthis->PingTimerHandle, TimerDelegate, 30.0f, true);
				}
				Weakthis->ConsecutivePingFailures = 0;
			}

			Weakthis->bOpenSessionInFlight = false;
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

	// Fire-and-forget ping; count consecutive transport or API failures.
	SubsystemPtr->MakeApiRequest(TEXT("/sessions/ping"), Params, FHttpRequestCompleteDelegate::CreateLambda(
		[Weakthis = TWeakObjectPtr<UGameJoltSessionManager>(this)](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (!Weakthis.IsValid() || !Weakthis->SubsystemPtr.IsValid()) return;

			FString ErrorMessage;
			if (!Weakthis->SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage))
			{
				++Weakthis->ConsecutivePingFailures;
				if (Weakthis->ConsecutivePingFailures >= 3)
				{
					if (UWorld* World = Weakthis->GetWorld())
					{
						World->GetTimerManager().ClearTimer(Weakthis->PingTimerHandle);
					}
					UE_LOG(LogTemp, Warning, TEXT("GameJolt session ping failed 3 consecutive times. Stopping further pings."));
				}
			}
			else
			{
				Weakthis->ConsecutivePingFailures = 0;
			}
		}));
}

void UGameJoltSessionManager::CloseSession(FOnSessionComplete OnComplete)
{
	if (!SubsystemPtr.IsValid() || !GetWorld())
	{
		OnComplete.ExecuteIfBound(false, TEXT("Invalid Subsystem or World."));
		return;
	}

	if (bCloseSessionInFlight)
	{
		OnComplete.ExecuteIfBound(false, TEXT("CloseSession already in progress."));
		return;
	}
	bCloseSessionInFlight = true;

	// Stop automatic pings immediately
	GetWorld()->GetTimerManager().ClearTimer(PingTimerHandle);
	ConsecutivePingFailures = 0;

	TMap<FString, FString> Params;
	Params.Add(TEXT("username"), CurrentUsername);
	Params.Add(TEXT("user_token"), CurrentUserToken);

	SubsystemPtr->MakeApiRequest(TEXT("/sessions/close"), Params, FHttpRequestCompleteDelegate::CreateLambda(
		[OnComplete, Weakthis = TWeakObjectPtr<UGameJoltSessionManager>(this)](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (!Weakthis.IsValid()) return;
			FString ErrorMessage;
			const bool bSuccess = Weakthis->SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage);
		
			Weakthis->bCloseSessionInFlight = false;
			OnComplete.ExecuteIfBound(bSuccess, ErrorMessage);
		}));

	// Clear stored credentials regardless of server response
	CurrentUsername.Empty();
	CurrentUserToken.Empty();
}