# Changelog

All notable changes to this project will be documented in this file.

## [2.0.0] - TBA

This is a major release with breaking changes. Blueprint graphs from 1.x will need to be rebuilt using the new single-node API.

### Added

- **Blueprint Function Library** (`UGameJoltBlueprintLibrary`): Every API call is now a single Blueprint node under the **Game Jolt** category. No subsystem or manager references needed.
  - `GJ Login` — authenticate a player and cache credentials automatically.
  - `GJ Open Session` / `GJ Close Session` / `GJ Ping Session`
  - `GJ Fetch Trophies`
  - `GJ Submit Score` / `GJ Submit Guest Score` / `GJ Fetch Scores` / `GJ Fetch Score Tables` / `GJ Get Score Rank`
  - `GJ Set/Get/Remove User Data` — user-scoped cloud storage, credentials injected automatically.
  - `GJ Set/Get/Remove Global Data` — game-wide cloud storage.
  - `GJ Is Logged In` / `GJ Get Current Username` — pure utility nodes.

- **Centralised Delegate Declarations**: All delegates (`FOnAuthUserComplete`, `FOnSessionComplete`, `FOnFetchTrophiesComplete`, etc.) moved to `UGameJoltTypes.h` as the single source of truth. Individual manager headers no longer declare their own delegates.

### Changed

- **API Request Architecture**: `MakeApiRequest` no longer accepts a `bIsPostRequest` parameter. The Game Jolt API v1.2 is GET-only; the POST branch and body-building logic have been removed entirely.

- **Correct Base URL**: Fixed base URL from `gamejolt.com` to `api.gamejolt.com`. All previous versions were sending requests to the wrong host.

- **Signature Generation**: Corrected to match the Game Jolt API v1.2 spec exactly — `MD5(raw_unencoded_url + private_key)`. The previous implementation used an incorrect POST-specific payload format. The raw URL is now signed separately from the URL-encoded version used in the actual request.

- **`IsResponseSuccessful`**: Fixed a bug where the method re-checked `Response.IsValid()` instead of checking whether `ParseResponse` succeeded (i.e. whether the API returned `success: true`). Previously, API-level errors were silently swallowed.

- **`UGameJoltSessionManager::OpenSession`**: Fixed credentials being read from undeclared variables (`Username`/`UserToken`) instead of the locals retrieved from the subsystem (`User`/`Token`). Sessions were previously opened with empty credentials.

- **`UGameJoltScoreManager::GetScoreRank`**: Fixed inverted delegate arguments — `bSuccess` and `Rank` were passed in the wrong order.

- **`UGameJoltUserManager::AuthenticateUser`**: Fixed missing `JsonObject` declaration in the response lambda (previously referenced but never assigned), broken brace structure, and missing `Username`/`UserToken` lambda captures needed for `SetActiveUser`.

- **`GenerateSignature`**: Signature in header now matches implementation — single `const FString&` parameter.

### Removed

- `bIsPostRequest` parameter from `MakeApiRequest` and all call sites across every manager.
- POST request body construction and the `SignaturePayload` concatenation logic.
- Duplicate delegate declarations from individual manager headers (`UGameJoltDataStoreManager.h`, `UGameJoltScoreManager.h`, `UGameJoltTrophyManager.h`).
- Spurious `#include "UGameJoltScoreManager.h"` from `UGameJoltTrophyManager.h`.

### Migration Guide

| 1.x Blueprint workflow | 2.0 replacement |
|---|---|
| `GetGameInstance → GetSubsystem → GetUserManager → AuthenticateUser` | `GJ Login` |
| `GetSubsystem → GetSessionManager → OpenSession` | `GJ Open Session` |
| `GetSubsystem → GetTrophyManager → FetchTrophies` | `GJ Fetch Trophies` |
| `GetSubsystem → GetScoreManager → AddScore` | `GJ Submit Score` |
| `GetSubsystem → GetDataStoreManager → SetUserData` | `GJ Set User Data` |

---

## [1.0.2] - TBA

### Added

- Credential Caching: Added `SetActiveUser` and `GetActiveUser` to `UGameJoltSubsystem`. The plugin now remembers who is playing.
- Auth State Tracking: Added `IsUserAuthenticated()` to allow Blueprints to check login status before making API calls.

### Changed

- Streamlined Logic: Reworked `AddScore` to automatically pull credentials from the subsystem.
- Internal Plumbing: Updated `UGameJoltUserManager` to automatically update the global cache on successful login.
- BP Node Efficiency: Reduced pin count on core nodes.

### Fixed

- Signature Generation (MD5): Updated `GenerateSignature` to support Game Jolt API v1.2, handling alphabetical key sorting and correct URL concatenation.

---

## [1.0.1] - 2025-06-16

### Added
- User, Session, Score, Trophy, and DataStore Managers in place of the old system.
- Comments and documentation visible in both Blueprint and C++.
- This Changelog.

### Changed
- Moved from base architecture to a UE subsystem-based architecture.
- Renamed most of the core to align with UE standards.
- Reworked compatibility from UE 5.0 to 5.6.

### Removed
- `UGJUserData`, `GameJolt`, and `GJUser` files, replaced with manager classes.

### Fixed
- Some functions not showing in Blueprints.
- Signature generation issues and improved error handling.

---

## [1.0.0] - 2025-06-12

### Added
- The base of the plugin.

---
[2.0.0]: https://github.com/flushwhy/Game-Jolt-Plugin-Docs/releases/tag/V2.0.0
[1.0.1]: https://github.com/flushwhy/Game-Jolt-Plugin-Docs/releases/tag/V1.0.1
[1.0.0]: https://github.com/flushwhy/Game-Jolt-Plugin-Docs/releases/tag/V1
