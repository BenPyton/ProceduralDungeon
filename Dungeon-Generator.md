# Dungeon Generator

### Create a dungeon generator

The dungeon generator is an actor placed in your main level and which generates the dungeon and spawns the room levels.\
To create one, create a new blueprint, expand "All Classes" and type "DungeonGenerator".

The dungeon generator don't work directly by itself, you have to [[create data and levels for your rooms|Room Architecture]] first.

To be able to generate a dungeon, you also have to override some minimal functions:
- [[Choose First Room Data]]
- [[Choose Next Room Data]]
- [[Continue To Add Room]]
- [[Is Valid Dungeon]]
- [[Choose Door]]

There are some other functions you can override:
- **Pre Generation**: Called after unloading previous dungeon but before generating new dungeon.
- **Post Generation**: Called after loading and initializing all rooms of the dungeon. Use it to spawn or teleport players in the first room.
- **Generation Init**: Called to reset all generation data. It is called each time  the generation start over when IsValidDungeon returns false.
- **On Room Added**: Called when a room is successfully added to the dungeon (but not spawned yet). The added room is destroyed when IsValidDungeon returns false (you have to use GenerationInit to reset changed variables from this).

### Other functions you can use

**Generate**\
Launch the generation on all clients when called by the server, do nothing when called by clients.

**Has Already Room Data**\
This function take a RoomData Class Reference and return true if the dungeon contain a room of that class.

**Has Already One Room Data From**\
This function take a list of RoomData Class Reference and return true if the dungeon contain at least one room of one of these classes.

**Count Room Data**\
This function take a RoomData Class Reference and return the number of room in the dungeon from that class.

**Count Total Room Data**\
This function take a list of RoomData Class Reference and return the total number of room in the dungeon from all the classes in the list.

**Get Random Room Data**\
Take an array of RoomData Class Reference and return deterministically a random RoomData in it.

**Get Nb Room**\
Return the current number of room during the generation, or total number of room after generation.

### Generator settings
The DungeonGenerator has some settings:\
[[Images/GeneratorSettings.jpg]]

**Generation Type**\
There are 2 types of generations: the depth first and breadth first.\
The depth first will prioritize the last added room to continue the generation. 
This will result in a dungeon more linear with shorter dead ends.\
The breadth first will prioritize the first added room to continue the generation. This will result in a more spread dungeon.\

This is an example using the same very basic generation rules defined in this wiki. We just set a counter of 20 rooms to add the end room. So with depth first we have a lot of rooms to reach the end but dead-ends of only 1 room each. And with the breadth first, we have less rooms until the end, but the dead-ends are the same length of the path to reach the end (some dead-ends are shorter because the algorithm can't place a room after).\
[[Images/DepthBreadth.jpg]]

**Seed Type**\
You can choose how the seed is generated:
- Random: at each generation, the seed is randomly chosen.
- Auto Increment: the initial seed is used the first time, then a fixed number will be added at each generation. This will produce different dungeon at each generation, but you will have the same generation sequence each time you play.
- Fixed: the initial seed is used each time you generate, so you will always get the same dungeon.

**Seed**\
The initial seed you want to use if the `Seed Type` is not `Random`

**Use Generator Transform**\
If checked, the dungeon will be offset and rotated using the dungeon generator actor's transform.
If unchecked, the first room will be placed at world's (0,0,0) and no rotation will be applied.
