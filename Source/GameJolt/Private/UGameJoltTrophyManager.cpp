#include "UGameJoltTrophyManager.h"

#include "UGameJoltSubsystem.h"
#include "UGameJoltTrophyManager.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpRequest.h"

class UGameJoltSubsystem;

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

    TMap<FString, FString> Params;
    Params.Add(TEXT("achieved"), bAchieved ? TEXT("true") : TEXT("false"));

    // Call MakeApiRequest, ensuring the 'false' for a GET request is present
    SubsystemPtr->MakeApiRequest(TEXT("/trophies"), Params, FHttpRequestCompleteDelegate::CreateLambda(
        [OnComplete, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            TArray<FGameJoltTrophy> FetchedTrophies;
            FString ErrorMessage;

            if (SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage))
            {
                // FIX: Added a space between 'const' and 'TSharedPtr'
                const TSharedPtr<FJsonObject> JsonObject = SubsystemPtr->ParseResponse(Response);

                if (JsonObject.IsValid())
                {
                    // FIX: Added a space between 'const' and 'TArray'
                    const TArray<TSharedPtr<FJsonValue>>* TrophiesJsonArray;

                    if (JsonObject->TryGetArrayField(TEXT("trophies"), TrophiesJsonArray))
                    {
                        if (FJsonObjectConverter::JsonArrayToUStruct(*TrophiesJsonArray, &FetchedTrophies, 0, 0))
                        {
                            // Success! Everything worked.
                            OnComplete.ExecuteIfBound(true, FetchedTrophies, TEXT(""));
                            return; // Exit the lambda early on success
                        }
                        else
                        {
                            ErrorMessage = TEXT("Failed to convert JSON to trophies struct.");
                        }
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
			
            // If we reach here, it means something failed. The ErrorMessage will be set.
            OnComplete.ExecuteIfBound(false, {}, ErrorMessage);
			
        }), false);
}