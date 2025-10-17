# GameJolt Plugin for Unreal Engine

This plugin simplifies the process of integrating Game Jolt features into your Unreal Engine projects, with support for both C++ and Blueprints.

---

## Features

- **User Authentication**: Securely log in players with their Game Jolt credentials.
- **Session Management**: Open, ping, and close game sessions to track when users are playing.
- **Trophies**: Fetch trophy information and award trophies to players.
- **Scores**: Submit scores to leaderboards and fetch high scores.
- **Cloud Data Storage**: Save and load game data to the cloud, either globally or per-user.
- **Server Time**: Fetch the official server time.
- **Friends List**: Get a list of a user's friends.

---

## Installation

1. Go to the Releases page of this repository.
2. Download the latest `.zip` file.
3. Find your Unreal Engine project's root directory.
4. Create a folder named `Plugins` if it doesn't already exist.
5. Extract the `GameJolt` folder from the downloaded `.zip` file into your `Plugins` folder. The final path should look like `MyProject/Plugins/GameJolt/`.
6. Right-click on your `.uproject` file and select **"Generate Visual Studio project files"**.
7. Open your project. Unreal Engine should prompt you that a new plugin has been found and ask to build it. Click **"Yes"**.

---

## Setup

Before you can use the API, you must provide your game's credentials:

1. Open your Unreal Engine project.
2. Navigate to **Edit -> Project Settings**.
3. In the left-hand panel, scroll down to the **Plugins** section and click on **Game Jolt API**.
4. Enter your **Game ID** and **Private Key**. You can find these on your game's dashboard on the Game Jolt website under **Game API -> API Settings**.

---

## Usage

### Blueprint Example: User Authentication

1. **Get the Subsystem and Manager**:
   - Drag off the **GameInstance** node and call `Get GameJoltSubsystem`.
   - From the `GameJoltSubsystem`, call `GetUserManager`.

2. **Call the Function and Bind the Event**:
   - Drag off the `UserManager` node and call `AuthenticateUser`.
   - Provide the `Username` and `UserToken` inputs.
   - Bind a custom event to the `OnComplete` delegate.

3. **Handle the Result**:
   - In the custom event, check the `bSuccess` output.
   - If `bSuccess` is `true`, use the `User` output to display the authenticated user's details (e.g., username).
   - If `bSuccess` is `false`, display the `ErrorMessage`.

---

### C++ Example: User Authentication

The C++ workflow is similar to the Blueprint workflow. Here's an example:
```c
#include "GameJoltSubsystem.h"
#include "UGameJoltUserManager.h"

void AMyPlayerController::AuthenticatePlayer(const FString& Username, const FString& UserToken)
{
// Get the subsystem from the Game Instance
UGameInstance* GameInstance = GetGameInstance();
if (!GameInstance) return;

    UGameJoltSubsystem* GameJoltSubsystem = GameInstance->GetSubsystem<UGameJoltSubsystem>();
    if (!GameJoltSubsystem || !GameJoltSubsystem->UserManager) return;

    // Create the delegate and bind a Lambda function to it
    FOnAuthUserComplete OnCompleteDelegate;
    OnCompleteDelegate.BindLambda([](bool bSuccess, const FGameJoltUser& User, const FString& ErrorMessage)
    {
        if (bSuccess)
        {
            UE_LOG(LogTemp, Log, TEXT("Successfully authenticated user: %s"), *User.username);
            // Now you can open a session, fetch trophies, etc.
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Game Jolt authentication failed: %s"), *ErrorMessage);
        }
    });

    // Call the API function
    GameJoltSubsystem->UserManager->AuthenticateUser(OnCompleteDelegate, Username, UserToken);
} 
```