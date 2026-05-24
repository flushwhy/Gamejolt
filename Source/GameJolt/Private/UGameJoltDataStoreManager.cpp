#include "UGameJoltDataStoreManager.h"
#include "JsonObjectConverter.h"
#include "UGameJoltSubsystem.h"
#include "Interfaces/IHttpRequest.h"

void UGameJoltDataStoreManager::Initialize(UGameJoltSubsystem* InSubsystem)
{
    SubsystemPtr = InSubsystem;
}

void UGameJoltDataStoreManager::SetData(FOnDataStoreOpComplete OnComplete, const FString& Key, const FString& Data)
{
    if (!SubsystemPtr.IsValid())
    {
        OnComplete.ExecuteIfBound(false, TEXT("Invalid Subsystem."));
        return;
    }

    if (bSetDataInFlight)
    {
        OnComplete.ExecuteIfBound(false, {});
        return;
    }
    bSetDataInFlight = true;


    TMap<FString, FString> Params;
    Params.Add(TEXT("key"), Key);
    Params.Add(TEXT("data"), Data);

    SubsystemPtr->MakeApiRequest(TEXT("/data-store/set"), Params, FHttpRequestCompleteDelegate::CreateLambda(
        [OnComplete, Weakthis = TWeakObjectPtr<UGameJoltDataStoreManager>(this)](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (!Weakthis.IsValid()) return;

            FString ErrorMessage;
            const bool bSuccess = Weakthis->SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage);
            Weakthis->bSetDataInFlight = false;
            OnComplete.ExecuteIfBound(bSuccess, ErrorMessage);
        }));
}

void UGameJoltDataStoreManager::FetchData(FOnDataStoreFetchComplete OnComplete, const FString& Key)
{
    if (!SubsystemPtr.IsValid())
    {
        OnComplete.ExecuteIfBound(false, TEXT(""), TEXT("Invalid Subsystem."));
        return;
    }

    if (bFetchDataInFlight)
    {
        OnComplete.ExecuteIfBound(false, {}, TEXT("Request already in flight."));
        return;
	}
	bFetchDataInFlight = true;

    TMap<FString, FString> Params;
    Params.Add(TEXT("key"), Key);

    SubsystemPtr->MakeApiRequest(TEXT("/data-store"), Params, FHttpRequestCompleteDelegate::CreateLambda(
        [OnComplete, Weakthis = TWeakObjectPtr<UGameJoltDataStoreManager>(this)](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (!Weakthis.IsValid()) return;
            FString ErrorMessage, FetchedData;
            if (Weakthis->SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage))
            {
                const TSharedPtr<FJsonObject> JsonObject = Weakthis->SubsystemPtr->ParseResponse(Response);
                if (JsonObject.IsValid() && JsonObject->TryGetStringField(TEXT("data"), FetchedData))
                {
                    OnComplete.ExecuteIfBound(true, FetchedData, TEXT(""));
                    return;
                }
                ErrorMessage = TEXT("Failed to extract data from response.");
            }
			Weakthis->bFetchDataInFlight = false;
            OnComplete.ExecuteIfBound(false, TEXT(""), ErrorMessage);
        }));
}

void UGameJoltDataStoreManager::RemoveData(FOnDataStoreOpComplete OnComplete, const FString& Key)
{
    if (!SubsystemPtr.IsValid())
    {
        OnComplete.ExecuteIfBound(false, TEXT("Invalid Subsystem."));
        return;
    }
    if (bRemoveDataInFlight)
    {
        OnComplete.ExecuteIfBound(false, {});
        return;
	}
	bRemoveDataInFlight = true;

    TMap<FString, FString> Params;
    Params.Add(TEXT("key"), Key);

    SubsystemPtr->MakeApiRequest(TEXT("/data-store/remove"), Params, FHttpRequestCompleteDelegate::CreateLambda(
        [OnComplete, Weakthis = TWeakObjectPtr<UGameJoltDataStoreManager>(this)](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (!Weakthis.IsValid()) return;
            FString ErrorMessage;
            const bool bSuccess = Weakthis->SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage);
			Weakthis->bRemoveDataInFlight = false;
            OnComplete.ExecuteIfBound(bSuccess, ErrorMessage);
        }));
}

void UGameJoltDataStoreManager::FetchKeys(FOnDataStoreKeysComplete OnComplete)
{
    if (!SubsystemPtr.IsValid())
    {
        OnComplete.ExecuteIfBound(false, {}, TEXT("Invalid Subsystem."));
        return;
    }
    if (bFetchKeysInFlight)
    {
        OnComplete.ExecuteIfBound(false, {}, TEXT("Request already in flight."));
        return;
    }
	bFetchKeysInFlight = true;

    SubsystemPtr->MakeApiRequest(TEXT("/data-store/get-keys"), {}, FHttpRequestCompleteDelegate::CreateLambda(
        [OnComplete, Weakthis = TWeakObjectPtr<UGameJoltDataStoreManager>(this)](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (!Weakthis.IsValid()) return;
            TArray<FGameJoltDataKey> FetchedKeys;
            FString ErrorMessage;
            if (Weakthis->SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage))
            {
                const TSharedPtr<FJsonObject> JsonObject = Weakthis->SubsystemPtr->ParseResponse(Response);
                if (JsonObject.IsValid())
                {
                    const TArray<TSharedPtr<FJsonValue>>* KeysJsonArray;
                    if (JsonObject->TryGetArrayField(TEXT("keys"), KeysJsonArray))
                    {
                        if (FJsonObjectConverter::JsonArrayToUStruct(*KeysJsonArray, &FetchedKeys, 0, 0))
                        {
                            OnComplete.ExecuteIfBound(true, FetchedKeys, TEXT(""));
                            return;
                        }
                        ErrorMessage = TEXT("Failed to process keys data.");
                    }
                    else
                    {
                        ErrorMessage = TEXT("No keys found in response.");
                    }
                }
            }
			Weakthis->bFetchKeysInFlight = false;
            OnComplete.ExecuteIfBound(false, {}, ErrorMessage);
        }));
}

void UGameJoltDataStoreManager::SetUserData(FOnDataStoreOpComplete OnComplete, const FString& Username, const FString& UserToken, const FString& Key, const FString& Data)
{
    if (!SubsystemPtr.IsValid())
    {
        OnComplete.ExecuteIfBound(false, TEXT("Invalid Subsystem."));
        return;
    }
    if (bSetUserDataInFlight)
    {
        OnComplete.ExecuteIfBound(false, {});
        return;
	}
	bSetUserDataInFlight = true;

    TMap<FString, FString> Params;
    Params.Add(TEXT("username"), Username);
    Params.Add(TEXT("user_token"), UserToken);
    Params.Add(TEXT("key"), Key);
    Params.Add(TEXT("data"), Data);

    SubsystemPtr->MakeApiRequest(TEXT("/data-store/set"), Params, FHttpRequestCompleteDelegate::CreateLambda(
        [OnComplete, Weakthis = TWeakObjectPtr<UGameJoltDataStoreManager>(this)](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (!Weakthis.IsValid()) return;
            FString ErrorMessage;
            const bool bSuccess = Weakthis->SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage);
			Weakthis->bSetUserDataInFlight = false;
            OnComplete.ExecuteIfBound(bSuccess, ErrorMessage);
        }));
}

void UGameJoltDataStoreManager::FetchUserData(FOnDataStoreFetchComplete OnComplete, const FString& Username, const FString& UserToken, const FString& Key)
{
    if (!SubsystemPtr.IsValid())
    {
        OnComplete.ExecuteIfBound(false, TEXT(""), TEXT("Invalid Subsystem."));
        return;
    }
    if (bFetchUserDataInFlight)
    {
        OnComplete.ExecuteIfBound(false, {}, TEXT("Request already in flight."));
        return;
    }
	bFetchUserDataInFlight = true;

    TMap<FString, FString> Params;
    Params.Add(TEXT("username"), Username);
    Params.Add(TEXT("user_token"), UserToken);
    Params.Add(TEXT("key"), Key);

    SubsystemPtr->MakeApiRequest(TEXT("/data-store"), Params, FHttpRequestCompleteDelegate::CreateLambda(
        [OnComplete, Weakthis = TWeakObjectPtr<UGameJoltDataStoreManager>(this)](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if(!Weakthis.IsValid()) return;
            FString ErrorMessage, FetchedData;
            if (Weakthis->SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage))
            {
                const TSharedPtr<FJsonObject> JsonObject = Weakthis->SubsystemPtr->ParseResponse(Response);
                if (JsonObject.IsValid() && JsonObject->TryGetStringField(TEXT("data"), FetchedData))
                {
                    OnComplete.ExecuteIfBound(true, FetchedData, TEXT(""));
                    return;
                }
                ErrorMessage = TEXT("Failed to extract data from response.");
            }
			Weakthis->bFetchUserDataInFlight = false;
            OnComplete.ExecuteIfBound(false, TEXT(""), ErrorMessage);
        }));
}

void UGameJoltDataStoreManager::RemoveUserData(FOnDataStoreOpComplete OnComplete, const FString& Username, const FString& UserToken, const FString& Key)
{
    if (!SubsystemPtr.IsValid())
    {
        OnComplete.ExecuteIfBound(false, TEXT("Invalid Subsystem."));
        return;
    }
    if (bRemoveUserDataInFlight)
    {
        OnComplete.ExecuteIfBound(false, {});
        return;
	}
	bRemoveUserDataInFlight = true;

    TMap<FString, FString> Params;
    Params.Add(TEXT("username"), Username);
    Params.Add(TEXT("user_token"), UserToken);
    Params.Add(TEXT("key"), Key);

    SubsystemPtr->MakeApiRequest(TEXT("/data-store/remove"), Params, FHttpRequestCompleteDelegate::CreateLambda(
        [OnComplete, Weakthis = TWeakObjectPtr<UGameJoltDataStoreManager>(this)](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (!Weakthis.IsValid()) return;
            FString ErrorMessage;
            const bool bSuccess = Weakthis->SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage);
			Weakthis->bRemoveUserDataInFlight = false;
            OnComplete.ExecuteIfBound(bSuccess, ErrorMessage);
        }));
}

void UGameJoltDataStoreManager::FetchUserKeys(FOnDataStoreKeysComplete OnComplete, const FString& Username, const FString& UserToken)
{
    if (!SubsystemPtr.IsValid())
    {
        OnComplete.ExecuteIfBound(false, {}, TEXT("Invalid Subsystem."));
        return;
    }
    if (bFetchUserKeysInFlight)
    {
        OnComplete.ExecuteIfBound(false, {}, TEXT("Request already in flight."));
        return;
	}
	bFetchUserKeysInFlight = true;

    TMap<FString, FString> Params;
    Params.Add(TEXT("username"), Username);
    Params.Add(TEXT("user_token"), UserToken);

    SubsystemPtr->MakeApiRequest(TEXT("/data-store/get-keys"), Params, FHttpRequestCompleteDelegate::CreateLambda(
        [OnComplete, Weakthis = TWeakObjectPtr<UGameJoltDataStoreManager>(this)](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (!Weakthis.IsValid()) return;
            TArray<FGameJoltDataKey> FetchedKeys;
            FString ErrorMessage;
            if (Weakthis->SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage))
            {
                const TSharedPtr<FJsonObject> JsonObject = Weakthis->SubsystemPtr->ParseResponse(Response);
                if (JsonObject.IsValid())
                {
                    const TArray<TSharedPtr<FJsonValue>>* KeysJsonArray;
                    if (JsonObject->TryGetArrayField(TEXT("keys"), KeysJsonArray))
                    {
                        if (FJsonObjectConverter::JsonArrayToUStruct(*KeysJsonArray, &FetchedKeys, 0, 0))
                        {
                            OnComplete.ExecuteIfBound(true, FetchedKeys, TEXT(""));
                            return;
                        }
                        ErrorMessage = TEXT("Failed to process keys data.");
                    }
                    else
                    {
                        ErrorMessage = TEXT("No keys found in response.");
                    }
                }
            }
			Weakthis->bFetchUserKeysInFlight = false;
            OnComplete.ExecuteIfBound(false, {}, ErrorMessage);
        }));
}