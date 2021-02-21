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
First of all, since a dungeon is composed of a set of room, you have to [[create your room levels and room data|Room Architecture]].

In order to get your dungeon generating, you have to override at least those function in your DungeonGenerator blueprint:
- [[Choose First Room Data]]
- [[Choose Next Room Data]]
- [[Is Valid Dungeon]]
- [[Continue To Add Room]]
- [[Choose Door]]

### Tips
The generation take a certain time (the generation itself is done on one frame, but the instantiation and loading of each room take some times proportionally to the number and complexity of the rooms).<br/>
So, if you have the player character spawned at start, it will fall into the void of the universe before the dungeon has the time to generate.

To manage this situation, you have 2 solutions:
- The easy and ugly way is to place a little plane somewhere with the player start on it, and disable the inputs from the player to prevent him to fall from this plane, then when the generation is complete you teleport the player into the firstroom and unlock the inputs;
- The better and cleaner way is to not spawn the player until the generation is finished, and you place a player start in your first room and trigger the spawn of the player when the generation is finished.

Both solutions use the `Post Generation Event` to know when the generation is done.
You will also want to show a loading screen when you start your game map which you hide on this event too.


