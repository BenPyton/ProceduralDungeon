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
- [[Continue Adding Room]]



