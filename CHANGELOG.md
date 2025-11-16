# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [3.7.0] - 2025-11-16

### Added

- Added a `Get Level Room Data` node accessible from actors, even during their construction script, allowing the actor to setup itself when placed in the room.
- Added a new Data Asset called `Dungeon Settings` to allow overriding the `Room Unit` in the `Dungeon Generator` class.
- Added a compatibility list in `Door Type` assets, so different door types may be compatible
- Added `Try Place Room At Location` for custom dungeon algorithms to place a room without using a `Door Def` structure.
- Added a relevancy system for the rooms (independent from the room visibilities).
  - You can set a distance from which a room is considered relevant to the player.
  - A relevant room has a value to tell how close the player is to the room (the lower, the closest) with 0 the room where the player is in.
  - Some new nodes in the `Room` instances are available to access this relevancy level, and an event dispatcher to be notified when this relevancy changes.
- Added room constraints:
  - New base class `RoomConstraint` to create your own constraints.
  - Added built-in location and direction constraints.
  - Added new node `Does Pass All Constraints` to check wether a `Room Data` passes all its contraints.
- Added new node `Get Random Room Candidate` to ease the selection of a `RoomCandidate` struct, optionally using their scores as weights.

### Changed

- Replaced the hotfix for Steam multiplayer with a proper fix (this impact the saved dungeon, making it not compatible with previous plugin versions)
- Now the generation algorithm can be asynchronous:
  - Updated the `Dungeon Generator Base` class to allow spreading the `Create Dungeon` work on multiple frames.
  - Added a `Yield Generation` node to tell the generator to call the `Create Dungeon` again the next frame (must be used in the `Create Dungeon` function only).
  - Updated the `Create Dungeon` function in `Dungeon Generator` to limit number of room generated per frame (can be set with `Room Batch Size` in the actor's details).
- Room Visibility system now handles properly local multiplayer (splitscreens)
  - The `Get Visibility Pawn` overridable function now takes a `Player Controller` as input.
- `Filter and Sort Rooms` now checks if a room candidate passes all its contraints to include it in the output condidates.

### Fixed

- Fixed room visibilities to be updated properly when toggling or changing occlusion distance.

## [3.6.4] - 2025-09-30

### Fixed

- Fixed last compilation errors on Linux platforms

## [3.6.3] - 2025-09-18

### Fixed

- Fixed the saving/loading when the dungeon contains some null doors
- Removed a leftover warning message when creating the dungeon save system
- Hotfix for Steam multiplayer games (`Room Data` was garbage collected)
- Fixed a compilation error on Linux platforms

## [3.6.2] - 2025-08-31

### Fixed

- Fixed dungeon bounds rotation the proper way now (without breaking the `Room::GetBoundsExtents`)

## [3.6.1] - 2025-08-27

### Fixed

- Fixed debug dungeon bounds not properly rotated with a rotated dungeon.
- Fixed random crashes happening in editor or packaged game caused by a missing "Dungeon Graph" sub-object.
- Fixed `Count Connected Doors` node returning unconnected door count instead.

## [3.6.0] - 2025-06-09

### Added

- Custom dungeon generation algorithm now available in Blueprint too
- New `Filter And Sort Rooms` node that allows you to pick the best room and door index to fit in a specific place.
- Added several new blueprint nodes very useful when working with `Door Def` and `Vector Int`.
- Added a `Generation Success` event in the `Dungeon Generator` class, as opposite to the `Generation Failed` event.
- Internally introduced new cell-based type of room bounds, not fully used yet (only used for `Filter And Sort Rooms` at this time) but will allow great changes in the future.

### Changed

- Now the `Door Def` structure has read and write access in Blueprint, you can create them and update them by yourself.
- Now, `Room Connection` is accessible in Blueprint, no need to loop on room instances then door anymore.
- `Custom Room Data` instances are now created early in the generation process, allowing the use of them in the generation algorithm.

### Fixed

- Fixed a log message format when loading a dungeon from a save file.

## [3.5.1] - 2025-02-28

### Changed

- Updated the plugin description.

### Fixed

- Fixed crash on multiplayer when using subobject replication list (like in Lyra example project).

## [3.5.0] - 2025-01-28

### Added

- [EXPERIMENTAL] Added a save/load system for the dungeon, made to be easy to integrate in your save games.
- Added a `Deterministic Random` component to provide an easy access for an *independent* random stream in your actors, deterministic with the dungeon's seed.
- Added a `Simple Guid` component to easily provide a runtime guid for your actors, also compatible with the deterministic random component and the new experimental dungeon saving feature.
- Added a boolean in the `Dungeon Generator` to block interactions with the navigation system (no navmesh rebuild forced by the plugin).
- Added a boolean in the plugin settings to hide the debug drawing in game (display the debug drawings only in editor's viewport, not in PIE or packaged game)
- `Door Type` assets now have 2 new variables:
  - A color variable to customize the color of the door's debug box.
  - An offset variable to allow a different door offset per door type.
- Added some new nodes:
  - A `Number of Room Between` node taking the `Read Only Room` interface directly as inputs.
  - A `Get Room At` to retrieve a room at a certain dungeon cell.
  - A node to convert a `Door Direction` enum into a `Int Vector`.

### Changed

- You can now search the functions in the `Rooms` variable directly in the generator, the variable getter will be created and linked automatically.
- Door debug colors are not used anymore to display the status of the doors (valid/invalid, can/can't place) because their color can be customized.
- The room's debug drawings are now always displayed when in the editor's room mode.
- Added `Room Instance` in inputs of the `Choose Door` function (the room data input is now deprecated in favor to `Get Room Data` of the room instance)

### Deprecated

- Deprecated the `Get Random Stream` node because the generator's random stream is not updated when using this node. You will need to use the `Random Stream` variable getter instead.

### Fixed

- Fixed multiple issues with multiplayer synchronization (e.g. when clients reconnect, etc.).
- Fixed compatibility of door actors with the `Room Visibility` components. Doors can now be used in room levels without visibility issues.
- Fixed UE 4.27 circular dependency error when compiling/packaging the game with the plugin.
- Fixed crash in editor's room mode when removing the `Room Data` while the Door tool is active.

## [3.4.1] - 2024-11-21

### Fixed

- Fixed compilation issues in UE 5.5

## [3.4.0] - 2024-10-18

### Added

- Added a room count limit in plugin settings to prevent some infinite loops during the dungeon generation.
- Added various utility functions:
  - `Get Connected Room Index`, `Get Doors With` and `Get Room ID` in `Room` class.
  - `Get Path Between` and `NumberOfRoomBetween` in `Dungeon Graph` class (variable `Rooms` in the dungeon generator).
  - `Get Owning Room` and `Get Owning Room Custom Data` in a blueprint library (accessible from any actor/component placed in room levels).
  - `Get Compatible Doors`, `Has Door of Type`, `Has Custom Data`, `Get Size` and `Get Volume` in `Door Data` class.
- `Added Level Component` field in `Room Custom Data` to allow attaching a component automatically on the `Room Level` blueprint script.
- Added a `Force Visibility` node in `Room` class to force the visibility even when the player is not in it (useful for e.g. cutscenes).
- Added `Get Visibility Pawn` overridable function in the `Dungeon Generator` class to customize which pawn is used for the room visibility (useful for e.g. spectating another player).
- Split the `Dungeon Generator` into a `Dungeon Generator Base` class, allowing to create custom generation algorithms (only C++ as of now).
- Added a `Read Only Room` interface to allow access to some variables during the generation process.
  - Added an input `Current Room Instance` of type `Read Only Room` in `Choose Next Room`.
  - Added an input `Room Instance` of type `Read Only Room` in `On Room Added`.
- Added a node Discard Room to explicitly tell we don't want to place a room in `Choose Next Room` (prevent error when returning null)
- Added `Auto Discard Room If Null` variable in `Dungeon Generator`

### Fixed

- Fixed player's components triggering room visibility even when ignoring the `Room Object Type`.

## [3.3.1] - 2024-07-18

### Fixed

- Fixed random crash on clients when unloading the dungeon.
- Fixed dungeon generator's begin initialization state triggered twice (missing break).
- Added back error logs on screen even when on screen debug is disabled in plugin.
- Now support push based replication and registered subobject lists (now compatible with Lyra example project + maybe compatible with Iris network subsystem).
- Fixed Door Tool in the Dungeon Room editor mode, to allow door removal and prevent door overlap when Room Unit is smaller than Door Size.
- Fixed debug cross of locked rooms not properly rotated when the room is rotated.

## [3.3.0] - 2024-06-23

### Added

- Added a blueprint node to select a random room with weights (non-uniform random).
- Added dungeon limits for room placement. If set, rooms will not be placed outside of the limits.
- Added a `Static Room Visibility` component with reduced computation over the non-static one (used for directly placed actors in room levels that will never go out of it at runtime).
- A tag `Ignore Room Culling` can be added to directly placed actors in room levels so they will be ignored by the room level automatic occlusion (before, only replicated actors were ignored). You can use a  `(Static) Room Visibility` component to manage their visibility like you want instead.
- Added a `Get Progress` function in `Dungeon Generator` class to keep track of generation progress and make progress bars easily.
- Added an `On Fail To Add Room` event, called after exhausted all tries of placing a room without success.
- Added a `Cleanup Room` function in `Room Data` class as a counterpart of the `Initialize Room` one.
- Can now tweak the max generation tries and max room placement tries in the plugin's settings.
- Added accessor to generator's `Rooms` variable in C++ outside of child classes.
- The door debug box (blue) will now turn orange if multiple doors are on the same place.
- Now the `RoomData`'s level will be automatically filled if empty when the data is selected in the Dungeon Room Editor mode.

### Fixed

- Crash when running in new PIE window in 'Play as Client' when a DungeonGenerator actor is in the launched level.
- Fixed missing call to `On Room Added` event for the first room.
- Fixed `RootComponent` of door actors to be able to edit them in editor.
- Fixed door actors so that they always spawn, regardless of the actor's parameter.

## [3.2.2] - 2024-05-17

### Fixed

- Fixed warning `AddReferencedObject` for UE 5.3 and newer.
- Fixed UE 5.4 compilation issues

## [3.2.1] - 2024-05-13

### Fixed

- Fixed warning `TriggerType` name conflicting with the `Enhanced Input` system.
- Fixed compilation error due to un-initialized `FMargin3D` members.
- Fixed UE 5.4 PIE issues

## [3.2.0] - 2024-04-07

### Added

- Added console commands to temporarily tweak some plugin settings during runtime (even in packaged development game) and to generate/unload dungeons.
- Added blueprint nodes to access most of the plugin settings, or to change `Occlusion Culling` and `Occlusion Distance` values.
- Added a `Flipped` output in `Choose Door` function to choose which room the door is facing.
- Added `On Actor Enter Room` and `On Actor Exit Room` delegates in the `Room Level` blueprint.
- Added `Room Observer` components that automatically binds on those new delegates.
- Added some functions to the `Room` instances to know if doors are connected or to get connected `Room` instances.
- Added room bounds center and extent blueprint accessors in `Room` instances and `Room Level`.
- Added `Can Loop` boolean in `Dungeon Generator` to be able to toggle it per-actor.
- Added optional world meshes collision checks before placing a room during the generation process.
- Added a plugin setting to change the room trigger collision type.
- Added plugin settings to customize the door's debug arrow.
- Added door deletion in `Dungeon Room` editor mode when right-clicking on an existing door with the `Door` tool.
- Added utility buttons in the `Dungeon Room` editor mode to make selected box volumes fit in the room bounds, and to delete all invalid doors.
- Added an optional camera rotation pivot at the center of the room (when `Orbit Around Actor Selection` is enabled in your Editor Preferences).

### Fixed

- Fixed undo/redo in `Dungeon Room` editor mode.
- Fixed the widget mode (translate, rotate, scale) while in the `Dungeon Room` editor mode to have a better actor modification while staying in this mode.
- Fixed missing includes in C++ causing sometimes compilation issues.

### Deprecated

- Deprecated `Can Loop` in the plugin's settings. It will be removed in a future version. Leave it ticked and use the one in the `Dungeon Generator` actor instead.

## [3.1.2] - 2024-03-06

### Fixed

- Fixed crash when compiling room level blueprint after exiting the room editor mode.
- Fixed crash at runtime when the room data reference in the room level and the level reference in room data do not match.
- Fixed debug drawings of rooms and doors not displayed in packaged development game.
- Fixed room visibility not updated when occlusion culling settings are changed at runtime in packaged game.
- Fixed compilation errors caused by missing includes that could happen sometimes when using unity build.

## [3.1.1] - 2024-02-17

### Fixed

- Workaround for the occlusion of replicated actors placed directly in a room level.
- They are now ignored by the room level, so you can add the `Room Visibility Component` on them to manage their visibility with the rooms.
- Fixed door actors wrong rotation (flipped on Y axis).
- Fixed room visibilities not updated during PIE when toggling on/off the occlusion culling or changing the occlusion distance.
- Clamp occlusion distance to 1 to get at least a visible room (the one the player is in).

## [3.1.0] - 2024-01-28

### Added

- Added an `Unload` function in the `Dungeon Generator` actor.
- Added `Door Index` output in `Choose Next Room` function. [#39]
- Added `Room Visitor` interface to create custom behaviors on actors and components when an actor enters/exits a room.
- Added door accessors from the room instances in blueprint.
- Added accessor to the `Dungeon Generator`'s random stream from room instances in blueprint.

### Fixed

- Replaced the custom `ProceduralLevelStreaming` class with the `LevelStreamingDynamic` using the custom instance name.
- This should be transparent for the plugin's users, but the level instances should now have recent Unreal Engine's modifications on them (>UE4.26).
- Improved network performances with the use of `Net Dormancy` to notify the Unreal's network system only when there are changes in the dungeon or door state.
- Updated the `Room Visibility Component` to use the new `Room Visitor` interface (should be transparent for plugin's users).

## [3.0.1] - 2023-10-24

### Fixed

- Fixed UE 5.3 compilation issues [#35]

## [3.0.0] - 2023-10-14

### Added

- New editor mode to ease room creation.
- New asset category to speed up data asset creation.
- Added a door type system to allow various door sizes in the dungeon [#21] .
- Added per room instance custom data.
- Added an initialization step in the dungeon generator for the room instances to be initialized before `IsValidDungeon` being called.
- Added a function to check if the player can reach one room from another (no locked room between them) [#25].
- Added a `Spectate` blueprint node because it is not in the base unreal engine.

### Changed

- Moved room handling in a subclass of the `DungeonGenerator`, you can access most of the previous functions from the Rooms variable of the dungeon generator now.
- Reworked the dungeon networking: it does no longer use an RPC to generate the dungeon on all clients. Now the server generates the dungeon and replicates all rooms to the clients. The dungeon now uses a state machine to handle the generation process, allowing late joining clients to load properly the dungeon.
- Room bounds can now be in negative too.
- Room's "origin" (magenta sphere) is no longer shown since not really useful and a little confusing (can be enabled via a plugin's setting)
- New plugin icon.

### Fixed

- Fixed wrong seed value when the dungeon' seed is set to `Auto Increment`.
- Fixed `ContinueToAddRoom` not called with each `ChooseNextRoomData`.

## [2.1.2] - 2023-07-18

### Added

- Added an error message and the `GenerationFailed` event when the dungeon generator didn't generated a valid dungeon after exhausting all the retries (#29). Also now the dungeon will not spawn anymore in that case.

### Fixed

- Fixed occlusion culling system not disabled by the plugin's setting.
- Fixed compilation issues (#26).
- Fixed room triggers returned by "trace by channel" functions (#27).

## [2.1.1] - 2023-03-26

### Added

- Added `RoomData` assets validation to notify users when something is not correctly setup (levels and doors).

### Changed

- Make doors replicating over network, and thus usable in multiplayer games.

### Fixed

- Improved a little the `TriggerDoor` class with a native event to check which actors can trigger the door.
- Deprecated the `OpenDoor` and `CloseDoor` functions in favor to a unique `Open(bool)` function.
- Various fixes.

## [2.1.0] - 2023-03-11

### Added

- Added Dynamic actors occlusion culling
- Added visibility distance to occlusion culling system for room visibility

### Changed

- Greatly improved occlusion culling performance (using Octree instead of box overlapping)
- Improved debug drawings (draw a cross for not connected doors instead of arrows, draw current player's room in green instead of red)

### Deprecated

- Deprecated `RoomLockerBase` and exposed the locking state of rooms in room's level blueprints and door actors.

## [2.0.2] - 2022-09-11

### Added

- Added a DungeonGenerator parameter to place the dungeon's rooms on the actor location and rotation (default disabled)

### Fixed

- Removed static streaming level id, allowing multiple DungeonGenerator actors generating at the same time.
- Fixed blue bounding box for doors showing the offset in the door blueprint preview.

## [2.0.1] - 2021-11-13

### Added

- Added accessors in blueprint to get and set the seed
- Added back some utility functions to count and check existence with data types (in addition to data instances)

## [2.0.0] - 2021-11-01

### Changed

- Now use RoomData instances instead of blueprint classes

## [1.2.2] - 2021-08-19

### Changed

- Updated the year in the LICENSE file, and added the license notice in code files.

### Fixed

- Fixed crashes when returning none in ChooseFirstRoomData or ChooseNextRoomData.

## [1.2.1] - 2021-04-14

### Fixed

- Fixed packaging errors when installing the plugin in the engine. (now, blueprint only project can use the plugin)

## [1.2.0] - 2021-03-25

### Added

- Added an option to allow room loop in the dungeon.

### Fixed

- Fixed multiple instances of door at one door place.

## [1.1.0] - 2021-02-21

### Added

- Added the `Depth First` and `Breadth First` generation types.
- Added option to display the generation logs on screen too.

### Changed

- Refactored the DungeonGenerator code to allow override of functions in C++ as well.

## [1.0.3] - 2021-02-16

### Fixed

- Fixed a crash caused by a conflict between GC and the C++ native delete.
- Fixed a possible crash when too many rooms are generated (avoid a stack overflow from recursivity call to AddRoom)

## [1.0.2] - 2021-01-11

### Fixed

- Fixed enum EDirection ambiguity with RigLogic plugin.

## [1.0.1] - 2019-09-10

### Added

- Added Draw Debug toggle in the plugin settings.

### Fixed

- Fixed some bugs.

## [1.0.0] - 2019-05-30

### Added

- Initial Release

[3.7.0]: https://github.com/BenPyton/ProceduralDungeon/compare/v3.6.4...v3.7.0
[3.6.4]: https://github.com/BenPyton/ProceduralDungeon/compare/v3.6.3...v3.6.4
[3.6.3]: https://github.com/BenPyton/ProceduralDungeon/compare/v3.6.2...v3.6.3
[3.6.2]: https://github.com/BenPyton/ProceduralDungeon/compare/v3.6.1...v3.6.2
[3.6.1]: https://github.com/BenPyton/ProceduralDungeon/compare/v3.6.0...v3.6.1
[3.6.0]: https://github.com/BenPyton/ProceduralDungeon/compare/v3.5.1...v3.6.0
[3.5.1]: https://github.com/BenPyton/ProceduralDungeon/compare/v3.5.0...v3.5.1
[3.5.0]: https://github.com/BenPyton/ProceduralDungeon/compare/v3.4.1...v3.5.0
[3.4.1]: https://github.com/BenPyton/ProceduralDungeon/compare/v3.4.0...v3.4.1
[3.4.0]: https://github.com/BenPyton/ProceduralDungeon/compare/v3.3.1...v3.4.0
[3.3.1]: https://github.com/BenPyton/ProceduralDungeon/compare/v3.3.0...v3.3.1
[3.3.0]: https://github.com/BenPyton/ProceduralDungeon/compare/v3.2.2...v3.3.0
[3.2.2]: https://github.com/BenPyton/ProceduralDungeon/compare/v3.2.1...v3.2.2
[3.2.1]: https://github.com/BenPyton/ProceduralDungeon/compare/v3.2.0...v3.2.1
[3.2.0]: https://github.com/BenPyton/ProceduralDungeon/compare/v3.1.2...v3.2.0
[3.1.2]: https://github.com/BenPyton/ProceduralDungeon/compare/v3.1.1...v3.1.2
[3.1.1]: https://github.com/BenPyton/ProceduralDungeon/compare/v3.1.0...v3.1.1
[3.1.0]: https://github.com/BenPyton/ProceduralDungeon/compare/v3.0.1...v3.1.0
[3.0.1]: https://github.com/BenPyton/ProceduralDungeon/compare/v3.0.0...v3.0.1
[3.0.0]: https://github.com/BenPyton/ProceduralDungeon/compare/v2.1.2...v3.0.0
[2.1.2]: https://github.com/BenPyton/ProceduralDungeon/compare/v2.1.1...v2.1.2
[2.1.1]: https://github.com/BenPyton/ProceduralDungeon/compare/v2.1.0...v2.1.1
[2.1.0]: https://github.com/BenPyton/ProceduralDungeon/compare/v2.0.2...v2.1.0
[2.0.2]: https://github.com/BenPyton/ProceduralDungeon/compare/v2.0.1...v2.0.2
[2.0.1]: https://github.com/BenPyton/ProceduralDungeon/compare/v2.0.0...v2.0.1
[2.0.0]: https://github.com/BenPyton/ProceduralDungeon/compare/v1.2.2...v2.0.0
[1.2.2]: https://github.com/BenPyton/ProceduralDungeon/compare/v1.2.1...v1.2.2
[1.2.1]: https://github.com/BenPyton/ProceduralDungeon/compare/v1.2.0...v1.2.1
[1.2.0]: https://github.com/BenPyton/ProceduralDungeon/compare/v1.1.0...v1.2.0
[1.1.0]: https://github.com/BenPyton/ProceduralDungeon/compare/v1.0.3...v1.1.0
[1.0.3]: https://github.com/BenPyton/ProceduralDungeon/compare/v1.0.2...v1.0.3
[1.0.2]: https://github.com/BenPyton/ProceduralDungeon/compare/v1.0.1...v1.0.2
[1.0.1]: https://github.com/BenPyton/ProceduralDungeon/compare/v1.0.0...v1.0.1
[1.0.0]: https://github.com/BenPyton/ProceduralDungeon/releases/tag/v1.0.0
