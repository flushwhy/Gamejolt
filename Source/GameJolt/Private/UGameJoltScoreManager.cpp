#include "UGameJoltScoreManager.h"
#include "UGameJoltSubsystem.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpResponse.h"

void UGameJoltScoreManager::Initialize(UGameJoltSubsystem* InSubsystem)
{
	SubsystemPtr = InSubsystem;
}

void UGameJoltScoreManager::FetchScoreTables(FOnFetchScoreTablesComplete OnComplete)
{
	if (!SubsystemPtr.IsValid())
	{
		OnComplete.ExecuteIfBound(false, {});
		return;
	}
	if (bFetchingScoresTablesInFlight)
	{
		OnComplete.ExecuteIfBound(false, {});
		return;
	}
	bFetchingScoresTablesInFlight = true;

	SubsystemPtr->MakeApiRequest(TEXT("/scores/tables"), {}, FHttpRequestCompleteDelegate::CreateLambda(
		[OnComplete, Weakthis = TWeakObjectPtr<UGameJoltScoreManager>(this)](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (!Weakthis.IsValid()) return;
			TArray<FGameJoltScoreTable> ScoreTables;
			FString ErrorMessage;
			bool bSuccess = false;

			if (Weakthis->SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage))
			{
				const TSharedPtr<FJsonObject> JsonObject = Weakthis->SubsystemPtr->ParseResponse(Response);
				if (JsonObject.IsValid())
				{
					const TArray<TSharedPtr<FJsonValue>>* TablesJsonArray;
					if (JsonObject->TryGetArrayField(TEXT("tables"), TablesJsonArray))
					{
						bSuccess = FJsonObjectConverter::JsonArrayToUStruct(*TablesJsonArray, &ScoreTables, 0, 0);
					}
				}
			}
			Weakthis->bFetchingScoresTablesInFlight = false;
			OnComplete.ExecuteIfBound(bSuccess, ScoreTables);
		}));
}

void UGameJoltScoreManager::FetchScores(FOnFetchScoresComplete OnComplete, int32 TableID, int32 Limit, bool bForUserOnly, const FString& Username, const FString& UserToken)
{
	if (!SubsystemPtr.IsValid())
	{
		OnComplete.ExecuteIfBound(false, {});
		return;
	}
	if (bFetchingScoresInFlight)
	{
		OnComplete.ExecuteIfBound(false, {});
		return;
	}
	bFetchingScoresInFlight = true;

	TMap<FString, FString> Params;
	if (TableID != 0) Params.Add(TEXT("table_id"), FString::FromInt(TableID));
	if (Limit > 0) Params.Add(TEXT("limit"), FString::FromInt(Limit));
	if (bForUserOnly)
	{
		Params.Add(TEXT("username"), Username);
		Params.Add(TEXT("user_token"), UserToken);
	}

	SubsystemPtr->MakeApiRequest(TEXT("/scores"), Params, FHttpRequestCompleteDelegate::CreateLambda(
		[OnComplete, Weakthis = TWeakObjectPtr<UGameJoltScoreManager>(this)](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (!Weakthis.IsValid()) return;
			TArray<FGameJoltScore> Scores;
			FString ErrorMessage;
			bool bSuccess = false;

			if (Weakthis->SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage))
			{
				const TSharedPtr<FJsonObject> JsonObject = Weakthis->SubsystemPtr->ParseResponse(Response);
				if (JsonObject.IsValid())
				{
					const TArray<TSharedPtr<FJsonValue>>* ScoresJsonArray;
					if (JsonObject->TryGetArrayField(TEXT("scores"), ScoresJsonArray))
					{
						bSuccess = FJsonObjectConverter::JsonArrayToUStruct(*ScoresJsonArray, &Scores, 0, 0);
					}
				}
			}
			Weakthis->bFetchingScoresInFlight = false;
			OnComplete.ExecuteIfBound(bSuccess, Scores);
		}));
}

void UGameJoltScoreManager::AddScoreForGuest(FOnAddScoreComplete OnComplete, const FString& GuestName, const FString& ScoreString, int32 SortValue, int32 TableID, const FString& ExtraData)
{
	if (!SubsystemPtr.IsValid())
	{
		OnComplete.ExecuteIfBound(false, TEXT("Invalid Subsystem."));
		return;
	}

	if (bAddScoreForGuestInFlight)
	{
		OnComplete.ExecuteIfBound(false, TEXT("Another AddScoreForGuest request is already in flight."));
		return;
	}
	bAddScoreForGuestInFlight = true;

	TMap<FString, FString> Params;
	Params.Add(TEXT("guest"), GuestName);
	Params.Add(TEXT("score"), ScoreString);
	Params.Add(TEXT("sort"), FString::FromInt(SortValue));
	if (TableID != 0) Params.Add(TEXT("table_id"), FString::FromInt(TableID));
	if (!ExtraData.IsEmpty()) Params.Add(TEXT("extra_data"), ExtraData);

	SubsystemPtr->MakeApiRequest(TEXT("/scores/add"), Params, FHttpRequestCompleteDelegate::CreateLambda(
		[OnComplete, Weakthis = TWeakObjectPtr<UGameJoltScoreManager>(this)](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (!Weakthis.IsValid()) return;
			FString ErrorMessage;
			const bool bSuccess = Weakthis->SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage);
			Weakthis->bAddScoreForGuestInFlight = false;
			OnComplete.ExecuteIfBound(bSuccess, ErrorMessage);
		}));
}

void UGameJoltScoreManager::GetScoreRank(FOnGetRankComplete OnComplete, int32 SortValue, int32 TableID)
{
	if (!SubsystemPtr.IsValid())
	{
		OnComplete.ExecuteIfBound(false, -1);
		return;
	}
	if (bGetScoreRankInFlight)
	{
		OnComplete.ExecuteIfBound(false, -1);
		return;
	}
	bGetScoreRankInFlight = true;

	TMap<FString, FString> Params;
	Params.Add(TEXT("sort"), FString::FromInt(SortValue));
	if (TableID != 0) Params.Add(TEXT("table_id"), FString::FromInt(TableID));

	SubsystemPtr->MakeApiRequest(TEXT("/scores/get-rank"), Params, FHttpRequestCompleteDelegate::CreateLambda(
		[OnComplete, Weakthis = TWeakObjectPtr<UGameJoltScoreManager>(this)](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (!Weakthis.IsValid()) return;
			int32 Rank = -1;
			bool bSuccess = false;
			FString ErrorMessage;

			if (Weakthis->SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage))
			{
				const TSharedPtr<FJsonObject> JsonObject = Weakthis->SubsystemPtr->ParseResponse(Response);
				if (JsonObject.IsValid())
				{
					// FIX: delegate params were inverted � bSuccess first, Rank second.
					bSuccess = JsonObject->TryGetNumberField(TEXT("rank"), Rank);
				}
			}
			Weakthis->bGetScoreRankInFlight = false;
			OnComplete.ExecuteIfBound(bSuccess, Rank);
		}));
}

void UGameJoltScoreManager::AddScore(FOnDataStoreOpComplete OnComplete, int32 SortValue, FString ScoreText, int32 TableID)
{
	if (!SubsystemPtr.IsValid())
	{
		OnComplete.ExecuteIfBound(false, TEXT("Invalid Subsystem."));
		return;
	}
	if (bAddScoreInFlight)
	{
		OnComplete.ExecuteIfBound(false, TEXT("Another AddScore request is already in flight."));
		return;
	}
	bAddScoreInFlight = true;

	FString User, Token;
	SubsystemPtr->GetActiveUser(User, Token);

	if (User.IsEmpty())
	{
		OnComplete.ExecuteIfBound(false, TEXT("Error: No user authenticated. Call AuthenticateUser first."));
		return;
	}

	TMap<FString, FString> Params;
	Params.Add(TEXT("username"), User);
	Params.Add(TEXT("user_token"), Token);
	Params.Add(TEXT("score"), ScoreText);
	Params.Add(TEXT("sort"), FString::FromInt(SortValue));
	if (TableID != 0) Params.Add(TEXT("table_id"), FString::FromInt(TableID));

	SubsystemPtr->MakeApiRequest(TEXT("/scores/add"), Params, FHttpRequestCompleteDelegate::CreateLambda(
		[OnComplete, Weakthis = TWeakObjectPtr<UGameJoltScoreManager>(this)](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (!Weakthis.IsValid()) return;
			FString ErrorMessage;
			const bool bSuccess = Weakthis->SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage);
			Weakthis->bAddScoreInFlight = false;
			OnComplete.ExecuteIfBound(bSuccess, ErrorMessage);
		}));
}