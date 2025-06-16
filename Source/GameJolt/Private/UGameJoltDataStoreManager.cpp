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

    TMap<FString, FString> Params;
    Params.Add(TEXT("key"), Key);
    Params.Add(TEXT("data"), Data);

    SubsystemPtr->MakeApiRequest(TEXT("/data-store/set"), Params, FHttpRequestCompleteDelegate::CreateLambda(
        [OnComplete, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            FString ErrorMessage;
            const bool bSuccess = SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage);
            OnComplete.ExecuteIfBound(bSuccess, ErrorMessage);
        }), true);
}

void UGameJoltDataStoreManager::FetchData(FOnDataStoreFetchComplete OnComplete, const FString& Key)
{
    if (!SubsystemPtr.IsValid())
    {
        OnComplete.ExecuteIfBound(false, TEXT(""), TEXT("Invalid Subsystem."));
        return;
    }

    TMap<FString, FString> Params;
    Params.Add(TEXT("key"), Key);

    SubsystemPtr->MakeApiRequest(TEXT("/data-store"), Params, FHttpRequestCompleteDelegate::CreateLambda(
        [OnComplete, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            FString ErrorMessage, FetchedData;
            if (SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage))
            {
                const TSharedPtr<FJsonObject> JsonObject = SubsystemPtr->ParseResponse(Response);
                if (JsonObject.IsValid() && JsonObject->TryGetStringField(TEXT("data"), FetchedData))
                {
                    OnComplete.ExecuteIfBound(true, FetchedData, TEXT(""));
                    return;
                }
                ErrorMessage = TEXT("Failed to extract data from response.");
            }
            OnComplete.ExecuteIfBound(false, TEXT(""), ErrorMessage);
        }), false);
}

void UGameJoltDataStoreManager::RemoveData(FOnDataStoreOpComplete OnComplete, const FString& Key)
{
    if (!SubsystemPtr.IsValid())
    {
        OnComplete.ExecuteIfBound(false, TEXT("Invalid Subsystem."));
        return;
    }

    TMap<FString, FString> Params;
    Params.Add(TEXT("key"), Key);

    SubsystemPtr->MakeApiRequest(TEXT("/data-store/remove"), Params, FHttpRequestCompleteDelegate::CreateLambda(
        [OnComplete, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            FString ErrorMessage;
            const bool bSuccess = SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage);
            OnComplete.ExecuteIfBound(bSuccess, ErrorMessage);
        }), true);
}

void UGameJoltDataStoreManager::FetchKeys(FOnDataStoreKeysComplete OnComplete)
{
    if (!SubsystemPtr.IsValid())
    {
        OnComplete.ExecuteIfBound(false, {}, TEXT("Invalid Subsystem."));
        return;
    }

    SubsystemPtr->MakeApiRequest(TEXT("/data-store/get-keys"), {}, FHttpRequestCompleteDelegate::CreateLambda(
        [OnComplete, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            TArray<FGameJoltDataKey> FetchedKeys;
            FString ErrorMessage;
            if (SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage))
            {
                const TSharedPtr<FJsonObject> JsonObject = SubsystemPtr->ParseResponse(Response);
                if (JsonObject.IsValid())
                {
                    const TArray<TSharedPtr<FJsonValue>>* KeysJsonArray;
                    if (JsonObject->TryGetArrayField(TEXT("keys"), KeysJsonArray))
                    {
                        if(FJsonObjectConverter::JsonArrayToUStruct(*KeysJsonArray, &FetchedKeys, 0, 0))
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
            OnComplete.ExecuteIfBound(false, {}, ErrorMessage);
        }), false);
}

void UGameJoltDataStoreManager::SetUserData(FOnDataStoreOpComplete OnComplete, const FString& Username, const FString& UserToken, const FString& Key, const FString& Data)
{
    if (!SubsystemPtr.IsValid())
    {
        OnComplete.ExecuteIfBound(false, TEXT("Invalid Subsystem."));
        return;
    }

    TMap<FString, FString> Params;
    Params.Add(TEXT("username"), Username);
    Params.Add(TEXT("user_token"), UserToken);
    Params.Add(TEXT("key"), Key);
    Params.Add(TEXT("data"), Data);
    
    SubsystemPtr->MakeApiRequest(TEXT("/data-store/set"), Params, FHttpRequestCompleteDelegate::CreateLambda(
        [OnComplete, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            FString ErrorMessage;
            const bool bSuccess = SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage);
            OnComplete.ExecuteIfBound(bSuccess, ErrorMessage);
        }), true);
}

void UGameJoltDataStoreManager::FetchUserData(FOnDataStoreFetchComplete OnComplete, const FString& Username, const FString& UserToken, const FString& Key)
{
    if (!SubsystemPtr.IsValid())
    {
        OnComplete.ExecuteIfBound(false, TEXT(""), TEXT("Invalid Subsystem."));
        return;
    }

    TMap<FString, FString> Params;
    Params.Add(TEXT("username"), Username);
    Params.Add(TEXT("user_token"), UserToken);
    Params.Add(TEXT("key"), Key);

    SubsystemPtr->MakeApiRequest(TEXT("/data-store"), Params, FHttpRequestCompleteDelegate::CreateLambda(
        [OnComplete, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            FString ErrorMessage, FetchedData;
            if (SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage))
            {
                const TSharedPtr<FJsonObject> JsonObject = SubsystemPtr->ParseResponse(Response);
                if (JsonObject.IsValid() && JsonObject->TryGetStringField(TEXT("data"), FetchedData))
                {
                    OnComplete.ExecuteIfBound(true, FetchedData, TEXT(""));
                    return;
                }
                ErrorMessage = TEXT("Failed to extract data from response.");
            }
            OnComplete.ExecuteIfBound(false, TEXT(""), ErrorMessage);
        }), false);
}

void UGameJoltDataStoreManager::RemoveUserData(FOnDataStoreOpComplete OnComplete, const FString& Username, const FString& UserToken, const FString& Key)
{
    if (!SubsystemPtr.IsValid())
    {
        OnComplete.ExecuteIfBound(false, TEXT("Invalid Subsystem."));
        return;
    }
    
    TMap<FString, FString> Params;
    Params.Add(TEXT("username"), Username);
    Params.Add(TEXT("user_token"), UserToken);
    Params.Add(TEXT("key"), Key);

    SubsystemPtr->MakeApiRequest(TEXT("/data-store/remove"), Params, FHttpRequestCompleteDelegate::CreateLambda(
        [OnComplete, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            FString ErrorMessage;
            const bool bSuccess = SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage);
            OnComplete.ExecuteIfBound(bSuccess, ErrorMessage);
        }), true);
}

void UGameJoltDataStoreManager::FetchUserKeys(FOnDataStoreKeysComplete OnComplete, const FString& Username, const FString& UserToken)
{
    if (!SubsystemPtr.IsValid())
    {
        OnComplete.ExecuteIfBound(false, {}, TEXT("Invalid Subsystem."));
        return;
    }
    
    TMap<FString, FString> Params;
    Params.Add(TEXT("username"), Username);
    Params.Add(TEXT("user_token"), UserToken);

    SubsystemPtr->MakeApiRequest(TEXT("/data-store/get-keys"), Params, FHttpRequestCompleteDelegate::CreateLambda(
        [OnComplete, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            TArray<FGameJoltDataKey> FetchedKeys;
            FString ErrorMessage;
            if (SubsystemPtr->IsResponseSuccessful(Response, bWasSuccessful, ErrorMessage))
            {
                const TSharedPtr<FJsonObject> JsonObject = SubsystemPtr->ParseResponse(Response);
                if (JsonObject.IsValid())
                {
                    const TArray<TSharedPtr<FJsonValue>>* KeysJsonArray;
                    if (JsonObject->TryGetArrayField(TEXT("keys"), KeysJsonArray))
                    {
                        if(FJsonObjectConverter::JsonArrayToUStruct(*KeysJsonArray, &FetchedKeys, 0, 0))
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
            OnComplete.ExecuteIfBound(false, {}, ErrorMessage);
        }), false);
}