# GameJolt

---

This is a plugin for [UE](https://www.unrealengine.com/) that allows you to use GameJolt in your project.

This is my first Plugin for UE, and I am slowly developing it. After I get most of the coverage done I will put in on the
Fab Store. If you have any suggestions or find any bugs, please let me know.

This plugin simplifies the process of integrating Game Jolt features into your Unreal Engine projects, with support for both C++ and Blueprints.

## Features

---

User Authentication: Securely log in players with their Game Jolt credentials.

Session Management: Open, ping, and close game sessions to track when users are playing.

Trophies: Fetch trophy information and award trophies to players.

Scores: Submit scores to leaderboards and fetch high scores.

Cloud Data Storage: Save and load game data to the cloud, either globally or per-user.

Server Time: Fetch the official server time.

Friends List: Get a list of a user's friends.

## Installation

---

Go to the Releases page of this repository.

1. Download the latest .zip file.


2. Find your Unreal Engine project's root directory.


3. Create a folder named Plugins if it doesn't already exist.


4. Extract the GameJolt folder from the downloaded .zip file into your Plugins folder. The final path should look like MyProject/Plugins/GameJolt/.


5. Right-click on your .uproject file and select "Generate Visual Studio project files".


6. Open your project. Unreal Engine should prompt you that a new plugin has been found and ask to build it. Click "Yes".

## Setup

---

Before you can use the API, you must provide your game's credentials.

In the Unreal Editor, navigate to Edit -> Project Settings.

In the left-hand panel, scroll down to the Plugins section and click on Game Jolt API.

Enter your Game ID and Private Key. You can find these on your game's dashboard on the Game Jolt website under Game API -> API Settings.

## Usage

---

All API functionality is accessed through the Game Jolt Subsystem. From the subsystem, you can get specific managers (Trophy Manager, Score Manager, etc.) to call their functions.

### Blueprint Usage Example

Here is a common example of how to authenticate a user when they click a button.

Get the Subsystem and Manager
First, get the Game Jolt Subsystem, and from it, get the User Manager.

Call the Function and Bind the Event
The API functions are asynchronous, meaning they don't return a value immediately. Instead, they take a delegate (the On Complete pin) that gets called when the web request finishes.

Drag off the On Complete pin and select Add Event -> Add Custom Event.

This creates a new red event node that will execute when the API call is done. Give it a descriptive name like OnUserAuthComplete.

Handle the Result
The Custom Event will have output pins corresponding to the delegate's parameters (bSuccess, User, ErrorMessage). You can now use these to handle the result of the API call. Always check the bSuccess boolean first!

---

### C++ Usage Example

The C++ workflow is very similar. You get the subsystem, get the manager, and call the function, providing a lambda to handle the callback.
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