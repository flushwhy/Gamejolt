# Fab listing copy (draft)

Use this as editable listing copy, not as a claim that the package has passed Fab review. Update the version and tested-engine list to match the actual upload.

## Title

**Game Jolt API for Unreal**

## Short description

Integrate Game Jolt authentication, sessions, trophies, leaderboards, and cloud data into Unreal Engine projects with Blueprint and C++ APIs.

## Full description

Connect your Unreal Engine game to the Game Jolt API using a plugin designed for both Blueprint workflows and native C++ projects.

### Features

- **Authentication:** log in a player using their Game Jolt username and user token.
- **Sessions:** open and close sessions and keep active sessions pinged.
- **Trophies:** fetch trophy information for authenticated players.
- **Leaderboards:** submit player or guest scores, fetch scores, and look up score ranks.
- **Cloud data:** read and write user-scoped or global data.
- **Blueprint and C++:** call plugin operations through Blueprint nodes or C++ functions.

### Requirements

- A C++ or C++/Blueprint Unreal Engine project. Pure Blueprint-only projects cannot load this C++ plugin.
- Game Jolt game API credentials configured in Project Settings.
- Internet access for API requests.

### Compatibility

Intended target range: Unreal Engine 4.20–4.27 and Unreal Engine 5.0–5.8. See the documentation and release notes for the exact versions verified for each release. Do not claim every minor version is tested unless it has been built and smoke-tested.

### Documentation and support

- Documentation: https://flushwhy.github.io/Gamejolt/
- Source and release notes: https://github.com/flushwhy/Gamejolt
- Bug reports: https://github.com/flushwhy/Gamejolt/issues

## Suggested tags

Unreal Engine, Game Jolt, API, Blueprint, C++, Authentication, Leaderboards, Trophies, Cloud Save, Online Services

## Media to prepare

Use genuine screenshots from the current plugin build:
1. Plugin enabled in Unreal Editor's Plugins window.
2. Game Jolt API settings page with all credentials obscured.
3. Blueprint graph showing login and its success/failure handling.
4. Blueprint graph or in-game view demonstrating a score or trophy operation.
5. Optional short video showing a working integration.

Do not include real user tokens, private keys, misleading compatibility badges, or third-party logos/artwork without the necessary rights.
