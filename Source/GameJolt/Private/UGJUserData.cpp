#include "UGJUserData.h"

void UGJUserData::SetUserData(const TSharedPtr<FJsonObject>& JsonObject)
{
	GJUserID = JsonObject->GetIntegerField(TEXT("id"));
	GJUserDisplayName = JsonObject->GetStringField(TEXT("developer_name"));
	GJUserType = JsonObject->GetStringField(TEXT("type"));
	GJUserAvatarURL = JsonObject->GetStringField(TEXT("avatar_url"));
	GJUserSignupDate = JsonObject->GetStringField(TEXT("signup_date"));
	GJUserLastLoginDate = JsonObject->GetStringField(TEXT("last_login_date"));
	GJUserStatus = JsonObject->GetStringField(TEXT("status"));
	GJUserAvatar = JsonObject->GetStringField(TEXT("avatar"));
	GJUserLastLoginTimeStamp = JsonObject->GetStringField(TEXT("last_login_timestamp"));
}
