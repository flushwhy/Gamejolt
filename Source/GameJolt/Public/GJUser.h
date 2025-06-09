#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GJUser.generated.h"


USTRUCT(BlueprintType)
struct FGJ
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Username;

	UPROPERTY(BlueprintReadOnly)
	FString UserID;

	UPROPERTY(BlueprintReadOnly)
	FString Token;
};


UCLASS()
class UGameJoltUserLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "GameJolt")
	static void AuthUser(const FString &gameID,const FString &Username, const FString &Token);

	UFUNCTION(BlueprintCallable, Category = "GameJolt")
	static void FetchUser(const FString &UserID,const FString &Username,
		const FString &Token);

private:

	int32 GJUserID;
	FString GJUserType;
	FString GJUserAvatar;
	FString GJUserSignupDate;
	FString GJUserSignupTimeStamp;
	FString GJUserLastLoginDate;
	FString GJUserLastLoginTimeStamp;
	FString GJUserStatus;
};
