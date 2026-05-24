#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UGameJoltTypes.h"
#include "UGameJoltUserManager.h"
#include "UGameJoltTrophyManager.h"
#include "UGameJoltScoreManager.h"
#include "UGameJoltDataStoreManager.h"
#include "UGameJoltSessionManager.h"
#include "UGameJoltBlueprintLibrary.generated.h"

/**
 * One-stop Blueprint function library for the Game Jolt API.
 *
 * Every function here is a single Blueprint node — no subsystem or manager
 * references required. Just drag, drop, and connect your delegates.
 *
 * Typical flow:
 *   1. GJ Login  (provide username + token)
 *   2. GJ Open Session
 *   3. Do game stuff (trophies, scores, data store)
 *   4. GJ Close Session  (on exit)
 */
UCLASS()
class GAMEJOLT_API UGameJoltBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// -----------------------------------------------------------------------
	//  USERS
	// -----------------------------------------------------------------------

	/**
	 * Authenticates a Game Jolt user with their username and game token.
	 * Must be called before any user-specific API calls (trophies, sessions, etc).
	 * The credentials are cached automatically for subsequent calls.
	 *
	 * @param WorldContextObject  Leave this as 'self' in Blueprint.
	 * @param Username            The player's Game Jolt username.
	 * @param UserToken           The player's Game Jolt game token (not their password).
	 * @param OnComplete          Fires when done. Check bSuccess; AuthenticatedUser is populated on success.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Users",
		meta = (DisplayName = "GJ Login", WorldContext = "WorldContextObject"))
	static void Login(
		const UObject* WorldContextObject,
		const FString& Username,
		const FString& UserToken,
		FOnAuthUserComplete OnComplete);

	/**
	 * Returns whether a user is currently authenticated this session.
	 *
	 * @param WorldContextObject  Leave this as 'self' in Blueprint.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game Jolt|Users",
		meta = (DisplayName = "GJ Is Logged In", WorldContext = "WorldContextObject"))
	static bool IsLoggedIn(const UObject* WorldContextObject);

	/**
	 * Returns the currently authenticated user's username, or an empty string if not logged in.
	 *
	 * @param WorldContextObject  Leave this as 'self' in Blueprint.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game Jolt|Users",
		meta = (DisplayName = "GJ Get Current Username", WorldContext = "WorldContextObject"))
	static FString GetCurrentUsername(const UObject* WorldContextObject);


	// -----------------------------------------------------------------------
	//  SESSIONS
	// -----------------------------------------------------------------------

	/**
	 * Opens a game session for the currently logged-in user and begins automatic pinging.
	 * Call this shortly after a successful GJ Login.
	 * Sessions not pinged within 120 seconds are closed by Game Jolt automatically.
	 *
	 * @param WorldContextObject  Leave this as 'self' in Blueprint.
	 * @param OnComplete          Fires when the session is open (or failed).
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Sessions",
		meta = (DisplayName = "GJ Open Session", WorldContext = "WorldContextObject"))
	static void OpenSession(
		const UObject* WorldContextObject,
		FOnSessionComplete OnComplete);

	/**
	 * Closes the active game session and stops automatic pinging.
	 * Call this when the player quits or logs out.
	 *
	 * @param WorldContextObject  Leave this as 'self' in Blueprint.
	 * @param OnComplete          Fires when the session is closed (or failed).
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Sessions",
		meta = (DisplayName = "GJ Close Session", WorldContext = "WorldContextObject"))
	static void CloseSession(
		const UObject* WorldContextObject,
		FOnSessionComplete OnComplete);

	/**
	 * Manually pings the active session. Not required if you used GJ Open Session,
	 * as that handles pinging automatically. Use this only if you need manual control.
	 *
	 * @param WorldContextObject  Leave this as 'self' in Blueprint.
	 * @param bIsActive           True = player is actively playing; False = player is idle.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Sessions",
		meta = (DisplayName = "GJ Ping Session", WorldContext = "WorldContextObject"))
	static void PingSession(
		const UObject* WorldContextObject,
		bool bIsActive = true);


	// -----------------------------------------------------------------------
	//  TROPHIES
	// -----------------------------------------------------------------------

	/**
	 * Fetches trophies for the currently logged-in user.
	 * Requires a successful GJ Login first.
	 *
	 * @param WorldContextObject  Leave this as 'self' in Blueprint.
	 * @param bOnlyAchieved       True = only return trophies the user has earned.
	 *                            False = return all trophies for the game.
	 * @param OnComplete          Fires when done. Trophies array is populated on success.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Trophies",
		meta = (DisplayName = "GJ Fetch Trophies", WorldContext = "WorldContextObject"))
	static void FetchTrophies(
		const UObject* WorldContextObject,
		bool bOnlyAchieved,
		FOnFetchTrophiesComplete OnComplete);


	// -----------------------------------------------------------------------
	//  SCORES
	// -----------------------------------------------------------------------

	/**
	 * Fetches scores from a score table.
	 *
	 * @param WorldContextObject  Leave this as 'self' in Blueprint.
	 * @param TableID             The table to fetch from. Use 0 for the primary table.
	 * @param Limit               Max number of scores to return (default 10).
	 * @param bCurrentUserOnly    True = only return scores for the logged-in user.
	 * @param OnComplete          Fires when done. Scores array is populated on success.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Scores",
		meta = (DisplayName = "GJ Fetch Scores", WorldContext = "WorldContextObject"))
	static void FetchScores(
		const UObject* WorldContextObject,
		int32 TableID,
		int32 Limit,
		bool bCurrentUserOnly,
		FOnFetchScoresComplete OnComplete);

	/**
	 * Fetches all score tables for this game.
	 *
	 * @param WorldContextObject  Leave this as 'self' in Blueprint.
	 * @param OnComplete          Fires when done. ScoreTables array is populated on success.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Scores",
		meta = (DisplayName = "GJ Fetch Score Tables", WorldContext = "WorldContextObject"))
	static void FetchScoreTables(
		const UObject* WorldContextObject,
		FOnFetchScoreTablesComplete OnComplete);

	/**
	 * Submits a score for the currently logged-in user.
	 * Requires a successful GJ Login first.
	 *
	 * @param WorldContextObject  Leave this as 'self' in Blueprint.
	 * @param ScoreText           The display string for the score (e.g. "5000 Points").
	 * @param SortValue           The numeric value used for ranking (e.g. 5000).
	 * @param TableID             The table to submit to. Use 0 for the primary table.
	 * @param OnComplete          Fires when done.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Scores",
		meta = (DisplayName = "GJ Submit Score", WorldContext = "WorldContextObject"))
	static void SubmitScore(
		const UObject* WorldContextObject,
		const FString& ScoreText,
		int32 SortValue,
		int32 TableID,
		FOnDataStoreOpComplete OnComplete);

	/**
	 * Submits a score for a guest (non-logged-in) player.
	 *
	 * @param WorldContextObject  Leave this as 'self' in Blueprint.
	 * @param GuestName           The display name for the guest.
	 * @param ScoreText           The display string for the score (e.g. "5000 Points").
	 * @param SortValue           The numeric value used for ranking (e.g. 5000).
	 * @param TableID             The table to submit to. Use 0 for the primary table.
	 * @param OnComplete          Fires when done.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Scores",
		meta = (DisplayName = "GJ Submit Guest Score", WorldContext = "WorldContextObject"))
	static void SubmitGuestScore(
		const UObject* WorldContextObject,
		const FString& GuestName,
		const FString& ScoreText,
		int32 SortValue,
		int32 TableID,
		FOnAddScoreComplete OnComplete);

	/**
	 * Gets the rank of a specific score value in a table.
	 *
	 * @param WorldContextObject  Leave this as 'self' in Blueprint.
	 * @param SortValue           The numeric score value to find the rank of.
	 * @param TableID             The table to check. Use 0 for the primary table.
	 * @param OnComplete          Fires when done. Rank is 1-based on success.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Scores",
		meta = (DisplayName = "GJ Get Score Rank", WorldContext = "WorldContextObject"))
	static void GetScoreRank(
		const UObject* WorldContextObject,
		int32 SortValue,
		int32 TableID,
		FOnGetRankComplete OnComplete);


	// -----------------------------------------------------------------------
	//  DATA STORE
	// -----------------------------------------------------------------------

	/**
	 * Stores a string value in the global (game-wide) data store.
	 *
	 * @param WorldContextObject  Leave this as 'self' in Blueprint.
	 * @param Key                 The key to store data under.
	 * @param Data                The string value to store.
	 * @param OnComplete          Fires when done.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Data Store",
		meta = (DisplayName = "GJ Set Global Data", WorldContext = "WorldContextObject"))
	static void SetGlobalData(
		const UObject* WorldContextObject,
		const FString& Key,
		const FString& Data,
		FOnDataStoreOpComplete OnComplete);

	/**
	 * Fetches a string value from the global (game-wide) data store.
	 *
	 * @param WorldContextObject  Leave this as 'self' in Blueprint.
	 * @param Key                 The key to fetch.
	 * @param OnComplete          Fires when done. Data string is populated on success.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Data Store",
		meta = (DisplayName = "GJ Get Global Data", WorldContext = "WorldContextObject"))
	static void GetGlobalData(
		const UObject* WorldContextObject,
		const FString& Key,
		FOnDataStoreFetchComplete OnComplete);

	/**
	 * Removes a key from the global (game-wide) data store.
	 *
	 * @param WorldContextObject  Leave this as 'self' in Blueprint.
	 * @param Key                 The key to remove.
	 * @param OnComplete          Fires when done.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Data Store",
		meta = (DisplayName = "GJ Remove Global Data", WorldContext = "WorldContextObject"))
	static void RemoveGlobalData(
		const UObject* WorldContextObject,
		const FString& Key,
		FOnDataStoreOpComplete OnComplete);

	/**
	 * Stores a string value in the currently logged-in user's data store.
	 * Requires a successful GJ Login first.
	 *
	 * @param WorldContextObject  Leave this as 'self' in Blueprint.
	 * @param Key                 The key to store data under.
	 * @param Data                The string value to store.
	 * @param OnComplete          Fires when done.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Data Store",
		meta = (DisplayName = "GJ Set User Data", WorldContext = "WorldContextObject"))
	static void SetUserData(
		const UObject* WorldContextObject,
		const FString& Key,
		const FString& Data,
		FOnDataStoreOpComplete OnComplete);

	/**
	 * Fetches a string value from the currently logged-in user's data store.
	 * Requires a successful GJ Login first.
	 *
	 * @param WorldContextObject  Leave this as 'self' in Blueprint.
	 * @param Key                 The key to fetch.
	 * @param OnComplete          Fires when done. Data string is populated on success.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Data Store",
		meta = (DisplayName = "GJ Get User Data", WorldContext = "WorldContextObject"))
	static void GetUserData(
		const UObject* WorldContextObject,
		const FString& Key,
		FOnDataStoreFetchComplete OnComplete);

	/**
	 * Removes a key from the currently logged-in user's data store.
	 * Requires a successful GJ Login first.
	 *
	 * @param WorldContextObject  Leave this as 'self' in Blueprint.
	 * @param Key                 The key to remove.
	 * @param OnComplete          Fires when done.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Jolt|Data Store",
		meta = (DisplayName = "GJ Remove User Data", WorldContext = "WorldContextObject"))
	static void RemoveUserData(
		const UObject* WorldContextObject,
		const FString& Key,
		FOnDataStoreOpComplete OnComplete);

private:

	/**
	*  Bool check to prevent multiple simultaneous API calls of the same type,which the Game Jolt API does not allow. This is not strictly necessary but helps catch potential issues in Blueprints (e.g. accidentally calling FetchScores every frame).
	*/
	bool bFetchScoresInFlight = false;

	/** Internal helper — retrieves the subsystem from any world context object. Returns nullptr and logs on failure. */
	static UGameJoltSubsystem* GetSubsystem(const UObject* WorldContextObject);
};
