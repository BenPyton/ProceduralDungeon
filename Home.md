---
title: Guides
sidebar_class_name: hidden
---

import DemoRoomSpawn from './Images/ProceduralDungeonDemo_RoomSpawn.gif';
import DemoRoomA from './Images/ProceduralDungeonDemo_RoomA.gif';
import DemoRoomB from './Images/ProceduralDungeonDemo_RoomB.gif';
import DemoRoomC from './Images/ProceduralDungeonDemo_RoomC.gif';
import DemoRoomD from './Images/ProceduralDungeonDemo_RoomD.gif';
import DemoRoomExit from './Images/ProceduralDungeonDemo_RoomExit.gif';
import DemoDungeon1 from './Images/ProceduralDungeonResult.gif';
import DemoDungeon2 from './Images/ProceduralDungeonResult2.gif';

# Procedural Dungeon Plugin

## Overview
This is an Unreal Engine plugin that let you create dungeons procedurally like "The Binding of Isaac" or "Rogue Legacy" but in 3D.\
With this plugin you create rooms by hand and you define your generation rules with blueprint.\
The plugin then generate a dungeon that can be random or deterministic following what rules you've programmed. 

## Example

Some hand-made rooms defined with bounds and doors:

<img src={DemoRoomSpawn} alt="RoomSpawn" width="125"/>
<img src={DemoRoomA} alt="RoomA" width="125"/>
<img src={DemoRoomB} alt="RoomB" width="125"/>
<img src={DemoRoomC} alt="RoomC" width="125"/>
<img src={DemoRoomD} alt="RoomD" width="125"/>
<img src={DemoRoomExit} alt="RoomExit" width="125"/>

Some very simple generation rules:
- A special room (red) used to spawn the player.
- Then 10 rooms chosen randomly in a list (blue, green, yellow, cyan).
- Then a special room (purple) used as an end goal for the player.
- Finally 10 other rooms chosen randomly.

Possible results:

<img src={DemoDungeon1} alt="Result1" width="400"/>
<img src={DemoDungeon2} alt="Result2" width="400"/>

## Lifecycle of a dungeon generation
Before creating your rules, you should understand how the plugin works.\
A generation is made of 4 phases : 
- **Idle**: the generator just wait until a change occurs
- **Unload**: the previous room levels unload (destroying level's actors, etc.)
- **Create Dungeon**: (*server only*) the generator creates a 'virtual' dungeon (your rules are defined here)
- **Load**: the room levels load and initialize (spawning actors, etc. and call to `BeginPlay` of room's level blueprints)

Here is the flowchart when we call `Generate` on a dungeon generator (or clients update their room list):

![](Images/Flowchart_Dark_v3.svg)

## What you need to do
First, read the [Installation](Installation.md) page to install the plugin.\
Then, read and follow the instructions in the [Getting Started](Creating-Rooms) page.

You can get an [example project here](https://github.com/BenPyton/DungeonExample) if you want a minimal working project.

You can also join the [Discord server](https://discord.gg/YE2dPda2CC) dedicated to this plugin if you want to ask question or get help from the community.

## Tips
The generation take a certain time (the generation itself is done on one frame, but the instantiation and loading of each room take some times proportionally to the number and complexity of the rooms).<br/>
So, if you have the player character spawned at start, it will fall into the void of the universe before the dungeon has the time to generate.

To manage this situation, you have multiple solutions:
- The easy and ugly way is to place a little plane somewhere with the player start on it, and disable the inputs from the player to prevent him to fall from this plane, then when the generation is complete you teleport the player into the first room and unlock the inputs;
- The better and cleaner way is to spawn the player as spectator until the generation is finished, and then you spawn the character actor in the first room (or anywhere else if you want to) and make it possessed by the player controller. (I've added a `Spectate` blueprint node to ease this solution)

Both solutions use the `Post Generation Event` to know when the generation is done.
You will also want to show or keep a loading screen when you start your game map, and you hide it on this event too.


