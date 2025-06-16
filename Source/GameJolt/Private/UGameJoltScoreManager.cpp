
#include "UGameJoltScoreManager.h"

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

	SubsystemPtr->MakeApiRequest(TEXT("/scores/tables"), {}, FHttpRequestCompleteDelegate::CreateLambda(
		[OnComplete, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			TArray<FGameJoltScoreTable> ScoreTables;
			bool bSuccess = false;

			if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
			{
				// Refactored to use the helper function for consistency.
				const TSharedPtr<FJsonObject> JsonObject = SubsystemPtr->ParseResponse(Response);
				if (JsonObject.IsValid())
				{
					const TArray<TSharedPtr<FJsonValue>>* TablesJsonArray;
					if (JsonObject->TryGetArrayField(TEXT("tables"), TablesJsonArray))
					{
						bSuccess = FJsonObjectConverter::JsonArrayToUStruct(*TablesJsonArray, &ScoreTables, 0, 0);
					}
				}
			}
			OnComplete.ExecuteIfBound(bSuccess, ScoreTables);
		}), false);
}

void UGameJoltScoreManager::FetchScores(FOnFetchScoresComplete OnComplete, int32 TableID, int32 Limit, bool bForUserOnly, const FString& Username, const FString& UserToken)
{
	if (!SubsystemPtr.IsValid())
	{
		OnComplete.ExecuteIfBound(false, {});
		return;
	}

	TMap<FString, FString> Params;
	if (TableID != 0) Params.Add(TEXT("table_id"), FString::FromInt(TableID));
	if (Limit > 0) Params.Add(TEXT("limit"), FString::FromInt(Limit));
	if (bForUserOnly)
	{
		Params.Add(TEXT("username"), Username);
		Params.Add(TEXT("user_token"), UserToken);
	}

	SubsystemPtr->MakeApiRequest(TEXT("/scores"), Params, FHttpRequestCompleteDelegate::CreateLambda(
		[OnComplete, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			TArray<FGameJoltScore> Scores;
			bool bSuccess = false;

			if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
			{
				// FIX 1: Corrected typo from ParseParms to ParseResponse.
				const TSharedPtr<FJsonObject> JsonObject = SubsystemPtr->ParseResponse(Response);
				if (JsonObject.IsValid())
				{
					const TArray<TSharedPtr<FJsonValue>>* ScoresJsonArray;
					if (JsonObject->TryGetArrayField(TEXT("scores"), ScoresJsonArray))
					{
						bSuccess = FJsonObjectConverter::JsonArrayToUStruct(*ScoresJsonArray, &Scores, 0, 0);
					}
				}
			}
			OnComplete.ExecuteIfBound(bSuccess, Scores);
		}), false);
}

void UGameJoltScoreManager::AddScoreForGuest(FOnAddScoreComplete OnComplete, const FString& GuestName, const FString& ScoreString, int32 SortValue, int32 TableID, const FString& ExtraData)
{
	if (!SubsystemPtr.IsValid())
	{
		OnComplete.ExecuteIfBound(false, TEXT("Invalid Subsystem."));
		return;
	}

	TMap<FString, FString> Params;
	Params.Add(TEXT("guest"), GuestName);
	Params.Add(TEXT("score"), ScoreString);
	Params.Add(TEXT("sort"), FString::FromInt(SortValue));
	if (TableID != 0) Params.Add(TEXT("table_id"), FString::FromInt(TableID));
	if (!ExtraData.IsEmpty()) Params.Add(TEXT("extra_data"), ExtraData);

	// The 'true' at the end specifies this is a POST request
	SubsystemPtr->MakeApiRequest(TEXT("/scores/add"), Params, FHttpRequestCompleteDelegate::CreateLambda(
		[OnComplete, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			FString ErrorMessage;
			const bool bSuccess = SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage);
			OnComplete.ExecuteIfBound(bSuccess, ErrorMessage);
		}), true);
}


void UGameJoltScoreManager::GetScoreRank(FOnGetRankComplete OnComplete, int32 SortValue, int32 TableID)
{
	if (!SubsystemPtr.IsValid())
	{
		OnComplete.ExecuteIfBound(false, false);
		return;
	}

	TMap<FString, FString> Params;
	Params.Add(TEXT("sort"), FString::FromInt(SortValue));
	if (TableID != 0) Params.Add(TEXT("table_id"), FString::FromInt(TableID));

	SubsystemPtr->MakeApiRequest(TEXT("/scores/get-rank"), Params, FHttpRequestCompleteDelegate::CreateLambda(
		[OnComplete, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			int32 Rank = -1;
			bool bSuccess = false;
			
			if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
			{
				const TSharedPtr<FJsonObject> JsonObject = SubsystemPtr->ParseResponse(Response);
				if (JsonObject.IsValid())
				{
					bSuccess = JsonObject->TryGetNumberField(TEXT("rank"), Rank);
				}
			}
			OnComplete.ExecuteIfBound(bool(Rank), bSuccess);
		}), false);
}