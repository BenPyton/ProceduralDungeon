# Procedural Dungeon Plugin

[![Plugin version number](https://img.shields.io/github/v/release/BenPyton/ProceduralDungeon?label=Version)](https://github.com/BenPyton/ProceduralDungeon/releases/latest)
[![Unreal Engine Supported Versions](https://img.shields.io/badge/Unreal_Engine-4.27_%7C_5.1_%7C_5.2_%7C_5.3_%7C_5.4_%7C_5.5-9455CE?logo=unrealengine)](https://github.com/BenPyton/ProceduralDungeon/releases)
[![License](https://img.shields.io/github/license/BenPyton/ProceduralDungeon?label=License&color=blue)](LICENSE)
![Download count](https://img.shields.io/github/downloads/BenPyton/ProceduralDungeon/total?label=Downloads)
[![Actively Maintained](https://img.shields.io/badge/Maintenance%20Level-Actively%20Maintained-green.svg)](https://gist.github.com/cheerfulstoic/d107229326a01ff0f333a1d3476e068d)
[![Discord](https://img.shields.io/discord/1182461404092055574?logo=discord&logoColor=white&label=Discord&color=%235865F2)][1]

## Overview

This is an Unreal Engine plugin to create procedural dungeons by arranging procedurally a set of room like "The Binding of Isaac" or "Rogue Legacy" but in 3D.

This procedural dungeon plugin uses hand-created rooms that are Unreal levels.\
You define your own rules in blueprints or C++ to generate the dungeon via an actor placed in a master world.

If you have any bug or crash, please open an issue in the Github repo.\
If you have suggestions, questions or need help to use the plugin you can join the [Discord server][1] dedicated to this plugin.\
If you want to contribute, feel free to create a pull request (*contributions to the wiki are also welcomed!*).

## Features

- Hand created rooms, allowing full control by the level designers.
- Generation rules defined in blueprint, allowing flexible and powerful procedural generation.
- Doors can be put on any Z height, allowing dungeons to be in 3D (but can do flat dungeons as well).
- Defining different door types, allowing more complex dungeons.
- Rooms and doors can be locked/unlocked. 
- Optional occlusion culling, allowing only relevant rooms rendered (and thus better performances).
- A new editor mode to ease the creation and edition of the rooms.
- Custom data in rooms, allowing gameplay specific code in room instances.
- Working well with navmeshes and multiplayers.

## Example

Some hand-made rooms defined with bounds and doors:\
<img src="https://github.com/BenPyton/ProceduralDungeon/wiki/Images/ProceduralDungeonDemo_RoomSpawn.gif" alt="Animated GIF" width="125"/>
<img src="https://github.com/BenPyton/ProceduralDungeon/wiki/Images/ProceduralDungeonDemo_RoomA.gif" alt="Animated GIF" width="125"/>
<img src="https://github.com/BenPyton/ProceduralDungeon/wiki/Images/ProceduralDungeonDemo_RoomB.gif" alt="Animated GIF" width="125"/>
<img src="https://github.com/BenPyton/ProceduralDungeon/wiki/Images/ProceduralDungeonDemo_RoomC.gif" alt="Animated GIF" width="125"/>
<img src="https://github.com/BenPyton/ProceduralDungeon/wiki/Images/ProceduralDungeonDemo_RoomD.gif" alt="Animated GIF" width="125"/>
<img src="https://github.com/BenPyton/ProceduralDungeon/wiki/Images/ProceduralDungeonDemo_RoomExit.gif" alt="Animated GIF" width="125"/>

Some very simple generation rules:
- A special room (red) used to spawn the player.
- Then 10 rooms chosen randomly in a list (blue, green, yellow, cyan).
- Then a special room (purple) used as an end goal for the player.
- Finally 10 other rooms chosen randomly.

Possible results:\
<img src="https://github.com/BenPyton/ProceduralDungeon/wiki/Images/ProceduralDungeonResult.gif" alt="Animated GIF" width="400"/>
<img src="https://github.com/BenPyton/ProceduralDungeon/wiki/Images/ProceduralDungeonResult2.gif" alt="Animated GIF" width="400"/>

[Demo Video on Youtube](http://www.youtube.com/watch?v=DmyNEd0YtDE "Procedural Dungeon Demo")<br>

You can find an example project [here](https://github.com/BenPyton/DungeonExample) too.

## How to use it

Go to the [Getting Started](https://benpyton.github.io/ProceduralDungeon/guides/Installation) page to begin to work with the plugin.

If you want more details about how it works internally, you can read the [wiki](https://BenPyton.github.io/ProceduralDungeon/guides/Home).

You can also join the [Discord server][1] dedicated to this plugin if you want to ask question or get help from the community.

## Installation

Install it like any other Unreal Engine plugin.

If you have any trouble with installation, read the [Installation](https://benpyton.github.io/ProceduralDungeon/guides/Installation) page of the wiki.

## FAQ

[comment]: # (Move the FAQ in a wiki page instead?)

<details>
<summary><b>Can I generate levels during runtime? What I mean is if I can generate a new dungeon while the player is in it.</b></summary>

> Yes, you can generate during runtime.\
> If you call the `Generate` function, then the previous rooms unload, and a new dungeon generate and load new rooms.\
> There is no map travel during the process, the player remains in the master map, only the dungeon's rooms are loaded/unloaded.

</details>

<details>
<summary><b>How large can I make the map?</b></summary>

> You are only limited by the performance of the machine your game runs on.\
> Mostly, the performance of the dungeon depends on the complexity of your rooms/meshes, and the hardware of your computer. The more details and diversity of actors there are, the more resources will be consumed on the computer.\
> To be able to generate a very large map, you will need to optimize the meshes/textures for the RAM and GPU, the collisions and number of dynamic actors (enemies, etc.) for CPU, etc.\
> The simple occlusion culling system I provide in the plugin is one (rudimentary) way to optimize the GPU side (less drawing).
> It is far from perfect but a good start.\
> You will need to do the other optimizations yourself.

</details>

<details>
<summary><b>Can I save and load dungeons?</b></summary>

> There is no save/load system implemented in the plugin. You will need to do it yourself.\
> I made this plugin for a rogue-like game, so I didn't made a save/load system of the dungeon.\
> A starting point will be to save the dungeon seed, and any other dynamic actors of your game.\
> You would set an ID for each actors in the room's levels that need to be saved (using the dungeon's seed). Then when you load a game you can retrieve the actors from their IDs in the generated dungeon, and apply the saved data to them.

</details>

<details>
<summary><b>How many different rooms can I have?</b></summary>

> You can have the number of room you want, there is not really a limit in the plugin.

</details>

<details>
<summary><b>It is pretty much up to my creativity to design whatever I want, right? If I want rooms to have enemies or anything like that, I can just create it in the level, right?</b></summary>

> Yes, you can design everything you want in the room. It is the purpose of the plugin: providing a generic way to generate a dungeon, without any compromise on the DA nor the game design.\
> If you want to add dynamic actors (like enemies, chests, etc.) I would suggest you to create spawners that you place in your room's levels, and spawn those actors after the dungeon generation.\
> For game save, you should provide a unique id (seed) for each spawner based on the dungeon's seed, so the random spawning of enemies or chest loot will be deterministic from the dungeon's seed.\
> For multiplayer, you should do the spawning only on the server side. The spawn command will be sent to the clients and the actors spawned will then be replicated correctly.

</details>

<details>
<summary><b>How does the occlusion culling work for multiplayer?</b></summary>

> The occlusion culling is client side. It will show only the room where the local player is and any adjacent rooms.\
> You can read further details about the occlusion culling system of this plugin from the [wiki page](https://benpyton.github.io/ProceduralDungeon/guides/Occlusion-Culling).\
> You can also disable the occlusion culling from the [plugin's settings](https://benpyton.github.io/ProceduralDungeon/guides/Plugin-Settings) and do it yourself in another way.

</details>

<details>
<summary><b>Is there a seed?</b></summary>

> Yes, there is a seed for the dungeon generation.\
> I made a parameter in the `DungeonGenerator` actor to have different types of seed:
> - You can have a fixed seed you can set in the actor which will be always used (useful for testing and debugging purpose, or to set manually the seed from the game).
> - You can have an incrementing seed, using the fixed seed for the first generation, then adding a value to it at each generation (useful for demonstration purpose).
> - You can have a random seed generated for each generation (for released game mostly, or to test quickly a lot of dungeon generations).

</details>

<details>
<summary><b>Can I have some sort of flow to the dungeon? Like have a secret room spawn only once and have boss rooms only spawn 4 rooms?</b></summary>

> Yes, you can define the flow you want for your dungeon. It is the purpose of the plugin.\
> There is the function [`ChooseNextRoomData`](https://benpyton.github.io/ProceduralDungeon/guides/Choose-Next-Room-Data) where you define what I call your "rules" of the dungeon.\
> You can, for example, check a minimum number of room before spawning a secret room, and then don't spawn it if you already have one in the dungeon.

</details>

<details>
<summary><b>Can I increase the difficulty of the dungeon? Lets say room level 1 is easy and room level 5 is hard, can I tell the dungeon to not go from level 1 to level 5?</b></summary>

> Of course, you can. To do that sort of thing, you should create a child blueprint of `RoomData` class to add new parameters like a `DifficultyLevel`, which you can set value for each rooms in your `RoomData` assets.\
> Then for example, in your [`ChooseNextRoomData`](https://benpyton.github.io/ProceduralDungeon/guides/Choose-Next-Room-Data) function you can choose a room depending on its difficulty level compared to the difficulty level of the previous room.

</details>

## License

This plugin is under [MIT license](LICENSE).\
This means you are free to use this plugin for personal/free/commercial projects, you are also allowed to modify the source code and/or redistribute it.\
The only condition is to add the copyright notice and a copy of the license with your project and/or any redistribution of the source code, modified or not.

## *Support Me*

 If you like my plugin, please consider tipping:

[![Ko-fi](https://img.shields.io/badge/Ko--fi-ff5f5f?style=for-the-badge)](https://ko-fi.com/M4M3NW2JV)
[![liberapay](https://img.shields.io/badge/liberapay-f6c915?style=for-the-badge)](https://liberapay.com/BenPyton/donate)
[![PayPal](https://img.shields.io/badge/PayPal-142c8e?style=for-the-badge)](https://www.paypal.com/donate/?hosted_button_id=9VWP66JU5DZXN)

[1]: https://discord.gg/YE2dPda2CC
