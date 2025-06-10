#include "GJUser.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"
#include "GenericPlatform/GenericPlatformHttp.h"

#define GAMEJOLT_BASE_URL "https://api.gamejolt.com/api/game/v1.2/"

UGameJoltUserLibrary::UGameJoltUserLibrary()
{
	
}

UGameJoltUserLibrary::~UGameJoltUserLibrary()
{
	
}


void UGameJoltUserLibrary::AuthUser(const FString& GameID, const FString& Username, const FString& UserToken,
	UObject* WorldContextObject, FUserDataCallback_BP Callback)
{
	const FString URL = FString::Printf(TEXT( GAMEJOLT_BASE_URL "users/auth/?game_id=%s&username=%s&user_token=%s"),
		*GameID,
		*FGenericPlatformHttp::UrlEncode(*Username),
		*FGenericPlatformHttp::UrlEncode(*UserToken));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(URL);
	Request->SetVerb(TEXT("GET"));
	Request->OnProcessRequestComplete().BindLambda([&Callback, Username](FHttpRequestPtr Rep, FHttpResponsePtr Response, bool bSucceeded)
	{
		if (!bSucceeded && Response->GetResponseCode() != 200)
		{
			UE_LOG(LogTemp, Error, TEXT("Error: %s"), *Response->GetContentAsString());
			Callback.ExecuteIfBound(nullptr);
			return;
		}

		const FString ResponseStr = Response->GetContentAsString();
		TSharedPtr<FJsonObject> RootJson;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);

		if (!FJsonSerializer::Deserialize(Reader, RootJson) || !RootJson.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid JSON response"));
			Callback.ExecuteIfBound(nullptr);
			return;
		}

		// Check if authentication was successful
		bool bSuccess = false;
		if(!RootJson->TryGetBoolField(TEXT("success"), bSuccess) || !bSuccess)
		{
			FString ErrorMessage;
			RootJson->TryGetStringField(TEXT("message"), ErrorMessage);
			UE_LOG(LogTemp, Warning, TEXT("Authentication failed: %s"), *ErrorMessage);
			Callback.ExecuteIfBound(nullptr);
			return;
		}

		// If we made it here, we have the Data
		UGJUserData* NewUserData = NewObject<UGJUserData>();
		NewUserData->GJUserID = 0;
		NewUserData->GJUserDisplayName = Username;
		NewUserData->GJUserType = TEXT("user");

		Callback.ExecuteIfBound(NewUserData);
	});
	Request->ProcessRequest();
}

void UGameJoltUserLibrary::FetchUser(const FString &gameID, const FString &UserIdentifier, UObject* WorldContextObject, EUserIdtifierType IdentifierType)
{
	if (!WorldContextObject)
	{
		UE_LOG(LogTemp, Error, TEXT("WorldContextObject is null"));
		return;
	}
	if (UserIdentifier.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("UserIdentifier is empty"));
		return;
	}

	// Split UserIdentifiers by comma
	TArray<FString> Identifiers;
	UserIdentifier.ParseIntoArray(Identifiers, TEXT(","), true);
    
	// If no valid identifiers after splitting, log error and return
	if (Identifiers.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No valid user identifiers provided"));
		return;
	}

	// Join the array back into a comma-separated string, trimming each element
	FString JoinedIdentifiers;
	for (int32 i = 0; i < Identifiers.Num(); ++i)
	{
		FString& Id = Identifiers[i];
		Id.TrimStartAndEndInline();
		if (!Id.IsEmpty())
		{
			if (!JoinedIdentifiers.IsEmpty())
			{
				JoinedIdentifiers += ",";
			}
			JoinedIdentifiers += Id;
		}
	}

	if (JoinedIdentifiers.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("No valid user identifiers after trimming"));
		return;
	}

	const FString ParamName = (IdentifierType == EUserIdtifierType::UserID) ? TEXT("user_id") : TEXT("username");
	const FString URL = FString::Printf(TEXT(GAMEJOLT_BASE_URL "users/?game_id=%s&%s=%s"), 
		*gameID, 
		*ParamName, 
		*FGenericPlatformHttp::UrlEncode(*JoinedIdentifiers));
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(URL);
	Request->SetVerb(TEXT("GET"));
	Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Rep, FHttpResponsePtr Response, bool bSucceeded)
	{
		if(bSucceeded && Response->GetResponseCode() == 200)
		{
			UE_LOG(LogTemp, Warning, TEXT("User Authenticated"));
			UGJUserData* NewUserData = NewObject<UGJUserData>();
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				NewUserData->SetUserData(JsonObject);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to parse JSON response"));
			}
		}
		else
		{
			FString ResponseStr = Response.IsValid() ? FString::Printf(TEXT("Error %d: %s"), Response->GetResponseCode(), *Response->GetContentAsString()) : TEXT("No response");
			UE_LOG(LogTemp, Warning, TEXT("User Authentication Failed: %s"), *ResponseStr);
		}
	});
	Request->ProcessRequest();
 }

bool UGameJoltUserLibrary::IsNumeric(const FString& Username)
{
	for (TCHAR Char : Username)
	{
		if (!FChar::IsDigit(Char))
		{
			return false;
		}
	}
	return !Username.IsEmpty();
}
