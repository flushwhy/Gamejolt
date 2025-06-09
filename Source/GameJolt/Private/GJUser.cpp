#include "GJUser.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"

void UGameJoltUserLibrary::FetchUser(const FString &gameID,const FString &Username, const FString &Token)
{
	FString URL = FString::Printf(TEXT("https://api.gamejolt.com/api/v1.2/users/auth/?game_id=%s&username=%s&user_token=%s"), *gameID, *Username, *Token);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(URL);
	Request->SetVerb(TEXT("GET"));
	Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Req, FHttpResponsePtr Response, bool bSucceeded)
	{
		if (bSucceeded && Response->GetResponseCode() == 200)
		{
			UE_LOG(LogTemp, Warning, TEXT("Auth Success"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Auth Failed"));
		}
	});
	Request->ProcessRequest();
}