#include "UGameJoltSubsystem.h"
#include "UGameJoltSettings.h"
#include "UGameJoltTrophyManager.h"
#include "UGameJoltScoreManager.h"
#include "UGameJoltUserManager.h"
#include "UGameJoltSessionManager.h"
#include "UGameJoltDataStoreManager.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "HttpModule.h"
#include "Misc/SecureHash.h"
#include "GenericPlatform/GenericPlatformHttp.h"

// --- Subsystem Lifecycle ---

void UGameJoltSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Load settings from Project Settings
	const UGameJoltSettings* Settings = GetDefault<UGameJoltSettings>();
	GameID = Settings->GameID;
	PrivateKey = Settings->PrivateKey;

	// Create and initialize all managers
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
}

void UGameJoltSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

// --- API Request Logic ---

void UGameJoltSubsystem::MakeApiRequest(const FString& Endpoint, const TMap<FString, FString>& Parameters, const FHttpRequestCompleteDelegate& OnComplete)
{
	// Pre-flight checks
	if (GameID.IsEmpty() || PrivateKey.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("GameJolt: Game ID or Private Key is not set in Project Settings. Aborting API request."));
		OnComplete.ExecuteIfBound(nullptr, nullptr, false);
		return;
	}

	// Assemble all parameters, including mandatory game_id and format
	TMap<FString, FString> AllParams = Parameters;
	AllParams.Add(TEXT("game_id"), GameID);
	AllParams.Add(TEXT("format"), TEXT("json"));

	// Sort keys alphabetically for a consistent, deterministic URL
	AllParams.KeySort(TLess<FString>());

	// Build the query string
	// NOTE: Per Game Jolt API v1.2 spec, the signature is computed on the *unencoded* URL,
	// so we build that first, then URL-encode only for the final request.
	//
	// Spec: https://gamejolt.com/game-api/doc/construction
	// Correct base URL host is api.gamejolt.com, not gamejolt.com.
	const FString BaseUrl = TEXT("https://api.gamejolt.com/api/game/v1_2");

	// Step 1: Build the raw (unencoded) URL for signing
	FString RawParamString;
	for (const auto& ParamPair : AllParams)
	{
		RawParamString.Append(ParamPair.Key + TEXT("=") + ParamPair.Value + TEXT("&"));
	}
	RawParamString.RemoveFromEnd(TEXT("&"));

	const FString RawUrl = BaseUrl + Endpoint + TEXT("?") + RawParamString;

	// Step 2: Generate the signature: MD5(RawUrl + PrivateKey)
	const FString Signature = GenerateSignature(RawUrl);

	// Step 3: Build the final URL with URL-encoded parameters + signature appended
	FString EncodedParamString;
	for (const auto& ParamPair : AllParams)
	{
		EncodedParamString.Append(
			FGenericPlatformHttp::UrlEncode(ParamPair.Key) + TEXT("=") +
			FGenericPlatformHttp::UrlEncode(ParamPair.Value) + TEXT("&")
		);
	}
	EncodedParamString.RemoveFromEnd(TEXT("&"));

	const FString FinalUrl = BaseUrl + Endpoint + TEXT("?") + EncodedParamString + TEXT("&signature=") + Signature;

	UE_LOG(LogTemp, Log, TEXT("GameJolt Request URL: %s"), *FinalUrl);

	// Create and fire the GET request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete() = OnComplete;
	HttpRequest->SetURL(FinalUrl);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();
}

// --- Helper Functions ---

FString UGameJoltSubsystem::GenerateSignature(const FString& FullUrl) const
{
	// Game Jolt API v1.2 signature spec:
	// SIGNATURE = MD5(FullUrl + PrivateKey)
	const FString StringToHash = FullUrl + PrivateKey;
	return FMD5::HashAnsiString(*StringToHash);
}

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
		UE_LOG(LogTemp, Warning, TEXT("GameJolt: Failed to deserialize JSON response. Body: %s"), *Response->GetContentAsString());
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
		OutErrorMessage = FString::Printf(TEXT("HTTP request failed with code: %d"),
			Response.IsValid() ? Response->GetResponseCode() : -1);
		return false;
	}

	// FIX: Was incorrectly checking Response.IsValid() again here instead of
	// checking whether ParseResponse succeeded (i.e. API returned success:true).
	const TSharedPtr<FJsonObject> ResponseObject = ParseResponse(Response);
	if (!ResponseObject.IsValid())
	{
		OutErrorMessage = TEXT("API returned an error or response failed to parse. Check log for details.");
		return false;
	}

	return true;
}