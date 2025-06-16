# Changelog

All notable changes to this project will be documented in this file...

## [Next Release]

### Added
- 


### Changed
 
- Changed build settings to be compatible with 5.0 UE.

### Removed

### Fixed

## [1.0.1] - 2025-06-16

### Added
- User, Session, Score, Trophy, and DataStore Managers in place of the old system.
- Comments / Documentation that show in both BP and Cpp
- This Changelog

### Changed
- Moved from base architecture to a UE subsystem based architecture
- Renamed most of the core to align with UE standards
- Reworked some parts to get back into compatibility with 5.0 to 5.6

### Removed
- UGJUserData, GameJolt, and GJUser cpp and header files. They were replaced with User, DataStore, and other Managers.

### Fixed
- Some functions not showing in BPs
- Fixed signature gen issues, and handled errors in a better way

## [1.0.0] - 2025-06-12

### Added 
- The base of the plugin.

---
[1.0.1]: https://github.com/flushwhy/Game-Jolt-Plugin-Docs/releases/tag/V1.0.1
[1.0.0]: https://github.com/flushwhy/Game-Jolt-Plugin-Docs/releases/tag/V1