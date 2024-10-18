---
pagination_next: guides/Installation
---

# Introduction

:::info[Prerequisites]

This guide assumes you are familiar with [Actors](https://dev.epicgames.com/documentation/en-us/unreal-engine/actors-and-geometry-in-unreal-engine), [Levels](https://dev.epicgames.com/documentation/en-us/unreal-engine/levels-in-unreal-engine) and [Blueprint Scripting](https://dev.epicgames.com/documentation/en-us/unreal-engine/blueprints-visual-scripting-in-unreal-engine)  in Unreal engine.\
Also, if you are a new to programming languages in general, you should read [the best practices](Best-Practices) section.

:::

:::warning[Before using the plugin]

The procedural level generation in any game is part of the level design step of game development.\
So, before starting to use this plugin, I strongly recommend you to first implement your [3C's](Best-Practices/Workflows/3Cs.md) in Unreal Engine.\
Because the very first settings of the plugin you will define should be based on 2 of the 3 C's:

- how your character moves (speed, jump, attacks, etc.)
- your camera point of view (first person, third person, top-down, etc.)

:::

## Overview of the Plugin

This plugin generates dungeons by assembling multiple Unreal levels (called room level) inside a top level (called "master" level).

Room levels are hand created, the plugin will not generate the content inside the room levels.\
However, you can use other plugins like Procedural Content Generation (PCG) to create the inside of the rooms.

:::note

As of now, the dungeons are generated only at runtime, you will not be able to generate a dungeon in editor and save it as a level (perhaps available in a future version?).

:::

## What you will have to do

This plugin is not usable out of the box, you will have to setup lots of things before seeing a dungeon generated.

Here a list of what you will have to setup:

1. Define the plugin's settings like the room unit, door size, etc. depending on your game design.
2. Create a child blueprint of `Room Data` with your game specific data (optional but strongly recommended)
3. Create some room levels with their corresponding `Room Data` assets.
4. Create a `Dungeon Generator` child blueprint and override the functions in it to write the generation code.
5. Create a master level (not a room level) and place your `Dungeon Generator` actor in it.
6. Call `Generate` in this actor from somewhere in your code.

These basic tutorials will guides you through all of this process.

You can navigate to the next steps using the buttons below.
