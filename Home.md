# Procedural Dungeon Plugin

### Overview
This is an Unreal Engine 4 plugin that let you create procedural dungeon like "The Binding of Isaac" or "Rogue Legacy" but in 3D. \
This plugin is an engine that let you create your dungeon with your own rules.

### Lifecycle of a dungeon generation
Before creating your rules, you should understand how the plugin work.\
A generation is made of 4 phases : 
- Unloading the previous dungeon (destroying actors, etc.)
- Generating a virtual dungeon (here are your rules)
- Loading the dungeon (spawning actors, etc.)
- Initializing the dungeon

[[Images/ProceduralDungeonLifecycle.png]]

### What you need to do
First, read the [[Installation]] page to install the plugin.\
Then, read and follow the instructions in the [[Dungeon-Generator]] page.

You can get the [example project in the release page](https://github.com/BenPyton/ProceduralDungeon/releases/tag/v2.0.1) if you want a minimal working project.

### Tips
The generation take a certain time (the generation itself is done on one frame, but the instantiation and loading of each room take some times proportionally to the number and complexity of the rooms).<br/>
So, if you have the player character spawned at start, it will fall into the void of the universe before the dungeon has the time to generate.

To manage this situation, you have multiple solutions:
- The easy and ugly way is to place a little plane somewhere with the player start on it, and disable the inputs from the player to prevent him to fall from this plane, then when the generation is complete you teleport the player into the firstroom and unlock the inputs;
- The better and cleaner way is to spawn the player as spectator until the generation is finished, and then you spawn the character actor in the first room (or anywhere else if you want to) and make it possessed by the player controller.

Both solutions use the `Post Generation Event` to know when the generation is done.
You will also want to show or keep a loading screen when you start your game map, and you hide it on this event too.


