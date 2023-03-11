# Procedural Dungeon Plugin

[![Actively Maintained](https://img.shields.io/badge/Maintenance%20Level-Actively%20Maintained-green.svg)](https://gist.github.com/cheerfulstoic/d107229326a01ff0f333a1d3476e068d)

Author : Benoît PELLETIER

## Overview
This is an Unreal Engine 4 plugin to create procedural dungeons by arranging a set of room like "The Binding of Isaac" or "Rogue Legacy" but in 3D.

This procedural dungeon uses hand-created rooms that are Unreal levels.

You define your own rules to generate the dungeon.

If you want more details about how it works and how to use it, please read the [wiki](https://github.com/BenPyton/ProceduralDungeon/wiki).

If you have any suggestion/bug or you want to contribute, don't hesitate to add an issue or do a pull request.

### Demo in video:
[![](http://img.youtube.com/vi/DmyNEd0YtDE/0.jpg)](http://www.youtube.com/watch?v=DmyNEd0YtDE "Procedural Dungeon Demo")<br>

## Installation
Get the [latest release](https://github.com/BenPyton/ProceduralDungeon/releases) compatible with your Unreal version.

**For blueprint only projects**
- Before plugin version 1.2.1:\
Upgrade your project to a C++ project and follow the instruction for C++ project below, or you wont be able to package your game.\
(To do that, download Visual Studio 2019 and create a new C++ class from the Unreal Editor)

- Since plugin version 1.2.1:\
You have to extract the plugin inside your Unreal engine `Plugins` folder.\
For instance: `C:\Program Files\Epic Games\UE_4.26\Engine\Plugins`

**For C++ projects**\
Since the plugin version 1.2.1, you can install the plugin in the engine like for blueprint projects.\
You can also extract the plugin in the `Plugins` folder of your project.\
For instance: `D:\PathToYourProject\Plugins`

## FAQ

<details>
<summary><b>Can I generate levels during runtime? What I mean is if I can generate a new dungeon while the player is in it.</b></summary>

Yes, you can generate during runtime.\
If you call the `Generate` function, then the previous rooms unload, and a new dungeon generate and load new rooms.\
There is no map travel during the process, the player remains in the master map, only the dungeon's rooms are loaded/unloaded.

</details>

<details>
<summary><b>How large can I make the map?</b></summary>

You are only limited by the performance of the machine your game runs on.\
Mostly, the performance of the dungeon depends on the complexity of your rooms/meshes, and the hardware of your computer. The more details and diversity of actors there are, the more resources will be consumed on the computer.\
To be able to generate a very large map, you will need to optimize the meshes/textures for the RAM and GPU, the collisions and number of dynamic actors (enemies, etc.) for CPU, etc.\
The simple occlusion culling system I provide in the plugin is one (rudimentary) way to optimize the GPU side (less drawing).
It is far from perfect but a good start.\
You will need to do the other optimizations yourself.

</details>

<details>
<summary><b>Can I save and load dungeons?</b></summary>

There is no save/load system implemented in the plugin. You will need to do it yourself.\
I made this plugin for a rogue-like game, so I didn't made a save/load system of the dungeon.\
A starting point will be to save the dungeon seed, and any other dynamic actors of your game.\
You would set an ID for each actors in the room's levels that need to be saved (using the dungeon's seed). Then when you load a game you can retrieve the actors from their IDs in the generated dungeon, and apply the saved data to them.

</details>

<details>
<summary><b>How many different rooms can I have?</b></summary>

You can have the number of room you want, there is not really a limit in the plugin.

</details>

<details>
<summary><b>It is pretty much up to my creativity to design whatever I want, right? If I want rooms to have enemies or anything like that, I can just create it in the level, right?</b></summary>

Yes, you can design everything you want in the room. It is the purpose of the plugin: providing a generic way to generate a dungeon, without any compromise on the DA nor the game design.\
If you want to add dynamic actors (like enemies, chests, etc.) I would suggest you to create spawners that you place in your room's levels, and spawn those actors after the dungeon generation.\
For game save, you should provide a unique id (seed) for each spawner based on the dungeon's seed, so the random spawning of enemies or chest loot will be deterministic from the dungeon's seed.\
For multiplayer, you should do the spawning only on the server side. The spawn command will be sent to the clients and the actors spawned will then be replicated correctly.

</details>

<details>
<summary><b>How does the occlusion culling work for multiplayer?</b></summary>

The occlusion culling is client side. It will show only the room where the local player is and any adjacent rooms.\
You can read further details about the occlusion culling system of this plugin from the [wiki page](https://github.com/BenPyton/ProceduralDungeon/wiki/Occlusion-Culling).\
You can also disable the occlusion culling from the [plugin's settings](https://github.com/BenPyton/ProceduralDungeon/wiki/Plugin-Settings) and do it yourself in another way.

</details>

<details>
<summary><b>Is there a seed?</b></summary>

Yes, there is a seed for the dungeon generation.\
This is what is sent from server to clients in multiplayer. The clients also generate the dungeon on their side from the same seed. So, the actors and meshes inside the room levels are not linked over the network and then not replicated in multiplayer. So this is why I suggest you the spawner and ID system above.\
I made a parameter in the `DungeonGenerator` actor to have different types of seed:
- You can have a fixed seed you can set in the actor which will be always used (useful for testing and debugging purpose).
- You can have an incrementing seed, using the fixed seed for the first generation, then adding a value to it at each generation (useful for demonstration purpose).
- You can have a random seed generated for each generation (for released game mostly).

</details>

<details>
<summary><b>Can I have some sort of flow to the dungeon? Like have a secret room spawn only once and have boss rooms only spawn 4 rooms?</b></summary>

Yes, you can define the flow you want for your dungeon. It is the purpose of the plugin.\
There is the function [`ChooseNextRoomData`](https://github.com/BenPyton/ProceduralDungeon/wiki/Choose-Next-Room-Data) where you define what I call your "rules" of the dungeon.\
You can, for example, check a minimum number of room before spawning a secret room, and then don't spawn it if you already have one in the dungeon.

</details>

<details>
<summary><b>Can I increase the difficulty of the dungeon? Lets say room level 1 is easy and room level 5 is hard, can I tell the dungeon to not go from level 1 to level 5?</b></summary>

Of course, you can. To do that sort of thing, you should create a child blueprint of `RoomData` class to add new parameters like a `DifficultyLevel`, which you can set value for each rooms in your `RoomData` assets.\
Then for example, in your [`ChooseNextRoomData`](https://github.com/BenPyton/ProceduralDungeon/wiki/Choose-Next-Room-Data) function you can choose a room depending on its difficulty level compared to the difficulty level of the previous room.

</details>