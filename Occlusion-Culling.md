# Occlusion Culling

The occlusion culling system of this plugin consists of showing only the necessary rooms to avoid a lots of mesh drawing and thus improving performances.


:::warning
Only the visibility of meshes is toggled. The ticking, animations, collisions, etc. are kept! So, the possible performance gain is on the rendering only!
:::

You can toggle this system on or off in the [Plugin Settings](Plugin-Settings.md) alongside some other occlusion settings:
- The `occlusion distance` determine how many rooms are visible from the player's room. 
- `Occlude dynamic actors` allows dynamic actors spawned after the dungeon generation and/or moving between rooms to be occluded with the rooms by the plugin. You'll also have to put a `Room Visibility Component` on those actors.

## How the occlusion system works

The dungeon generator actor will keep track of the local player (`GetPlayerController(0)`) to know in which room it is located (referred later as the `Current Player Room`).

Then, the generator actor will toggle the visibility state of the `Current Player Room` and its directly connected rooms, and their own directly connected rooms, etc. based on the `Occlusion Distance` in the [Plugin Settings](Plugin-Settings.md) (`1` is only the `Current Player Room`, `2` is its direct connected rooms, etc.).

Each room will then toggle the visibility of their owned actors (the ones placed directly in their level).\
If an actor is spawned during runtime, it will not belongs to any room level, and so their visibility will not be toggled.\
If an actor can move between rooms during the gameplay, you should not place it directly in the room level, as it will still belong to it and be occluded with that room instead of the one the actor is located in.

If you want to have actors that can travel across rooms, you should create a spawner actor placed in the room levels, which will spawn your "dynamic" actor that will then be not attached to a specific room.

However, if you want this dynamic actor to be occluded properly with the rooms, you have to add a `Room Visibility Component` on it (see below for more details on it).

:::warning
[Geometry Brushes](Geometry-Brushes.md) can't have there visibility toggled, and so can't be hidden by the occlusion culling system.
You should convert them into static meshes in order to make them occluded correctly with the rooms.
:::

## Room Visibility Component

This actor component allows the actor to be tracked by the dungeon's rooms and so be occluded with them.

You need to toggle on `Occlude Dynamic Actors` in the [Plugin Settings](Plugin-Settings.md) in order to make it working.

What is available from this component:
- `Is Visible` function returns true if the actor is inside a visible room.
- `Set Visibility` allows you to set the visibility mode
	- `Same As Room` will toggle actor's visibility based on `Is Visible` value.
	- `Force Hidden`and `Force Visible` will turn visibility on/off regardless the `Is Visible` value.
	- `Custom` will does nothing itself and lets you manage the actor's visibility. You can then use `Is Visible` and/or your own logic to toggle actor's visibility.
- `Get Visibility` will returns the current visibility mode set by `Set Visibility`.
- `On Room Visibility Changed` is a delegate called when the `Is Visible` value changes. Useful for example when you have a `Custom` visibility and you are changing your actor visibility by events instead of in `Tick`.
