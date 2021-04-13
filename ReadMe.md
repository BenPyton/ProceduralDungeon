# Procedural Dungeon Plugin

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

**For blueprint only projects**\
__Before plugin version 1.2.1__:\
Upgrade your project to a C++ project and follow the instruction for C++ project below, or you wont be able to package your game.\
(To do that, download Visual Studio 2019 and create a new C++ class from the Unreal Editor)

Since plugin version 1.2.1:\
You have to extract the plugin inside your Unreal engine `Plugins` folder.\
For instance: `C:\Program Files\Epic Games\UE_4.26\Engine\Plugins`

**For C++ projects**\
Since the plugin version 1.2.1, you can install the plugin in the engine like for blueprint projects.\
You can also extract the plugin in the `Plugins` folder of your project.\
For instance: `D:\PathToYourProject\Plugins`