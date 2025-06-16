#include "UGameJoltSubsystem.h"
#include "UGameJoltSettings.h"
#include "UGameJoltTrophyManager.h"
#include "UGameJoltScoreManager.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "HttpModule.h"
#include "Misc/SecureHash.h"
#include "GenericPlatform/GenericPlatformHttp.h"

#include "UGameJoltUserManager.h"
#include "UGameJoltSessionManager.h"
#include "UGameJoltDataStoreManager.h"

// --- Subsystem Lifecycle ---

void UGameJoltSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// Load settings from Project Settings
	const UGameJoltSettings* Settings = GetDefault<UGameJoltSettings>();
	GameID = Settings->GameID;
	PrivateKey = Settings->PrivateKey;

	// Create and initialize managers
	TrophyManager = NewObject<UGameJoltTrophyManager>(this);
	TrophyManager->Initialize(this);
	
	ScoreManager = NewObject<UGameJoltScoreManager>(this);
	ScoreManager->Initialize(this);

    DataStoreManager = NewObject<UGameJoltDataStoreManager>(this);
    DataStoreManager->Initialize(this);

	SessionManager = NewObject<UGameJoltSessionManager>(this);
	SessionManager->Initialize(this);

    UserManager = NewObject<UGameJoltUserManager>(this);
    UserManager->Initialize(this);

	// Add other managers here as you create them
}

void UGameJoltSubsystem::Deinitialize()
{
	Super::Deinitialize();

	// Managers will be garbage collected, no need to manually null them
}

// --- API Request Logic ---

void UGameJoltSubsystem::MakeApiRequest(const FString& Endpoint, const TMap<FString, FString>& Parameters, const FHttpRequestCompleteDelegate& OnComplete, bool bIsPostRequest)
{
	// 1. Pre-flight Checks
	if (GameID.IsEmpty() || PrivateKey.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("GameJolt: Game ID or Private Key is not set in Project Settings. Aborting API request."));
		OnComplete.ExecuteIfBound(nullptr, nullptr, false);
		return;
	}

	// 2. Assemble All Parameters
	TMap<FString, FString> AllParams = Parameters;
	AllParams.Add(TEXT("game_id"), GameID);
	AllParams.Add(TEXT("format"), TEXT("json"));
	AllParams.KeySort(TLess<FString>()); // Sort alphabetically for a consistent signature

	FString Signature;
	FString Url;
	FString PostBody;

	const FString BaseUrl = TEXT("https://gamejolt.com/api/game/v1_2");

	// 3. Build Request and Signature based on method (GET vs POST)
	if (bIsPostRequest)
	{
		// --- POST Request Logic ---
		Url = BaseUrl + Endpoint;

		// Create the URL-encoded body for the actual request (key1=value1&key2=value2)
		FString RequestBodyString = TEXT("");
		for (const auto& ParamPair : AllParams)
		{
			RequestBodyString.Append(FGenericPlatformHttp::UrlEncode(ParamPair.Key) + TEXT("=") + FGenericPlatformHttp::UrlEncode(ParamPair.Value) + TEXT("&"));
		}
		RequestBodyString.RemoveFromEnd(TEXT("&"));
		
		// Create the special signature payload (key1value1key2value2...) as per documentation
		FString SignaturePayload = TEXT("");
		for (const auto& ParamPair : AllParams)
		{
			SignaturePayload.Append(ParamPair.Key + ParamPair.Value);
		}

		// Hash the URL + Signature Payload + Private Key
		const FString StringToHash = Url + SignaturePayload + PrivateKey;
		Signature = FMD5::HashAnsiString(*StringToHash);
		UE_LOG(LogTemp, Log, TEXT("GameJolt: String to hash: %s"), *StringToHash);

		// The final body includes the encoded params AND the signature
		PostBody = RequestBodyString + TEXT("&signature=") + Signature;
	}
	else
	{
		// --- GET Request Logic ---
		FString ParamString = TEXT("");
		for (const auto& ParamPair : AllParams)
		{
			ParamString.Append(FGenericPlatformHttp::UrlEncode(ParamPair.Key) + TEXT("=") + FGenericPlatformHttp::UrlEncode(ParamPair.Value) + TEXT("&"));
		}
		ParamString.RemoveFromEnd(TEXT("&"));

		Url = BaseUrl + Endpoint + TEXT("?") + ParamString;
		
		// Hash the full URL with parameters + Private Key
		const FString StringToHash = Url + PrivateKey;
		Signature = FMD5::HashAnsiString(*StringToHash);
		UE_LOG(LogTemp, Log, TEXT("GameJolt ELSE: String to hash: %s"), *StringToHash);

		// The final URL includes the signature
		Url.Append(TEXT("&signature=") + Signature);
	}
	// 4. Create and Configure the HTTP Request
	FHttpModule& HttpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = HttpModule.CreateRequest();
	HttpRequest->OnProcessRequestComplete() = OnComplete;
	HttpRequest->SetURL(Url);

	if (bIsPostRequest)
	{
		HttpRequest->SetVerb(TEXT("POST"));
		HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
		HttpRequest->SetContentAsString(PostBody);
		UE_LOG(LogTemp, Log, TEXT("GameJolt POST Body: %s"), *PostBody);
	}
	else
	{
		HttpRequest->SetVerb(TEXT("GET"));
	}
	
	// 5. Process the Request
	UE_LOG(LogTemp, Log, TEXT("GameJolt Request URL: %s"), *HttpRequest->GetURL());
	HttpRequest->ProcessRequest();
}

// --- Helper Functions ---

TSharedPtr<FJsonObject> UGameJoltSubsystem::ParseResponse(const FHttpResponsePtr& Response) const
{
	if (!Response.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("GameJolt: ParseResponse called with an invalid HTTP response pointer."));
		return nullptr;
	}

	TSharedPtr<FJsonObject> RootObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("GameJolt: Failed to deserialize JSON response."));
		return nullptr;
	}

	const TSharedPtr<FJsonObject>* ResponseObject;
	if (!RootObject->TryGetObjectField(TEXT("response"), ResponseObject))
	{
		UE_LOG(LogTemp, Warning, TEXT("GameJolt: Field 'response' not found in JSON."));
		return nullptr;
	}

	if (!(*ResponseObject)->GetBoolField(TEXT("success")))
	{
		FString ApiErrorMessage;
		(*ResponseObject)->TryGetStringField(TEXT("message"), ApiErrorMessage);
		UE_LOG(LogTemp, Warning, TEXT("GameJolt API Error: %s"), *ApiErrorMessage);
		return nullptr;
	}

	return *ResponseObject;
}

bool UGameJoltSubsystem::IsResponseSuccessful(const FHttpResponsePtr& Response, bool bWasSuccessful, FString& OutErrorMessage) const
{
	if (!bWasSuccessful)
	{
		OutErrorMessage = TEXT("HTTP transport failed. Check network connection.");
		return false;
	}

	if (!Response.IsValid() || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		OutErrorMessage = FString::Printf(TEXT("HTTP request failed with code: %d"), Response.IsValid() ? Response->GetResponseCode() : -1);
		return false;
	}

	const TSharedPtr<FJsonObject> ResponseObject = ParseResponse(Response);
	if (!Response.IsValid())
	{
		OutErrorMessage = TEXT("Response was not successful or failed to parse.");
		return false;
	}

	return true;
}
