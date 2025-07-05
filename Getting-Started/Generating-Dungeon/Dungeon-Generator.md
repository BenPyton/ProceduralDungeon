---
pagination_prev: guides/Getting-Started/Creating-Rooms
pagination_next: guides/Getting-Started/Generating-Dungeon/Choose-First-Room-Data
---

# Dungeon Generator

import Flowchart from "../../Images/Flowchart_Dark_v4.svg";

## Create a dungeon generator

The dungeon generator is an actor placed in your main level. It is responsible to generate the dungeon and spawn the room levels.\
To create one, create a new blueprint, expand "All Classes" and type `DungeonGenerator`.

The dungeon generator don't work directly by itself, you have to [create data and levels for your rooms](../Creating-Rooms.md) first.

To be able to generate a dungeon, you also have to override some minimal functions:

- [Choose First Room Data](Choose-First-Room-Data.md)
- [Choose Next Room Data](Choose-Next-Room-Data.md)
- [Continue To Add Room](Continue-To-Add-Room.md)
- [Is Valid Dungeon](Is-Valid-Dungeon.md)
- [Choose Door](Choose-Door.md)

Some other functions can also be overridden:

Function | Description
--- | ---
**Pre&nbsp;Generation** | Called before anything else when the generator is about to generate a new dungeon.<br/>You can use it to display a loading screen for example.
**Post&nbsp;Generation** | Called after loading and initializing all rooms of the dungeon.<br/>Use it to spawn or teleport players in the first room.
**Generation&nbsp;Init** | Called each time the generation start over when `Is Valid Dungeon` returns false.<br/>Used in most cases to reset all your custom generation variables.
**Generation&nbsp;Failed** | Called when `Is Valid Dungeon` returned false after too many generation attempts. No dungeon is generated when this is called.
**On&nbsp;Room&nbsp;Added** | Called when a room is successfully added to the dungeon (but not spawned yet). The added room is destroyed when `Is Valid Dungeon` returns false (you may have to use *GenerationInit* to reset variables changed from this call).
**Initialize&nbsp;Dungeon** | Called after `Continue To Add Room` returned false. Allows you to initialize room instances before validating the dungeon with `Is Valid Dungeon` (useful if you want for example locking some rooms and putting keys in other ones).

## Understanding the internals of the Dungeon Generator

<Flowchart/>

This flowchart shows how all functions are pieced together.\
A generation is made of 4 phases :

- **`Idle`**: the generator just wait until a change occurs.
- **`Unload`**: the previous room levels are unloading (destroying level's actors, etc.)
- **`Create Dungeon`**: (*server only*) This is the critical part of the code. This function creates virtually the dungeon (meaning the room levels are not yet instantiated). Your generation rules are defined here.
- **`Load`**: the room levels are loading and initializing (spawning actors, etc. and call to `BeginPlay` of room's level blueprints)

The `Create Dungeon` function is the heart of the generation algorithm.\
The flowchart of this function shown above is the default behavior: starting with a first room then adding rooms to existing ones through unconnected doors.\
If you have some needs that could not be fulfilled with this approach, you can [write your own custom `Create Dungeon` function](../../Advanced-Features/Custom-Create-Dungeon.md).

## Useful functions of the Dungeon Generator

Aside the internal events you can override, there are some useful other functions the `Dungeon Generator` provides to you.\
There are also the `Generate` and the `Unload` functions.

Take a look at the [API Documentation](https://benpyton.github.io/ProceduralDungeon/api/Classes/DungeonGenerator#nodes) for a list of all functions provided by the `Dungeon Generator`.

## The `Rooms` variable, at the heart of the plugin

The `Rooms` variable of the `Dungeon Generator` contains the list of all generated room instances.\
You can search for specific room instances, count them, check a path exists between rooms, etc.
Take a look at the [API Documentation](https://benpyton.github.io/ProceduralDungeon/api/Classes/DungeonGraph#nodes) for a list of all functions provided by the `Rooms` variable.

## Generator settings

The `Dungeon Generator` actor has some parameters grouped inside the `Procedural Generation` category:

![](../../Images/GeneratorSettings.jpg)

#### `Generation Type`

- The `depth first` will prioritize the last added room to continue the generation.
This will result in a dungeon more linear with shorter dead ends.
- The `breadth first` will prioritize the first added room to continue the generation. This will result in a more spread dungeon.

This is an example using the same very basic generation rules defined in this wiki. We just set a counter of 20 rooms to add the end room. So with depth first we have a lot of rooms to reach the end but dead-ends of only 1 room each. And with the breadth first, we have less rooms until the end, but the dead-ends are the same length of the path to reach the end (some dead-ends are shorter because the algorithm can't place a room after).

![](../../Images/DepthBreadth.jpg)

#### `Can Loop`

If checked, when a new room is placed in the dungeon, any door aligned with another door of other rooms will be connected.\
If unchecked, only the door from the previous room (the one used to generate this room) will be connected.\
*This parameter is only used when `Can Loop` from the project-wide [settings](../Plugin-Settings.md) is also checked.*

#### `Dungeon Limits`

This is the bounding limits of the dungeon. If a limit is defined, a room will be considered as not placeable if its bounding box  is outside of this limit.

#### `Use Generator Transform`

If checked, the dungeon will be offset and rotated using the dungeon generator actor's transform.\
If unchecked, the first room will be placed at world's (0,0,0) and no rotation will be applied.

#### `Seed Type`

- `Random`: at each generation, the seed is randomly chosen.
- `Auto Increment`: the initial seed is used the first time, then a fixed number will be added at each generation. This will produce different dungeon at each generation, but you will have the same generation sequence each time you play.
- `Fixed`: the initial seed is used each time you generate, so you will always get the same dungeon.

#### `Seed`

The initial seed you want to use if the `Seed Type` is not `Random`

#### `Seed Increment`

The number used to increment the seed if the `Seed Type` is `Auto Increment`

#### `Auto Discard Room If Null`

When checked, if the `Choose Next Room` returns null, the generator will not consider anymore placing a room at the current door, and switch directly to the next door, without throwing errors.

#### `Use World Collision Checks`

When checked, the dungeon generator will test a box overlapping with world static actors in order to allow or not a room to be generated.\
This can be CPU-intensive and should be used only when necessary. Though, it is only used during the generation process, not during gameplay.

#### `Use Json Save`

> :warning: *This is only available for editor and dev builds!*

When checked, the dungeon save system will serialize as json format instead of binary.  

#### `Draw Debug Dungeon Bounds`

Toggle the draw of the whole dungeon bounding box in yellow.

#### `Rebuild Navmesh`

Toggle the force rebuild of the navmesh at the end of the dungeon generation.
