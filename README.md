# GameJolt Plugin for Unreal Engine

This plugin simplifies integrating Game Jolt features into your Unreal Engine projects, with full support for both C++ and Blueprints.

---

## Features

- **User Authentication** — Securely log in players with their Game Jolt credentials.
- **Session Management** — Open, ping, and close game sessions to track when users are playing.
- **Trophies** — Fetch trophy information for the logged-in user.
- **Scores** — Submit scores to leaderboards and fetch high scores, including guest scores.
- **Cloud Data Storage** — Save and load game data to the cloud, either globally or per-user.

---

## Requirements
>⚠️ This plugin requires a C++ or C++/Blueprint project.
Pure Blueprint-only projects cannot load C++ plugins.
If your project is Blueprint-only, convert it first:
Tools → New C++ Class → choose any class → compile.
This is a one-time step and does not affect your existing Blueprints.

---
## Installation

1. Go to the **Releases** page of this repository and download the latest `.zip` file.
2. Find your Unreal Engine project's root directory.
3. Create a folder named `Plugins` if it doesn't already exist.
4. Extract the `GameJolt` folder from the `.zip` into your `Plugins` folder. The final path should look like `MyProject/Plugins/GameJolt/`.
5. Right-click your `.uproject` file and select **"Generate Visual Studio project files"**.
6. Open your project. Unreal Engine will prompt you to build the new plugin — click **"Yes"**.

---

## Setup

Before using the API, provide your game's credentials:

1. Open your project and navigate to **Edit → Project Settings**.
2. In the left panel, scroll to the **Plugins** section and click **Game Jolt API**.
3. Enter your **Game ID** and **Private Key**, found on your Game Jolt game dashboard under **Game API → API Settings**.

---

## Usage

### Blueprint

All API functions are available as single Blueprint nodes under the **Game Jolt** category. No subsystem or manager references are needed.

**Typical flow:**

```
Event BeginPlay
  → GJ Login (Username, Token)
      → On Complete (bSuccess = true)
          → GJ Open Session
```

Then anywhere in any Blueprint:

| Node | Description |
|---|---|
| `GJ Login` | Authenticate a player. Must be called first. |
| `GJ Open Session` | Open a session and start auto-pinging. |
| `GJ Close Session` | Close the session on exit. |
| `GJ Fetch Trophies` | Get trophies for the logged-in user. |
| `GJ Submit Score` | Submit a score for the logged-in user. |
| `GJ Submit Guest Score` | Submit a score for a guest player. |
| `GJ Fetch Scores` | Fetch scores from a leaderboard table. |
| `GJ Get Score Rank` | Get the rank of a score value. |
| `GJ Set User Data` | Save a string to the user's cloud data store. |
| `GJ Get User Data` | Load a string from the user's cloud data store. |
| `GJ Set Global Data` | Save a string to the global (game-wide) data store. |
| `GJ Get Global Data` | Load a string from the global data store. |

All nodes fire a delegate on completion with a `bSuccess` bool and an `ErrorMessage` string on failure.

---

### C++

**Authentication:**

```cpp
#include "UGameJoltBlueprintLibrary.h"

void AMyPlayerController::AuthenticatePlayer(const FString& Username, const FString& UserToken)
{
    FOnAuthUserComplete OnComplete;
    OnComplete.BindLambda([](bool bSuccess, const FGameJoltUser& User, const FString& ErrorMessage)
    {
        if (bSuccess)
        {
            UE_LOG(LogTemp, Log, TEXT("Authenticated: %s"), *User.username);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Auth failed: %s"), *ErrorMessage);
        }
    });

    UGameJoltBlueprintLibrary::Login(this, Username, UserToken, OnComplete);
}
```

**Submitting a score:**

```cpp
FOnDataStoreOpComplete OnComplete;
OnComplete.BindLambda([](bool bSuccess, const FString& ErrorMessage)
{
    if (!bSuccess)
    {
        UE_LOG(LogTemp, Error, TEXT("Score submit failed: %s"), *ErrorMessage);
    }
});

UGameJoltBlueprintLibrary::SubmitScore(this, TEXT("5000 Points"), 5000, 0, OnComplete);
```

**Saving user data:**

```cpp
FOnDataStoreOpComplete OnComplete;
OnComplete.BindLambda([](bool bSuccess, const FString& ErrorMessage){});

UGameJoltBlueprintLibrary::SetUserData(this, TEXT("high_score"), TEXT("5000"), OnComplete);
```

> All C++ functions require a valid `WorldContextObject` as the first argument. Passing `this` from any `AActor` or `UUserWidget` subclass works.

---

## Notes

- `GJ Login` / `UGameJoltBlueprintLibrary::Login` must be called before any user-specific API calls (sessions, trophies, user data store).
- Sessions are automatically pinged every 30 seconds after `GJ Open Session`. Game Jolt closes sessions not pinged within 120 seconds.
- Guest scores do not require a logged-in user.
- Global data store operations do not require a logged-in user.
