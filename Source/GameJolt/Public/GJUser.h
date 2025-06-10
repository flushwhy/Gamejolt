#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UGJUserData.h"
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


// This is for FetchUser since it takes ID or Username.
UENUM(BlueprintType)
enum class EUserIdtifierType : uint8
{
	Username,
	UserID
};

UCLASS()
class GAMEJOLT_API UGameJoltUserLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UGameJoltUserLibrary();
	~UGameJoltUserLibrary();

	DECLARE_DYNAMIC_DELEGATE_OneParam(FUserDataCallback_BP, UGJUserData*, UserData);
	UFUNCTION(BlueprintCallable, Category = "GameJolt|Users", meta = (WorldContext = "WorldContextObject"))
	static void AuthUser(const FString& GameID, const FString& Username, const FString& UserToken, UObject* WorldContextObject, FUserDataCallback_BP Callback);


	UFUNCTION(BlueprintCallable, Category = "GameJolt|Users", meta = (WorldContext = "WorldContextObject"))
	static void FetchUser(
		const FString &gameID,
		const FString &UserIdentifier,
		UObject* WorldContextObject,
		EUserIdtifierType IdentifierType = EUserIdtifierType::Username);

	UFUNCTION(BlueprintCallable, Category = "GameJolt|Users")
	static bool IsNumeric(const FString& Username);
};
