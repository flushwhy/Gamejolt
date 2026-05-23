#include "UGameJoltTrophyManager.h"
#include "UGameJoltSubsystem.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpRequest.h"

void UGameJoltTrophyManager::Initialize(UGameJoltSubsystem* InSubsystem)
{
    SubsystemPtr = InSubsystem;
}

void UGameJoltTrophyManager::FetchTrophies(bool bAchieved, FOnFetchTrophiesComplete OnComplete)
{
    if (!SubsystemPtr.IsValid())
    {
        OnComplete.ExecuteIfBound(false, {}, TEXT("Invalid Subsystem."));
        return;
    }

    FString User, Token;
    SubsystemPtr->GetActiveUser(User, Token);

    if (User.IsEmpty())
    {
        OnComplete.ExecuteIfBound(false, {}, TEXT("No authenticated user. Call AuthenticateUser first."));
        return;
    }

    TMap<FString, FString> Params;
    Params.Add(TEXT("username"), User);
    Params.Add(TEXT("user_token"), Token);
    Params.Add(TEXT("achieved"), bAchieved ? TEXT("true") : TEXT("false"));

    SubsystemPtr->MakeApiRequest(TEXT("/trophies"), Params, FHttpRequestCompleteDelegate::CreateLambda(
        [OnComplete, Weakthis = TWeakObjectPtr<UGameJoltTrophyManager>(this)](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (!Weakthis.IsValid()) return;
            TArray<FGameJoltTrophy> FetchedTrophies;
            FString ErrorMessage;

            if (Weakthis->SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage))
            {
                const TSharedPtr<FJsonObject> JsonObject = Weakthis->SubsystemPtr->ParseResponse(Response);
                if (JsonObject.IsValid())
                {
                    const TArray<TSharedPtr<FJsonValue>>* TrophiesJsonArray;
                    if (JsonObject->TryGetArrayField(TEXT("trophies"), TrophiesJsonArray))
                    {
                        if (FJsonObjectConverter::JsonArrayToUStruct(*TrophiesJsonArray, &FetchedTrophies, 0, 0))
                        {
                            OnComplete.ExecuteIfBound(true, FetchedTrophies, TEXT(""));
                            return;
                        }
                        ErrorMessage = TEXT("Failed to convert JSON to trophies struct.");
                    }
                    else
                    {
                        ErrorMessage = TEXT("Missing 'trophies' array in JSON response.");
                    }
                }
                else
                {
                    ErrorMessage = TEXT("Failed to parse valid JSON from a successful response.");
                }
            }

            OnComplete.ExecuteIfBound(false, {}, ErrorMessage);
        }));
}