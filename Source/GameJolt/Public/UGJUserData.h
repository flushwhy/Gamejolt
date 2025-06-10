#pragma once

#include "UGJUserData.generated.h"

UCLASS(BlueprintType)
class UGJUserData  : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "GameJolt|Users")
	int32 GJUserID;

	UPROPERTY(BlueprintReadOnly, Category = "GameJolt|Users")
	FString GJUserDisplayName; // Is developer_name in the API works for users and developers.

	UPROPERTY(BlueprintReadOnly, Category = "GameJolt|Users")
	FString GJUserType;

	UPROPERTY(BlueprintReadOnly, Category = "GameJolt|Users")
	FString GJUserAvatarURL;

	UPROPERTY(BlueprintReadOnly, Category = "GameJolt|Users")
	FString GJUserSignupDate;

	UPROPERTY(BlueprintReadOnly, Category = "GameJolt|Users")
	FString GJUserLastLoginDate;

	UPROPERTY(BlueprintReadOnly, Category = "GameJolt|Users")
	FString GJUserStatus;

	UPROPERTY(BlueprintReadOnly, Category = "GameJolt|Users")
	FString GJUserAvatar;

	UPROPERTY(BlueprintReadOnly, Category = "GameJolt|Users")
	FString GJUserLastLoginTimeStamp;
	
	void SetUserData(const TSharedPtr<FJsonObject>& JsonObject);
};
