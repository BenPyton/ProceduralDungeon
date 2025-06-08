# Procedural Dungeon Plugin

[![Plugin version number](https://img.shields.io/github/v/release/BenPyton/ProceduralDungeon?label=Version)](https://github.com/BenPyton/ProceduralDungeon/releases/latest)
[![Unreal Engine Supported Versions](https://img.shields.io/badge/Unreal_Engine-4.27_%7C_5.3_%7C_5.4_%7C_5.5_%7C_5.6-9455CE?logo=unrealengine)](https://github.com/BenPyton/ProceduralDungeon/releases)
[![License](https://img.shields.io/badge/License-CeCILL--C_or_Fab_EULA-blue)](LICENSE)
![Download count](https://img.shields.io/github/downloads/BenPyton/ProceduralDungeon/total?label=Downloads)
[![Actively Maintained](https://img.shields.io/badge/Maintenance%20Level-Actively%20Maintained-green.svg)](https://gist.github.com/cheerfulstoic/d107229326a01ff0f333a1d3476e068d)
[![Discord](https://img.shields.io/discord/1182461404092055574?logo=discord&logoColor=white&label=Discord&color=%235865F2)][Discord]

## Overview

This Unreal Engine plugin allows you to create rich, diverse dungeon experiences that combine the best of both worlds: the creativity and precision of handmade room designs, paired with the unpredictability and excitement of procedural generation.

Rooms are handmade Unreal levels instanced in the world.
You define their size and their doors as well as several other parameters.
You code your own rules in blueprints or C++, with great flexibility and customization, to generate the dungeons ***you*** want.

If you have any bug or crash, please open an issue in the Github repo.\
If you have suggestions, questions or need help to use the plugin you can join the [Discord server][Discord] dedicated to this plugin.\
If you want to contribute, feel free to create a pull request (*contributions to the wiki are also welcomed!*).

## Features

- Handcrafted rooms, giving full control of their design by the level designers.
- Generation rules defined in blueprint or C++, allowing flexible and powerful procedural generation.
- A new editor mode to ease the creation and edition of the rooms.
- Several interfaces and components for your actors (RoomVisitor, RoomObserver, DeterministicRandom, etc.)
- Different door types, allowing more complex dungeons.
- Optional room culling system, allowing to render only the relevant rooms to the player.
- Ready for multiplayer games (push model and subobject lists are implemented).
- :construction:[^experimental] Save/Load nodes for the dungeon, easy to use with any game save system (blueprint or C++)

[^experimental]: :construction: : Experimental features

## Example

Some hand-made rooms defined with bounds and doors:\
<img src="https://github.com/BenPyton/ProceduralDungeon/wiki/Images/ProceduralDungeonDemo_RoomSpawn.gif" alt="Animated GIF" width="125"/>
<img src="https://github.com/BenPyton/ProceduralDungeon/wiki/Images/ProceduralDungeonDemo_RoomA.gif" alt="Animated GIF" width="125"/>
<img src="https://github.com/BenPyton/ProceduralDungeon/wiki/Images/ProceduralDungeonDemo_RoomB.gif" alt="Animated GIF" width="125"/>
<img src="https://github.com/BenPyton/ProceduralDungeon/wiki/Images/ProceduralDungeonDemo_RoomC.gif" alt="Animated GIF" width="125"/>
<img src="https://github.com/BenPyton/ProceduralDungeon/wiki/Images/ProceduralDungeonDemo_RoomD.gif" alt="Animated GIF" width="125"/>
<img src="https://github.com/BenPyton/ProceduralDungeon/wiki/Images/ProceduralDungeonDemo_RoomExit.gif" alt="Animated GIF" width="125"/>

Some extremely simple generation rules:

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

Follow the [Getting Started guide on the wiki](https://benpyton.github.io/ProceduralDungeon/guides/Introduction) to start working with the plugin.

If you want more details about how it works internally, you can read the [wiki](https://benpyton.github.io/ProceduralDungeon/guides/Home).

You have also access in the wiki to all the exposed [classes and nodes](https://benpyton.github.io/ProceduralDungeon/api) in Blueprint.

You can also join the [Discord server][Discord] dedicated to this plugin if you want to ask question or get help from the community.

## Installation

Install it like any other Unreal Engine plugin.

If you have any trouble with installation, read the [Installation](https://benpyton.github.io/ProceduralDungeon/guides/Getting-Started/Installation) page of the wiki.

## FAQ

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

> Since version 3.5 of the plugin, There are [some nodes](https://benpyton.github.io/ProceduralDungeon/guides/Advanced-Features/Saving-Dungeon) to help you easily setup a save/load of the dungeon.
> It'll need some works on your side but it is definitely possible to do it.\
> The dungeon save should be compatible with any save system you are using.
> In C++ you can also use some functions for archive-based save systems ( `StructuredArchive` compatible too).

</details>

<details>
<summary><b>How many different rooms can I have?</b></summary>

> You can have the number of room you want, there is not really a limit in the plugin.

</details>

<details>
<summary><b>It is pretty much up to my creativity to design whatever I want, right? If I want rooms to have enemies or anything like that, I can just create it in the level, right?</b></summary>

> Yes, you can design everything you want in the room. It is the purpose of the plugin: providing a generic way to generate a dungeon, without any compromise on the DA nor the game design.\
> If you don't want to create the rooms manually, you may use other procedural plugins (like PCG) to create the content of the rooms (I've never tested that myself though).

</details>

<details>
<summary><b>How does the room culling work for multiplayer?</b></summary>

> The room culling system built in the plugin is client side. It will show only the room where the local player is and any adjacent rooms.\
> You can read further details about the room culling system of this plugin from the [wiki page](https://benpyton.github.io/ProceduralDungeon/guides/Advanced-Features/Occlusion-Culling).\
> You can also disable the room culling system from the [plugin's settings](https://benpyton.github.io/ProceduralDungeon/guides/Getting-Started/Plugin-Settings) and do it yourself in another way.

</details>

<details>
<summary><b>Is there a seed?</b></summary>

> Yes, there is a seed for the dungeon generation.\
> I made a parameter in the [`DungeonGenerator`](https://benpyton.github.io/ProceduralDungeon/guides/Getting-Started/Generating-Dungeon/Dungeon-Generator#seed-type) actor to have different types of seed:
>
> - You can have a fixed seed you can set in the actor which will be always used (useful for testing and debugging purpose, or to set manually the seed in Blueprint or C++).
> - You can have an incrementing seed, using the fixed seed for the first generation, then adding a value to it at each generation (useful for demonstration purpose).
> - You can have a random seed generated for each generation (for released game mostly, or to test quickly a lot of dungeon generations).

</details>

<details>
<summary><b>Can I have some sort of flow to the dungeon? Like have a secret room spawn only once and have boss rooms only spawn 4 rooms?</b></summary>

> Yes, you can define the flow you want for your dungeon. It is the purpose of the plugin.\
> There is the function [`ChooseNextRoomData`][ChooseNextRoom] where you define what I call your "rules" of the dungeon.\
> You can, for example, check a minimum number of room before spawning a secret room, and then don't spawn it if you already have one in the dungeon.\
> If you need help on how to define your dungeon rules, you can check this [example](https://benpyton.github.io/ProceduralDungeon/guides/Best-Practices/Workflows/Dungeon-Generation-Algorithm) and get help on the [Discord server][Discord] dedicated to this plugin.

</details>

<details>
<summary><b>Can I increase the difficulty of the dungeon? Lets say room level 1 is easy and room level 5 is hard, can I tell the dungeon to not go from level 1 to level 5?</b></summary>

> Of course, you can do that sort of thing!
> For this difficulty example, you should create a child blueprint of `RoomData` class to add new parameters like a `DifficultyLevel`, which you can set a different value for each room in your `RoomData` assets.\
> Then for example, in your [`ChooseNextRoomData`][ChooseNextRoom] function you can choose a room depending on its difficulty level compared to the difficulty level of the previous room.

</details>

## License

This plugin is available under different licenses depending on the source from which it was obtained:

- The [Fab EULA](https://fab.com/eula) applies when obtained from the Fab marketplace.
- The [CeCILL-C license](https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.

Please refer to the accompanying [LICENSE file](LICENSE) for further details.

## *Support Me*

 If you like my plugin, please consider leaving a tip, it would mean so much to me! 😊

[![Ko-fi](https://img.shields.io/badge/Ko--fi-FF5E5B?logo=kofi&logoColor=fff&style=for-the-badge)](https://ko-fi.com/M4M3NW2JV)
[![Buy Me a Coffee](https://img.shields.io/badge/Buy%20Me%20a%20Coffee-FFDD00?logo=buymeacoffee&logoColor=000&style=for-the-badge)](https://buymeacoffee.com/benpyton)
[![Liberapay](https://img.shields.io/badge/Liberapay-F6C915?logo=liberapay&logoColor=000&style=for-the-badge)](https://liberapay.com/BenPyton/donate)
[![PayPal](https://img.shields.io/badge/PayPal-003087?logo=paypal&logoColor=fff&style=for-the-badge)](https://www.paypal.com/donate/?hosted_button_id=9VWP66JU5DZXN)

[Discord]: https://discord.gg/YE2dPda2CC
[ChooseNextRoom]: https://benpyton.github.io/ProceduralDungeon/guides/Getting-Started/Generating-Dungeon/Choose-Next-Room-Data
